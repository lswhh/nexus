# [ opensource altibase compile test ]

### WSL 20.04 설치

설치는 windows 10에서 app store에서 linux 검색해서 설치하면 된다. 

설치시 기본 admin 계정 비밀번호를 입력하고 마친다. 

### WSL 설치 후 패키지 설치

##### sudo apt update와 sudo apt upgrade

lswhh@DESKTOP-HQPQNKV:~$ sudo apt update
Get:1 http://security.ubuntu.com/ubuntu focal-security InRelease [109 kB]
Hit:2 http://archive.ubuntu.com/ubuntu focal InRelease
Get:3 http://archive.ubuntu.com/ubuntu focal-updates InRelease [114 kB]
Get:4 http://security.ubuntu.com/ubuntu focal-security/main amd64 Packages [395 kB]
Get:5 http://security.ubuntu.com/ubuntu focal-security/main Translation-en [90.4 kB]
Get:6 http://security.ubuntu.com/ubuntu focal-security/main amd64 c-n-f Metadata [5584 B]
Get:7 http://security.ubuntu.com/ubuntu focal-security/restricted amd64 Packages [80.7 kB]
Get:8 http://security.ubuntu.com/ubuntu focal-security/restricted Translation-en [12.8 kB]
Get:9 http://security.ubuntu.com/ubuntu focal-security/universe amd64 Packages [521 kB]
Get:10 http://archive.ubuntu.com/ubuntu focal-backports InRelease [101 kB]
Get:11 http://security.ubuntu.com/ubuntu focal-security/universe Translation-en [68.6 kB]
Get:12 http://security.ubuntu.com/ubuntu focal-security/universe amd64 c-n-f Metadata [9364 B]
Get:13 http://security.ubuntu.com/ubuntu focal-security/multiverse amd64 Packages [1256 B]
Get:14 http://security.ubuntu.com/ubuntu focal-security/multiverse Translation-en [540 B]
Get:15 http://security.ubuntu.com/ubuntu focal-security/multiverse amd64 c-n-f Metadata [116 B]
Get:16 http://archive.ubuntu.com/ubuntu focal/universe amd64 Packages [8628 kB]
Get:17 http://archive.ubuntu.com/ubuntu focal/universe Translation-en [5124 kB]
Get:18 http://archive.ubuntu.com/ubuntu focal/universe amd64 c-n-f Metadata [265 kB]
Get:19 http://archive.ubuntu.com/ubuntu focal/multiverse amd64 Packages [144 kB]
Get:20 http://archive.ubuntu.com/ubuntu focal/multiverse Translation-en [104 kB]
Get:21 http://archive.ubuntu.com/ubuntu focal/multiverse amd64 c-n-f Metadata [9136 B]
Get:22 http://archive.ubuntu.com/ubuntu focal-updates/main amd64 Packages [699 kB]
Get:23 http://archive.ubuntu.com/ubuntu focal-updates/main Translation-en [176 kB]
Get:24 http://archive.ubuntu.com/ubuntu focal-updates/main amd64 c-n-f Metadata [11.3 kB]
Get:25 http://archive.ubuntu.com/ubuntu focal-updates/restricted amd64 Packages [103 kB]
Get:26 http://archive.ubuntu.com/ubuntu focal-updates/restricted Translation-en [15.8 kB]
Get:27 http://archive.ubuntu.com/ubuntu focal-updates/restricted amd64 c-n-f Metadata [348 B]
Get:28 http://archive.ubuntu.com/ubuntu focal-updates/universe amd64 Packages [708 kB]
Get:29 http://archive.ubuntu.com/ubuntu focal-updates/universe Translation-en [140 kB]
Get:30 http://archive.ubuntu.com/ubuntu focal-updates/universe amd64 c-n-f Metadata [13.7 kB]
Get:31 http://archive.ubuntu.com/ubuntu focal-updates/multiverse amd64 Packages [20.0 kB]
Get:32 http://archive.ubuntu.com/ubuntu focal-updates/multiverse Translation-en [4668 B]
Get:33 http://archive.ubuntu.com/ubuntu focal-updates/multiverse amd64 c-n-f Metadata [552 B]
Get:34 http://archive.ubuntu.com/ubuntu focal-backports/main amd64 c-n-f Metadata [112 B]
Get:35 http://archive.ubuntu.com/ubuntu focal-backports/restricted amd64 c-n-f Metadata [116 B]
Get:36 http://archive.ubuntu.com/ubuntu focal-backports/universe amd64 Packages [3988 B]
Get:37 http://archive.ubuntu.com/ubuntu focal-backports/universe Translation-en [1448 B]
Get:38 http://archive.ubuntu.com/ubuntu focal-backports/universe amd64 c-n-f Metadata [224 B]
Get:39 http://archive.ubuntu.com/ubuntu focal-backports/multiverse amd64 c-n-f Metadata [116 B]
Fetched 17.7 MB in 7s (2453 kB/s)
Reading package lists... Done
Building dependency tree
Reading state information... Done
142 packages can be upgraded. Run 'apt list --upgradable' to see them.

