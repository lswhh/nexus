#include <aclSafeList.h>
#include <ace.h>

#define ACL_SAFELIST_MARKER ((acp_ulong_t)(0x01))

#define ACL_SAFELIST_LIVENODE ((acp_ulong_t)(0x00))
#define ACL_SAFELIST_DEADNODE ((acp_ulong_t)(0x01))

#define ACL_SAFELIST_FREE_LEVEL_INIT            (0)
#define ACL_SAFELIST_FREE_LEVEL_CHECK_COUNTER   (1)
#define ACL_SAFELIST_FREE_LEVEL_CLEAN           (2)

#define ACL_SAFELIST_NO    (0)
#define ACL_SAFELIST_YES   (1)

/* garbage collect cycle */
#define ACL_SAFELIST_GC_INTERVAL_IN_FREECOUNT     (100)

#define ACL_SAFELIST_GETMARKER(aRef)                            \
    ((acp_ulong_t)aRef & ACL_SAFELIST_MARKER)
#define ACL_SAFELIST_GETPOINTER(aRef)                           \
    (acl_safelist_node_t*)((acp_ulong_t)aRef & ~ACL_SAFELIST_MARKER)

#define ACL_SAFELIST_MAKEREF(aRef, aMark)                       \
    (acl_safelist_node_t*)((aMark == ACL_SAFELIST_LIVENODE)?        \
     ((acp_ulong_t)aRef & ~ACL_SAFELIST_MARKER) :               \
     ((acp_ulong_t)aRef | ACL_SAFELIST_MARKER))

#define ACL_SAFELIST_GETNEXT(aNode)                   \
            ACL_SAFELIST_GETPOINTER(acpAtomicGet(&(aNode->mNext)))

#define ACL_SAFELIST_GETPREV(aNode)                   \
        ((acl_safelist_node_t*)(acpAtomicGet(&aNode->mPrev)))

#define ACL_SAFELIST_SETNEXT(aNode, aPointer, aFlag)            \
        (void)acpAtomicSet(&(aNode->mNext),                     \
                           (acp_slong_t)ACL_SAFELIST_MAKEREF(aPointer, aFlag))
#define ACL_SAFELIST_SETPREV(aNode, aPointer)                   \
        (void)acpAtomicSet(&(aNode->mPrev), (acp_slong_t)aPointer)

#define ACL_SAFELSIT_IS_EMPTY(aList) \
        ((aList->mHead.mNext) == (&(aList->mTail))? ACP_TRUE : ACP_FALSE)

acp_uint32_t gFreeCount = 0;

ACP_EXPORT acp_bool_t aclSafeListCASNext(
    acl_safelist_node_t* aThisNode,
    acl_safelist_node_t* aOldRef, acp_ulong_t aOldMark,
    acl_safelist_node_t* aNewRef, acp_ulong_t aNewMark)
{
    acp_bool_t  sRC = ACP_FALSE;

    sRC = acpAtomicCasBool(&(aThisNode->mNext),
           (acp_slong_t)ACL_SAFELIST_MAKEREF(aNewRef, aNewMark),
           (acp_slong_t)ACL_SAFELIST_MAKEREF(aOldRef, aOldMark)
          ); 

    return sRC;
}

ACP_EXPORT void aclSafeListInsertFreeList(acl_safelist_t* aList,
                                          acl_safelist_node_t* aOldNode)
{
    acp_list_node_t* sNewNode = NULL;
    acp_list_node_t* sNext = NULL;

    ACE_DASSERT(aclSafeListIsNodeAlive(aOldNode) == ACP_FALSE);

    /* add aOldRef to free list, no return error */
    acpListInitObj(&(aOldNode->mFreeNode), aOldNode);

    while(ACP_TRUE)
    {
        /*acpListGetNextNode(aList->mFreeList*/
        sNext = (acp_list_node_t*)acpAtomicGet(&(aList->mFreeList.mNext));

        /* Link new node with proper position */
        acpAtomicSet(&(aOldNode->mFreeNode.mNext), (acp_slong_t)sNext);
        acpAtomicSet(&(aOldNode->mFreeNode.mPrev), (acp_slong_t)&(aList->mFreeList));

        /* Splice into list */
        if(acpAtomicCasBool(&(aList->mFreeList.mNext),
                            (acp_slong_t)&(aOldNode->mFreeNode),
                            (acp_slong_t)sNext) == ACP_TRUE)
        {
            /* We're done! */
            acpAtomicSet(&(sNext->mPrev), (acp_slong_t)&(aOldNode->mFreeNode));
            (void)acpAtomicInc32(&(aList->mFreeNodeCnt));
            break;
        }
        else
        {
            /*
             * Someone touched the list.
             * Try again.
             */
        }
    }
}

