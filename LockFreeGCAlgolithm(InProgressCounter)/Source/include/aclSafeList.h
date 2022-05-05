/*******************************************************************************
 * Copyright 1999-2011, ALTIBASE Corporation or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/

/*******************************************************************************
 * $Id: $
 ******************************************************************************/

#if !defined(_O_ACL_SAFELIST_H_)
#define _O_ACL_SAFELIST_H_

#include <acp.h>

typedef struct acl_safelist_node_t {
    struct acl_safelist_node_t*     mPrev;      /* Previous Node */
    struct acl_safelist_node_t*     mNext;      /* <Next Node, Deleted> */
    acp_uint32_t                    mRefCnt;    /* refernce count*/
    void*                           mData;      /* Data pointer */
    acp_list_t                      mFreeNode;  /* node for free */
} acl_safelist_node_t;

typedef struct acl_safelist_t {
    acl_safelist_node_t         mHead;           /* Head Dummy Node */
    acl_safelist_node_t         mTail;           /* Tail Dummy Node */

    /* only one thr can delete free list item */
    acp_list_t                  mFreeList;       /* Deleted node list */
    acp_uint32_t                mFreeNodeCnt;        /* list item count, do not need, for test*/
    acp_uint16_t                mDoingFreeFlag;  /* can this thr free free list */
    acp_uint16_t                mFreeJobLevel;   /* GC job level */
    acp_list_node_t*            mStartFreePos;   /* position to start free node */
    acp_uint32_t                mGCFlag;         /* flag for GC*/
    acp_uint32_t                mInProgressCounter1; /* counts of thread searching list */
    acp_uint32_t                mInProgressCounter2; /* counts of thread searching list */
} acl_safelist_t;

ACP_EXPORT acp_rc_t    aclSafeListCreate(acl_safelist_t* aList);

ACP_EXPORT acp_rc_t    aclSafeListDestroy(acl_safelist_t* aList);
ACP_EXPORT acp_rc_t    aclSafeListGarbageCollect(acl_safelist_t* aList);


ACP_EXPORT acp_bool_t aclSafeListCASNext(
    acl_safelist_node_t* aThisNode,
    acl_safelist_node_t* aOldRef, acp_ulong_t aOldMark,
    acl_safelist_node_t* aNewRef, acp_ulong_t aNewMark);

ACP_EXPORT void aclSafeListInsertFreeList(acl_safelist_t* aList,
                                          acl_safelist_node_t* aOldNode);

ACP_EXPORT acp_rc_t    aclSafeListNext(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode);
ACP_EXPORT acp_rc_t    aclSafeListNextInternal(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode);

ACP_EXPORT acp_rc_t    aclSafeListPrev(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode);

ACP_EXPORT acp_rc_t    aclSafeListPrevInternal(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t**   aNode);

ACP_INLINE acp_rc_t    aclSafeListFirst(
    acl_safelist_t*         aList,
    acl_safelist_node_t**   aNode)
{
    acp_rc_t                sRC;
    acl_safelist_node_t*    sNode;

    sRC = aclSafeListNext(aList, &(aList->mHead), &sNode);
    ACP_TEST(ACP_RC_NOT_SUCCESS(sRC));
    ACP_TEST_RAISE(sNode == &(aList->mTail), EEMPTY);

    *aNode = sNode;

    return ACP_RC_SUCCESS;

    ACP_EXCEPTION(EEMPTY)
    {
        sRC = ACP_RC_ENOENT;
    }

    ACP_EXCEPTION_END;

    return sRC;
}

ACP_INLINE void aclSafeListUseNode(acl_safelist_node_t*    aNode)
{
    (void)acpAtomicInc32(&(aNode->mRefCnt));
}
ACP_INLINE void aclSafeListUnuseNode(acl_safelist_node_t*    aNode)
{
    (void)acpAtomicDec32(&(aNode->mRefCnt));
}

ACP_EXPORT acp_rc_t    aclSafeListInsertAfter(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aAnchorNode,
    acl_safelist_node_t*    aNewNode);

ACP_INLINE acp_rc_t    aclSafeListPushHead(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aNewNode)
{
    return aclSafeListInsertAfter(aList, &(aList->mHead), aNewNode);
}


ACP_EXPORT acp_bool_t  aclSafeListIsNodeAlive(acl_safelist_node_t* aNode);
ACP_EXPORT acp_rc_t    aclSafeListDeleteNode(
    acl_safelist_t*         aList,
    acl_safelist_node_t*    aNode,
    acp_bool_t              aRetry);

#endif