lswhh@DESKTOP-HQPQNKV:~$ sudo apt upgrade
Reading package lists... Done
Building dependency tree
Reading state information... Done
Calculating upgrade... Done
The following NEW packages will be installed:
  motd-news-config python3-pexpect python3-ptyprocess
The following packages will be upgraded:
  accountsservice alsa-ucm-conf apport apt apt-utils base-files bcache-tools bind9-dnsutils bind9-host bind9-libs bolt
  bsdutils busybox-initramfs busybox-static ca-certificates cloud-init command-not-found cryptsetup cryptsetup-bin
  cryptsetup-initramfs cryptsetup-run curl distro-info-data fdisk finalrd gcc-10-base gir1.2-packagekitglib-1.0
  initramfs-tools initramfs-tools-bin initramfs-tools-core krb5-locales language-selector-common libaccountsservice0
  libapt-pkg6.0 libasound2 libasound2-data libblkid1 libbrotli1 libc-bin libc6 libcryptsetup12 libcurl3-gnutls
  libcurl4 libdns-export1109 libefiboot1 libefivar1 libfdisk1 libfreetype6 libgcc-s1 libgl1 libglvnd0 libglx0
  libgnutls30 libgssapi-krb5-2 libisc-export1105 libk5crypto3 libkrb5-3 libkrb5support0 libldap-2.4-2 libldap-common
  liblzma5 libmaxminddb0 libmount1 libnetplan0 libnss-systemd libpackagekit-glib2-18 libpam-modules libpam-modules-bin
  libpam-runtime libpam-systemd libpam0g libparted2 libperl5.30 libplymouth5 libproxy1v5 libpulse0 libpulsedsp
  libpython3.8 libpython3.8-minimal libpython3.8-stdlib libsmartcols1 libssl1.1 libstdc++6 libsystemd0 libudev1
  libuuid1 libuv1 libx11-6 libx11-data libx11-xcb1 locales mdadm mount netplan.io open-vm-tools openssl packagekit
  packagekit-tools parted perl perl-base perl-modules-5.30 plymouth plymouth-theme-ubuntu-text pulseaudio-utils
  python-apt-common python3-apport python3-apt python3-commandnotfound python3-cryptography python3-distupgrade
  python3-distutils python3-gdbm python3-lib2to3 python3-problem-report python3-software-properties python3-urllib3
  python3.8 python3.8-minimal rsyslog show-motd snapd software-properties-common sosreport sudo systemd systemd-sysv
  systemd-timesyncd tmux tzdata ubuntu-minimal ubuntu-release-upgrader-core ubuntu-server ubuntu-standard ubuntu-wsl
  udev unattended-upgrades update-motd util-linux uuid-runtime xz-utils zlib1g
142 upgraded, 3 newly installed, 0 to remove and 0 not upgraded.
Need to get 78.5 MB of archives.
After this operation, 9127 kB of additional disk space will be used.
Do you want to continue? [Y/n]Y

update-rc.d: warning: start and stop actions are no longer supported; falling back to defaults
Setting up libnss-systemd:amd64 (245.4-4ubuntu3.3) ...
Setting up plymouth-theme-ubuntu-text (0.9.4git20200323-0ubuntu6.2) ...
update-initramfs: deferring update (trigger activated)
Setting up libpam-systemd:amd64 (245.4-4ubuntu3.3) ...
Setting up ubuntu-standard (1.450.2) ...
Processing triggers for man-db (2.9.1-1) ...
Processing triggers for dbus (1.12.16-2ubuntu2.1) ...
Processing triggers for install-info (6.7.0.dfsg.2-5) ...
Processing triggers for mime-support (3.64ubuntu1) ...
Processing triggers for libc-bin (2.31-0ubuntu9.1) ...
Processing triggers for ca-certificates (20201027ubuntu0.20.04.1) ...
Updating certificates in /etc/ssl/certs...
0 added, 0 removed; done.
Running hooks in /etc/ca-certificates/update.d...
done.
Processing triggers for initramfs-tools (0.136ubuntu6.3) ...