/**
 * Is @a aNode alive?
 * @return #ACP_TRUE if @a aNode is alive, #ACP_FALSE if dead
 */
ACP_EXPORT acp_bool_t aclSafeListIsNodeAlive(acl_safelist_node_t* aNode)
{
    return (ACL_SAFELIST_GETMARKER(acpAtomicGet(&(aNode->mNext)))
            == ACL_SAFELIST_LIVENODE)? ACP_TRUE:ACP_FALSE;
}

/*
 * Update mPrev according to mNext.
 * This make the mPrev link correspondent to the predecessor's mNext
 */
ACP_EXPORT acp_rc_t    aclSafeListCreate(acl_safelist_t* aList)
{
    aList->mHead.mPrev = NULL;
    aList->mTail.mNext = NULL;
    aList->mHead.mNext = &(aList->mTail);
    aList->mTail.mPrev = &(aList->mHead);
    aList->mFreeNodeCnt    = 0;
    acpListInit(&(aList->mFreeList));
    aList->mDoingFreeFlag = ACL_SAFELIST_NO;
    aList->mFreeJobLevel  = ACL_SAFELIST_FREE_LEVEL_INIT;
    aList->mStartFreePos  = NULL;
    aList->mGCFlag        = ACL_SAFELIST_NO;
    aList->mInProgressCounter1 = 0;
    aList->mInProgressCounter2 = 0;

    return ACP_RC_SUCCESS;
}

ACP_EXPORT acp_rc_t aclSafeListDestroy(acl_safelist_t* aList)
{
    acp_rc_t sRC;
    acl_safelist_node_t*   sNode;
    acp_list_t*            sFreeList = &(aList->mFreeList);

    sRC = aclSafeListGarbageCollect(aList);
    ACP_TEST_RAISE(aList->mFreeNodeCnt != 1, ERRNOTEMPTY);

    aList->mStartFreePos = acpListGetNextNode(sFreeList);
    aList->mFreeJobLevel = ACL_SAFELIST_FREE_LEVEL_CLEAN;

    sRC = aclSafeListGarbageCollect(aList);
    ACP_TEST_RAISE(aList->mFreeNodeCnt != 0, ERRNOTEMPTY);

    ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));
    sRC = aclSafeListNextInternal(aList, &(aList->mHead), &sNode);
    ACP_TEST_RAISE(sNode != &aList->mTail, ERRNOTEMPTY);

    (void)acpPrintf(" freed success = %d\n", gFreeCount);
    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ERRNOTEMPTY)
    {
        sRC = ACP_RC_ENOTEMPTY;
    }

    ACP_EXCEPTION_END;
    (void)acpPrintf(" freed failed = %d\n", gFreeCount);
    return sRC;

}

ACP_EXPORT acp_rc_t aclSafeListGarbageCollect(acl_safelist_t* aList)
{
    acp_bool_t      sRCBool = ACP_FALSE;
    acp_bool_t      sIsAllVersionChange = ACP_FALSE;
    acp_rc_t        sRC;
    acp_uint32_t    i = 0;
    acl_safelist_node_t*    sFreeNode = NULL;
    acp_list_node_t*        sTmpNode  = NULL;
    acp_list_node_t*        sTmpNext = NULL;
    acp_list_t*             sFreeList = NULL;
    acp_uint16_t                sFreeJobLevel;
    acp_list_node_t*            sStartFreePos;
    acp_uint32_t            sInProgressCnt = ACP_UINT32_MAX; 

    /* only one thr garbage collect */
    sRCBool = acpAtomicCas16Bool(&(aList->mDoingFreeFlag), 
                                 ACL_SAFELIST_YES,
                                 ACL_SAFELIST_NO);

    ACP_TEST_RAISE(sRCBool != ACP_TRUE, ERRBUSY);

    sFreeList = &(aList->mFreeList);
    sFreeJobLevel = acpAtomicGet16(&(aList->mFreeJobLevel));

    /* do garbage collect */
    switch(sFreeJobLevel)
    {
        case ACL_SAFELIST_FREE_LEVEL_INIT:

            /*start free position get */
            /*두번째 이상부터 시작 지점으로 잡는다. free list가 safe가 아니므로*/
            sTmpNode = acpListGetFirstNode(&(aList->mFreeList));
            aList->mStartFreePos = acpListGetNextNode(sTmpNode);
            /*aList->mStartFreePos = acpListGetFirstNode(&(aList->mFreeList));
            aList->mStartFreePos = (acp_list_node_t*)acpAtomicGet(&(aList->mFreeList.mNext));*/
            /*GC Flag Set*/
            if(aList->mGCFlag != ACL_SAFELIST_YES)
            {
                if(acpAtomicGet32(&(aList->mInProgressCounter2)) == 0)
                {
                    acpAtomicSet32(&(aList->mGCFlag), ACL_SAFELIST_YES);
                }
                else 
                {
                    break; /*다른 노드가 이미 검색을 진입한 놈이 있어서 GC 진행 불가함*/
                }
            }
            else
            {
            }


            if(aList->mStartFreePos == sFreeList)
            {
                /* free list empty*/
                acpAtomicSet32(&(aList->mGCFlag), ACL_SAFELIST_NO);
                break;
            }
            else
            {
                /*do nothing*/
            }

            /*free job level change*/
            acpAtomicSet16(&(aList->mFreeJobLevel), ACL_SAFELIST_FREE_LEVEL_CHECK_COUNTER);

        case ACL_SAFELIST_FREE_LEVEL_CHECK_COUNTER:

            sInProgressCnt = acpAtomicGet32(&(aList->mInProgressCounter1));
            
            /* no more threads in progress, free ok */
            if(sInProgressCnt == 0)
            {
                acpAtomicSet16(&(aList->mFreeJobLevel), ACL_SAFELIST_FREE_LEVEL_CLEAN);
                acpAtomicSet32(&(aList->mGCFlag), ACL_SAFELIST_NO);
            }
            else
            {
                break;
            }

        case ACL_SAFELIST_FREE_LEVEL_CLEAN:
            /*ok free, ref count*/
            sTmpNode = (acp_list_node_t*)acpAtomicGet(&(aList->mStartFreePos));

            while(sTmpNode != sFreeList) /*tail까지 모두 처리했음*/
            {
                sFreeNode = (acl_safelist_node_t*)sTmpNode->mObj;

                /*ok free*/
                if(acpAtomicGet32(&(sFreeNode->mRefCnt)) <= 0)
                {
                    /*data free*/
                    acpMemFree(sFreeNode->mData);
                    sFreeNode->mData = NULL;
                    sFreeNode->mPrev = sFreeNode->mNext = NULL;
                    sTmpNode->mObj = NULL;

                    if(sTmpNode == aList->mStartFreePos)
                    {
                        acpAtomicSet(&(aList->mStartFreePos), (acp_slong_t)sTmpNode->mNext);
                    }
                    else
                    {
                    }

                    /*free node delete acpListDeleteNode(acp_list_node_t *aNodeToDelete)*/
                    acpListDeleteNode(sTmpNode);
                    sTmpNode = (acp_list_node_t*)acpAtomicGet(&(sTmpNode->mNext));
                    acpMemFree(sFreeNode);
                    acpAtomicDec32(&(aList->mFreeNodeCnt));
                    acpAtomicInc32(&gFreeCount);
                }
                else 
                {
                    /*do nothing*/
                    sTmpNode = (acp_list_node_t*)acpAtomicGet(&(sTmpNode->mNext));
                }

            }
            if(aList->mStartFreePos == sFreeList)
            { 
                aList->mStartFreePos = NULL;
                acpAtomicSet16(&(aList->mFreeJobLevel), ACL_SAFELIST_FREE_LEVEL_INIT);
            }
            break;
        
        default:
            break;
    }

    (void) acpAtomicCas16Bool(&(aList->mDoingFreeFlag), 
                                 ACL_SAFELIST_NO,
                                 ACL_SAFELIST_YES);
    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ERRBUSY)
    {
        sRC = ACP_RC_EBUSY;
    }
    ACP_EXCEPTION_END;

    return sRC;
}