##### 로케일 언어 설정

sudo dpkg-reconfigure locales

* en_US.UTF-8 UTF-8
* ko_KR.EUC-KR EUC-KR
* ko_KR.UTF-8 UTF-8

3가지 선택(나머지는 모두 해제)후 기본 언어로 ko_KR.UTF-8 UTF-8 를 선택

```
[옵션]
윈도우와의 파일 공유 등 몇몇 이유로 시스템 기본 로케일을 euc-kr 로 했을 때 가장 큰 문제점은 nautilus 에서 파일을 만들면, 해당 파일명의 인코딩이 항상 UTF-8 로 된다는 것입니다.
이 경우, 노틸러스의 기본 파일 인코딩을 euc-kr 로 바꿔 주면 됩니다.
/etc/environment 에 다음의 라인을 추가합니다 :

G_FILENAME_ENCODING=eucKR

그리고, 시스템을 재시작하면, 노틸러스에서 한글 파일명을 생성해도 파일명의 encoding 이 eucKR 로 되어서 생성됩니다. (wsl은 잘 모르겠음)
```



###### 로케일 언어 설정을 위한 패키지 설치

lswhh@DESKTOP-HQPQNKV:~$ sudo apt-get install language-pack-ko
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following additional packages will be installed:
  language-pack-ko-base
The following NEW packages will be installed:
  language-pack-ko language-pack-ko-base
0 upgraded, 2 newly installed, 0 to remove and 0 not upgraded.
Need to get 933 kB of archives.
After this operation, 4018 kB of additional disk space will be used.
Do you want to continue? [Y/n] Y
Get:1 http://archive.ubuntu.com/ubuntu focal-updates/main amd64 language-pack-ko-base all 1:20.04+20200709 [931 kB]
52% [1 language-pack-ko-base 604 kB/931 kB 65%]

lswhh@DESKTOP-HQPQNKV:~$ sudo locale-gen ko_KR.EUC-KR
Generating locales (this might take a while)...
  ko_KR.EUC-KR... done
Generation complete.
lswhh@DESKTOP-HQPQNKV:~$ sudo /var/lib/locales/supported.d/ko
sudo: /var/lib/locales/supported.d/ko: command not found
lswhh@DESKTOP-HQPQNKV:~$ sudo vi /var/lib/locales/supported.d/ko
sudo vi /var/lib/locales/supported.d/ko 하단에 "ko_KR.EUC-KR EUC-KR" 추가
sudo dpkg-reconfigure locales
lswhh@DESKTOP-HQPQNKV:~$ sudo dpkg-reconfigure locales
Generating locales (this might take a while)...
  en_US.UTF-8... done
  ko_KR.EUC-KR... done
  ko_KR.UTF-8... done

###### 한글 입력기 nabi 설치

lswhh@DESKTOP-HQPQNKV:~$ sudo apt install nabi
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following additional packages will be installed:
  adwaita-icon-theme fontconfig gtk-update-icon-cache hicolor-icon-theme humanity-icon-theme libatk1.0-0
  libatk1.0-data libavahi-client3 libavahi-common-data libavahi-common3 libcairo-gobject2 libcairo2 libcups2
  libdatrie1 libgail-common libgail18 libgdk-pixbuf2.0-0 libgdk-pixbuf2.0-bin libgdk-pixbuf2.0-common libgraphite2-3
  libgtk2.0-0 libgtk2.0-bin libgtk2.0-common libhangul-data libhangul1 libharfbuzz0b libjbig0 libjpeg-turbo8 libjpeg8
  libpango-1.0-0 libpangocairo-1.0-0 libpangoft2-1.0-0 libpixman-1-0 librsvg2-2 librsvg2-common libthai-data libthai0
  libtiff5 libwebp6 libxcb-render0 libxcb-shm0 libxcursor1 ubuntu-mono
Suggested packages:
  cups-common gvfs librsvg2-bin imhangul-gtk2 | imhangul-gtk3