/**
 * Next node of @a aAnchorNode.
 * Finds next node of @a aAnchorNode and store it into @a *aNode.
 * @param aList Thread-safe doubly linked list structure
 * @param aAnchorNode Node to find the next node of which
 * @param aNode Node pointer to store next node of @a aAnchorNode
 * @return #ACP_RC_SUCCESS Finding successful
 * @return #ACP_RC_EINVAL @a aList, @a aAnchorNode or @a aNode is NULL
 * @return #ACP_RC_ENOENT No next node of @a aAnchorNode
 */
ACP_EXPORT acp_rc_t aclSafeListNext(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;
    acp_uint32_t            sLocalGCFlag = ACL_SAFELIST_NO;

    /*get local GC Flag */
    sLocalGCFlag = acpAtomicGet32(&(aList->mGCFlag));

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicInc32(&(aList->mInProgressCounter1));
    }
    else
    {
        /* nothing to do */
        acpAtomicInc32(&(aList->mInProgressCounter2));
    }

    sRC = aclSafeListNextInternal(aList, aAnchorNode, aNode);
    ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));
    /* Live node encountered */
    (void)acpAtomicInc32(&((*aNode)->mRefCnt));

    /* no more acces to AnchorNode */ 
    (void)acpAtomicDec32(&(aAnchorNode->mRefCnt));

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicDec32(&(aList->mInProgressCounter1));
    }
    else
    {
        /* nothing to do */
        acpAtomicDec32(&(aList->mInProgressCounter2));
    }

    if(aList->mFreeNodeCnt >= ACL_SAFELIST_GC_INTERVAL_IN_FREECOUNT)
    {
        (void)aclSafeListGarbageCollect(aList);
    }

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION_END;

    return sRC;
}

ACP_EXPORT acp_rc_t aclSafeListNextInternal(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;
    acl_safelist_node_t*    sNext;
    acl_safelist_node_t*    sAnchorNext;
    acl_safelist_node_t*    sTmpNext;

    ACP_TEST_RAISE((aList == NULL || aAnchorNode == NULL || aNode == NULL),
                   ENULL);
    ACP_TEST_RAISE(aAnchorNode == &(aList->mTail), EBEYONDLIST);

    sAnchorNext = sNext = ACL_SAFELIST_GETNEXT(aAnchorNode);

    while(ACP_TRUE)
    {
        /* End of list encountered */
        ACP_TEST_RAISE(sNext == NULL, EBEYONDLIST);

        if(aclSafeListIsNodeAlive(sNext) == ACP_TRUE)
        {
            /* Live node encountered */
            //ACP_TEST_RAISE(sNext == &(aList->mTail), EBEYONDLIST);
            break;
        }
        else
        {
            /* Passing through dead node. */
            sNext = ACL_SAFELIST_GETNEXT(sNext);
        }
    }

    /* 
     * sNext now contains the live next node of aAnchorNode
     * CAS Next of aAnchorNode
     * if aAnchorNode is already marked as dead, this CAS does nothing
     */
    if(aclSafeListCASNext(aAnchorNode,
                          sAnchorNext,   ACL_SAFELIST_LIVENODE,
                          sNext,         ACL_SAFELIST_LIVENODE)
       == ACP_TRUE)
    {
        ACL_SAFELIST_SETPREV(sNext, aAnchorNode);
        
        sTmpNext = sAnchorNext;
        while(ACP_TRUE)
        {
            /* End of list encountered */
            ACP_TEST_RAISE(sTmpNext == NULL, EBEYONDLIST);
    
            if(sTmpNext == sNext)
            {
                /* Live node encountered */
                break;
            }
            else
            {
                /* Passing through dead node. */
                aclSafeListInsertFreeList(aList, sTmpNext);
                sTmpNext = ACL_SAFELIST_GETNEXT(sTmpNext);
            }
        }
    }
    else
    {
        /* Do not update for dead node */
    }

    *aNode = sNext;

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ENULL)
    {
        sRC = ACP_RC_EINVAL;
    }
    ACP_EXCEPTION(EBEYONDLIST)
    {
        sRC = ACP_RC_ENOENT;
        /* no more acces to last node? */ 
        /*(void)acpAtomicDec32(&(aAnchorNode->mRefCnt));*/
    }

    ACP_EXCEPTION_END;
    return sRC;
}

static acp_rc_t aclSafeListPrevNext(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aBeginNode,
    acl_safelist_node_t*    aEndNode,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;

    acl_safelist_node_t*    sNext;
    acl_safelist_node_t*    sLive;
    acl_safelist_node_t*    sLiveNext;
    acl_safelist_node_t*    sTmpNext;

    ACP_UNUSED(aList);

    sLive = aBeginNode;
    sLiveNext = ACL_SAFELIST_GETNEXT(sLive);
    sNext = ACL_SAFELIST_GETNEXT(aBeginNode);

    while(sNext != aEndNode)
    {
        /* aBeginNode or aEndNode was deleted during search */
        ACP_TEST_RAISE(aclSafeListIsNodeAlive(aBeginNode) == ACP_FALSE,
                       ENOSUCHANCHOR);
        ACP_TEST_RAISE(aclSafeListIsNodeAlive(aEndNode) == ACP_FALSE,
                       ENOSUCHANCHOR);

        if(aclSafeListIsNodeAlive(sNext) == ACP_TRUE)
        {
/*
            (void)aclSafeListCASNext(sLive,
                                     sLiveNext, ACL_SAFELIST_LIVENODE,
                                     sNext,     ACL_SAFELIST_LIVENODE);
*/
            if(aclSafeListCASNext(sLive,
                                  sLiveNext, ACL_SAFELIST_LIVENODE,
                                  sNext,     ACL_SAFELIST_LIVENODE)
            == ACP_TRUE)
            {
                ACL_SAFELIST_SETPREV(sNext, sLive);
                
                sTmpNext = sLiveNext;
                while(ACP_TRUE)
                {
                    if(sTmpNext == sNext)
                    {
                        break;
                    }
                    else
                    {
                        aclSafeListInsertFreeList(aList, sTmpNext);
                        sTmpNext = ACL_SAFELIST_GETNEXT(sTmpNext);
                    }
                }
            }
            else
            {
                ACL_SAFELIST_SETPREV(sNext, sLive);
            }

            sLive = sNext;
            sLiveNext = ACL_SAFELIST_GETNEXT(sLive);
        }
        else
        {
            /* Pass dead node 여기도 추가해야함 insertFreeList*/ 
        }
            
        sNext = ACL_SAFELIST_GETNEXT(sNext);
    }

    /* sLive is the most-forward live node */
    *aNode = sLive;
    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ENOSUCHANCHOR)
    {
        sRC = ACP_RC_ESRCH;
    }

    ACP_EXCEPTION_END;
    return sRC;
}

/**
 * Previous node of @a aAnchorNode.
 * Finds previous node of @a aAnchorNode and store it into @a *aNode.
 * @param aList Thread-safe doubly linked list structure
 * @param aAnchorNode Node to find the previous node of which
 * @param aNode Node pointer to store previous node of @a aAnchorNode
 * @return #ACP_RC_SUCCESS Finding successful
 * @return #ACP_RC_EINVAL @a aList, @a aAnchorNode or @a aNode is NULL
 * @return #ACP_RC_ENOENT No previous node of @a aAnchorNode
 * @return #ACP_RC_ESRCH  @a aAnchorNode was deleted during search
 */
ACP_EXPORT acp_rc_t    aclSafeListPrev(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;
    acp_uint32_t            sLocalGCFlag = ACL_SAFELIST_NO;

    /*get local GC Flag */
    sLocalGCFlag = acpAtomicGet32(&(aList->mGCFlag));

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicInc32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicInc32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    sRC = aclSafeListPrevInternal(aList, aAnchorNode, aNode);
    ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));

    /* Live node encountered */
    (void)acpAtomicInc32(&((*aNode)->mRefCnt));

    /* no more acces to AnchorNode */
    (void)acpAtomicDec32(&(aAnchorNode->mRefCnt));

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicDec32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicDec32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    if(aList->mFreeNodeCnt >= ACL_SAFELIST_GC_INTERVAL_IN_FREECOUNT)
    {
        (void)aclSafeListGarbageCollect(aList);
    }

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION_END;
    return sRC;
}