The following NEW packages will be installed:
  adwaita-icon-theme fontconfig gtk-update-icon-cache hicolor-icon-theme humanity-icon-theme libatk1.0-0
  libatk1.0-data libavahi-client3 libavahi-common-data libavahi-common3 libcairo-gobject2 libcairo2 libcups2
  libdatrie1 libgail-common libgail18 libgdk-pixbuf2.0-0 libgdk-pixbuf2.0-bin libgdk-pixbuf2.0-common libgraphite2-3
  libgtk2.0-0 libgtk2.0-bin libgtk2.0-common libhangul-data libhangul1 libharfbuzz0b libjbig0 libjpeg-turbo8 libjpeg8
  libpango-1.0-0 libpangocairo-1.0-0 libpangoft2-1.0-0 libpixman-1-0 librsvg2-2 librsvg2-common libthai-data libthai0
  libtiff5 libwebp6 libxcb-render0 libxcb-shm0 libxcursor1 nabi ubuntu-mono
0 upgraded, 44 newly installed, 0 to remove and 0 not upgraded.
Need to get 14.4 MB of archives.
After this operation, 67.0 MB of additional disk space will be used.
Do you want to continue? [Y/n] Y

###### nabi 설정

cd ~
vi .gnomerc
export XMODIFIERS="@im=nabi"
export GTK_IM_MODULE=xim
export VTE_USE_XFT=0

##### kde language pack 설치

lswhh@DESKTOP-HQPQNKV:~$ sudo apt install language-pack-kde-ko
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following additional packages will be installed:
  k3b-i18n
The following NEW packages will be installed:
  k3b-i18n language-pack-kde-ko
0 upgraded, 2 newly installed, 0 to remove and 0 not upgraded.
Need to get 3731 kB of archives.
After this operation, 16.6 MB of additional disk space will be used.
Do you want to continue? [Y/n]

##### ssh 서버 재설치

lswhh@DESKTOP-HQPQNKV:~$ sudo apt purge openssh-server
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following packages will be REMOVED:
  openssh-server*
0 upgraded, 0 newly installed, 1 to remove and 0 not upgraded.
After this operation, 1527 kB disk space will be freed.
Do you want to continue? [Y/n] Y
(Reading database ... 46734 files and directories currently installed.)
Removing openssh-server (1:8.2p1-4ubuntu0.1) ...
invoke-rc.d: could not determine current runlevel

 * Stopping OpenBSD Secure Shell server sshd                                                                     [ OK ]
Processing triggers for man-db (2.9.1-1) ...
(Reading database ... 46716 files and directories currently installed.)
Purging configuration files for openssh-server (1:8.2p1-4ubuntu0.1) ...
Processing triggers for systemd (245.4-4ubuntu3.3) ...
Processing triggers for ufw (0.36-6) ...

lswhh@DESKTOP-HQPQNKV:~$ sudo apt-get install openssh-server
Reading package lists... Done
Building dependency tree
Reading state information... Done
Suggested packages:
  molly-guard monkeysphere ssh-askpass
The following NEW packages will be installed:
  openssh-server
0 upgraded, 1 newly installed, 0 to remove and 0 not upgraded.
Need to get 377 kB of archives.
After this operation, 1527 kB of additional disk space will be used.
Get:1 http://archive.ubuntu.com/ubuntu focal-updates/main amd64 openssh-server amd64 1:8.2p1-4ubuntu0.1 [377 kB]
Fetched 377 kB in 2s (163 kB/s)
Preconfiguring packages ...
Selecting previously unselected package openssh-server.
(Reading database ... 46711 files and directories currently installed.)
Preparing to unpack .../openssh-server_1%3a8.2p1-4ubuntu0.1_amd64.deb ...
Unpacking openssh-server (1:8.2p1-4ubuntu0.1) ...
Setting up openssh-server (1:8.2p1-4ubuntu0.1) ...

Creating config file /etc/ssh/sshd_config with new version
Creating SSH2 RSA key; this may take some time ...
3072 SHA256:WJyT7eQ1HpNhn7Llxtq2sZwbJOyLWRrbqB+tpGTN2vI root@DESKTOP-HQPQNKV (RSA)
Creating SSH2 ECDSA key; this may take some time ...
256 SHA256:8GWM2WNqQedSEz0T3ztvcYJu8U+Z6fr2GF3355xJipM root@DESKTOP-HQPQNKV (ECDSA)
Creating SSH2 ED25519 key; this may take some time ...
256 SHA256:jHQmnmA2MtqSMPZi2zJeURXOs4vafjSQJes0qzQaedQ root@DESKTOP-HQPQNKV (ED25519)
Created symlink /etc/systemd/system/sshd.service → /lib/systemd/system/ssh.service.
Created symlink /etc/systemd/system/multi-user.target.wants/ssh.service → /lib/systemd/system/ssh.service.
invoke-rc.d: could not determine current runlevel
Processing triggers for systemd (245.4-4ubuntu3.3) ...
Processing triggers for man-db (2.9.1-1) ...