ACP_EXPORT acp_rc_t    aclSafeListPrevInternal(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;
    acl_safelist_node_t*    sPrev;

    ACP_TEST_RAISE((aList == NULL || aAnchorNode == NULL || aNode == NULL),
                   ENULL);
    ACP_TEST_RAISE(aAnchorNode == &(aList->mHead), EBEYONDLIST);

    sPrev = ACL_SAFELIST_GETPREV(aAnchorNode);

    while(ACP_TRUE)
    {
        /* aAnchor was deleted during search */
        ACP_TEST_RAISE(aclSafeListIsNodeAlive(aAnchorNode) == ACP_FALSE,
                       ENOSUCHANCHOR);

        /* End of list encountered */
        ACP_TEST_RAISE(sPrev == NULL, EBEYONDLIST);

        if(aclSafeListIsNodeAlive(sPrev) == ACP_TRUE)
        {
            if(ACL_SAFELIST_GETNEXT(sPrev) == aAnchorNode)
            {
                ACL_SAFELIST_SETPREV(aAnchorNode, sPrev);
                /* Gotcha! */
                break;
            }
            else
            {
                /* Find more-forward live node */
                sRC = aclSafeListPrevNext(aList, sPrev, aAnchorNode, &sPrev);
                ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));
                break;
            }
        }
        else
        {
            /* Dead node encountered */
            sPrev = ACL_SAFELIST_GETPREV(sPrev);
        }
    }

    *aNode = sPrev;

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ENULL)
    {
        sRC = ACP_RC_EINVAL;
    }
    ACP_EXCEPTION(EBEYONDLIST)
    {
        /* no more acces to last node? */ 
        /*(void)acpAtomicDec32(&(aAnchorNode->mRefCnt));*/
        sRC = ACP_RC_ENOENT;
    }
    ACP_EXCEPTION(ENOSUCHANCHOR)
    {
        /* this thread no more use, aleady deleted*/
        //(void)acpAtomicDec32(&(aAnchorNode->mRefCnt));
        sRC = ACP_RC_ESRCH;
    }

    ACP_EXCEPTION_END;
    return sRC;
}

/**
 * Insert @a aNewNode into @a aList after the position of @a aAnchorNode.
 * @param aList Thread-safe doubly linked list structure
 * @param aAnchorNode Anchor node to insert @a aNewNode of which
 * @param aNewNode Node pointer storing new node
 * @return #ACP_RC_SUCCESS Insertion successful
 * @return #ACP_RC_EINVAL @a aList, @a aAnchorNode or @a aNode is NULL
 * @return #ACP_RC_ENOENT Tried to insert beyond the list
 * @return #ACP_RC_ESRCH @a aAnchorNode was deleted during insertion
 */
ACP_EXPORT acp_rc_t    aclSafeListInsertAfter(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t*    aNewNode)
{
    acp_rc_t                sRC;
    acl_safelist_node_t*    sNext;
    acp_uint32_t            sLocalGCFlag = ACL_SAFELIST_NO;

    /* The pointer must be the multiple of 2. */
    ACE_DASSERT(((acp_ulong_t)aNewNode) % 2 == 0);

    ACP_TEST_RAISE((aList == NULL || aAnchorNode == NULL || aNewNode == NULL),
                   ENULL);
    ACP_TEST_RAISE(aAnchorNode == &(aList->mTail), EBEYONDLIST);

    /*get local GC Flag */
    sLocalGCFlag = acpAtomicGet32(&(aList->mGCFlag));

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicInc32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicInc32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    /*node reference count initialize*/
    acpAtomicSet32(&(aNewNode->mRefCnt), 0);

    while(ACP_TRUE)
    {
        /* Someone deleted aAnchorNode */
        ACP_TEST_RAISE(aclSafeListIsNodeAlive(aAnchorNode) == ACP_FALSE,
                       ENOSUCHANCHOR);

        sRC = aclSafeListNextInternal(aList, aAnchorNode, &sNext);
        ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));

        /* Link new node with proper position */
        ACL_SAFELIST_SETNEXT(aNewNode, sNext, ACL_SAFELIST_LIVENODE);
        ACL_SAFELIST_SETPREV(aNewNode, aAnchorNode);

        /* Splice into list */
        if(aclSafeListCASNext(aAnchorNode,
                              sNext,    ACL_SAFELIST_LIVENODE,
                              aNewNode, ACL_SAFELIST_LIVENODE) == ACP_TRUE)
        {
            /* We're done! */
            ACL_SAFELIST_SETPREV(sNext, aNewNode);
            break;
        }
        else
        {
            /* 
             * Someone touched the list.
             * Try again.
             */
        }
    }

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicDec32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicDec32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    if(aList->mFreeNodeCnt >= ACL_SAFELIST_GC_INTERVAL_IN_FREECOUNT)
    {
        (void)aclSafeListGarbageCollect(aList);
    }

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ENULL)
    {
        sRC = ACP_RC_EINVAL;
    }
    ACP_EXCEPTION(EBEYONDLIST)
    {
        sRC = ACP_RC_ENOENT;
    }
    ACP_EXCEPTION(ENOSUCHANCHOR)
    {
        sRC = ACP_RC_ESRCH;
    }

    ACP_EXCEPTION_END;
    return sRC;
}