###### ssh 서버 설정 및 재시작

lswhh@DESKTOP-HQPQNKV:~$ sudo vi /etc/ssh/sshd_config
/etc/ssh/sshd_config <-- 아래 라인 주석 해제 확인
Port 22
PermitRootLogin prohibit-password  
AuthorizedKeysFile      .ssh/authorized_keys .ssh/authorized_keys2
PasswordAuthentication yes
PubkeyAuthentication yes
ChallengeResponseAuthentication no
X11Forwarding yes
UseDNS no
lswhh@DESKTOP-HQPQNKV:~$ sudo service ssh --full-restart
vice ssh restart * Stopping OpenBSD Secure Shell server sshd                                                     [ OK ]

Starting OpenBSD Secure Shell server sshd                                                                     [ OK ]
lswhh@DESKTOP-HQPQNKV:~$ sudo service ssh restart

Restarting OpenBSD Secure Shell server sshd                                                                   [ OK ]

##### 한영키 설정

 xev 프로그램 실행 후 한영키와 한자키 코드 확인 
 lswhh@DESKTOP-HQPQNKV:~$ xev
 한영 64
 한자 37
xmodmap -e "keycode 64 = Hangul"
xmodmap -e "keycode 37 = Hangul_Hanja"

lswhh@DESKTOP-HQPQNKV:~$ vi ~/.Xmodmap 에 아래 내용 저장
keycode 64 = Hangul
keycode 37 = Hangul_Hanja

##### 은폰트 설치

lswhh@DESKTOP-HQPQNKV:~$ sudo apt install fonts-unfonts-core
패키지 목록을 읽는 중입니다... 완료
의존성 트리를 만드는 중입니다
상태 정보를 읽는 중입니다... 완료
다음 새 패키지를 설치할 것입니다:
  fonts-unfonts-core
0개 업그레이드, 1개 새로 설치, 0개 제거 및 0개 업그레이드 안 함.
14.9 M바이트 아카이브를 받아야 합니다.
이 작업 후 34.3 M바이트의 디스크 공간을 더 사용하게 됩니다.
받기:1 http://archive.ubuntu.com/ubuntu focal/universe amd64 fonts-unfonts-core all 1:1.0.2-080608-16 [14.9 MB]
lswhh@DESKTOP-HQPQNKV:~$ sudo apt install fonts-unfonts-extra
패키지 목록을 읽는 중입니다... 완료
의존성 트리를 만드는 중입니다
상태 정보를 읽는 중입니다... 완료
다음 새 패키지를 설치할 것입니다:

##### [gcc-7, g++-7 설치]

lswhh@DESKTOP-HQPQNKV:~$ sudo apt install gcc-7
...
lswhh@DESKTOP-HQPQNKV:~$ sudo apt install g++-7
...
내려받기 11.2 M바이트, 소요시간 5초 (2,153 k바이트/초)
Selecting previously unselected package libstdc++-7-dev:amd64.
(데이터베이스 읽는중 ...현재 50996개의 파일과 디렉터리가 설치되어 있습니다.)
Preparing to unpack .../libstdc++-7-dev_7.5.0-6ubuntu2_amd64.deb ...
Unpacking libstdc++-7-dev:amd64 (7.5.0-6ubuntu2) ...
Selecting previously unselected package g++-7.
Preparing to unpack .../g++-7_7.5.0-6ubuntu2_amd64.deb ...
Unpacking g++-7 (7.5.0-6ubuntu2) ...
libstdc++-7-dev:amd64 (7.5.0-6ubuntu2) 설정하는 중입니다 ...
g++-7 (7.5.0-6ubuntu2) 설정하는 중입니다 ...
Processing triggers for man-db (2.9.1-1) ...
[build essential 설치]
lswhh@DESKTOP-HQPQNKV:~$ sudo apt install build-essential
여기까지 참고
http://nok.altibase.com/x/T6VS

### 개발환경