/**
 * Delete @a aNode from @a aList.
 * @param aList Thread-safe doubly linked list structure
 * @param aNode Node pointer to be deleted
 * @param aRetry Retry deletion when failure,
 * except @a aNode was already deleted.
 * If @a aRetry was #ACP_TRUE, #ACP_RC_EINTR will not be returned.
 * @return #ACP_RC_SUCCESS Deletion successful
 * @return #ACP_RC_EINVAL @a aList, @a aAnchorNode or @a aNode is NULL
 * @return #ACP_RC_ESRCH @a aNode was deleted during deletion
 * @return #ACP_RC_EINTR @a aNode was modified during deletion
 */
ACP_EXPORT acp_rc_t    aclSafeListDeleteNode(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aNode,
    acp_bool_t              aRetry)
{
    acp_rc_t                sRC;
    acl_safelist_node_t*    sNext;
    acl_safelist_node_t*    sLiveNext;
    acl_safelist_node_t*    sLivePrev;
    acp_uint32_t            sLocalGCFlag = ACL_SAFELIST_NO;

    ACP_TEST_RAISE((aList == NULL || aNode == NULL),
                   ENULL);

    /*get local GC Flag */
    sLocalGCFlag = acpAtomicGet32(&(aList->mGCFlag));

    if(sLocalGCFlag == ACL_SAFELIST_NO)/*gcflag 2*/
    {
        acpAtomicInc32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicInc32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    while(ACP_TRUE)
    {
        ACP_TEST_RAISE(aclSafeListIsNodeAlive(aNode) == ACP_FALSE,
                       EALREADYDELETED);

        sNext = ACL_SAFELIST_GETNEXT(aNode);
        sRC   = aclSafeListNextInternal(aList, aNode, &sLiveNext);
        ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));
        sRC   = aclSafeListPrevInternal(aList, aNode, &sLivePrev);
        ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));

        if(aclSafeListCASNext(aNode,
                              sNext, ACL_SAFELIST_LIVENODE,
                              sNext, ACL_SAFELIST_DEADNODE) == ACP_TRUE)
        {
            /* Just for cleanup */
            (void)aclSafeListPrevNext(aList, sLivePrev, sLiveNext, &sNext);
            break;
        }
        else
        {
            /* Someone changed node. Retry? */
            ACP_TEST_RAISE(aRetry == ACP_FALSE, EINTERRUPTED);
        }
    }

    aclSafeListUnuseNode(aNode);

    if(sLocalGCFlag == ACL_SAFELIST_NO)
    {
        acpAtomicDec32(&(aList->mInProgressCounter1));
    }
    else
    {
        acpAtomicDec32(&(aList->mInProgressCounter2));
        /* nothing to do */
    }

    if(aList->mFreeNodeCnt >= ACL_SAFELIST_GC_INTERVAL_IN_FREECOUNT)
    {
        (void)aclSafeListGarbageCollect(aList);
    }

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(ENULL)
    {
        sRC = ACP_RC_EINVAL;
    }
    ACP_EXCEPTION(EALREADYDELETED)
    {
        sRC = ACP_RC_ESRCH;
    }
    ACP_EXCEPTION(EINTERRUPTED)
    {
        sRC = ACP_RC_EINTR;
    }

    ACP_EXCEPTION_END;

    if(sRC != ACP_RC_EINVAL)
    {
        aclSafeListUnuseNode(aNode);
        if(sLocalGCFlag == ACL_SAFELIST_NO)
        {
            acpAtomicDec32(&(aList->mInProgressCounter1));
        }
        else
        {
            acpAtomicDec32(&(aList->mInProgressCounter2));
            /* nothing to do */
        }
    }
    return sRC;
}

