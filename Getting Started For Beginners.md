Getting Started For Beginners
===

Sorry for my poor English and I hope you can understand me.

# Prerequisite
- github.com 와 git 도구의 활용법 숙지

# 소스 내려받기
    git clone https://github.com/guileschool/BEAGLEBONE-tutorials.git

# 우분투 설치하기
## 우분투 installer 이미지 다운로드후 설치
[http://processors.wiki.ti.com/index.php/How_to_Build_a_Ubuntu_Linux_host_under_VMware#Using_the_Target_as_a_USB_Mass_Storage_Device](http://processors.wiki.ti.com/index.php/How_to_Build_a_Ubuntu_Linux_host_under_VMware#Using_the_Target_as_a_USB_Mass_Storage_Device)

## 필수 패키지 설치하기( essential packages )
    $ sudo apt-get install build-essential subversion ccache sed wget cvs git-core coreutils unzip texinfo docbook-utils gawk help2man diffstat file g++ texi2html bison flex htmldoc chrpath libxext-dev xserver-xorg-dev doxygen corkscrew minicom vim xinetd tftp tftpd tree
    $ sudo apt-get install kpartx
    $ sudo apt-get install libsdl1.2-dev

## minicom 설치후 사용시 권한 오류및 대처
    $ sudo usermod -a -G dialout user   <----- 사용자 아이디가 'user' 라고 가정

# 네트워크 설정(Network Configuration)
## 네트워크 개념도
디폴트 네트워크외에, USB 이더넷 어댑터를 이용하여 비글본 사용을 위한 또 하나의 네트워크를 추가 구성한다.
<p align="center">
  <img src="http://www.guileschool.com/image2/7432EDE3-F45A-43CE-88C7-A5A3A425A7EF.jpg" width=650 loop=infinite>
</p>

## Configuring a TFTP Server Run by xinetd

     $ sudo mkdir /tftpboot
     $ sudo chmod 777 /tftpboot

/etc/xinetd.d 디렉토리내의 파일 tftp 을 다음과 같이 수정

     $ sudo vim /etc/xinetd.d/tftp

```
#--------------------
service tftp
{
    socket_type     = dgram
    protocol        = udp
    wait            = yes
    user            = root
    server          = /usr/sbin/in.tftpd
    server_args     = -s /tftpboot
    disable         = yes
}
#--------------------
```

## Configuring a MPTCP

/etc/network/if-up.d/mptcp_up 파일을 수정

     $ sudo vim /etc/network/if-up.d/mptcp_up

```
#--------------------
#!/bin/sh
GATEWAY=`ifconfig eth0 | grep "inet " | awk -F'[: ]+' '{ print $4 }' | sed -e 's;[0-9]*$;;'`
while ip route del default; do :; done
ip route add default via ${GATEWAY}1 metric 1000
ip route list
#--------------------
```
☞ eth0, eth1, eth2 ... 복수의 네트워크 장치를 사용할 때 만 필요함.

# 컴파일러 설치하기( toolchain 설치 방법 )

~~~
$ wget https://sourcery.mentor.com/sgpp/lite/arm/portal/package8734/public/arm-none-eabi/arm-2011.03-42-arm-none-eabi-i686-pc-linux-gnu.tar.bz2
$ sudo tar jxf arm-2011.03-42-arm-none-eabi-i686-pc-linux-gnu.tar.bz2 -C /usr/local

'$HOME/.bashrc' 에 아래 내용을 추가한다.

export PATH=/usr/local/arm-2011.03/bin:$PATH
~~~

로그아웃 후 다시 로그인 하자  

 
# 비글보드 활용 준비물

비글보드를 사용하기 위해 필요한 재료들을 다음과 같이 정리한다.
<table>
<tr>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/DD22B704-8756-4C4E-B950-F5D137EEA05F.jpg" width=450 loop=infinite>
</p>
</td>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/E1BF590A-0D99-4D55-A3AA-833BB57023E4.jpg" width=450 loop=infinite>
</p>
</td>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/15DC5D01-972C-47A2-9748-C79B56152089.jpg" width=450 loop=infinite>
</p>
</td>
</tr>
<tr>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/ACDC8DF1-C759-415E-93F5-358AFBF92FA6.jpg" width=450 loop=infinite>
</p>
</td>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/2173EBB6-94ED-4BBD-AFE2-603BF24C6C8D.jpg" width=450 loop=infinite>
</p>
</td>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/F28821D9-CB30-49B6-AD97-703F83DFFFB8.jpg" width=450 loop=infinite>
</p>
</td>
</tr>
<tr>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/3C48EEA9-E04E-43D7-97FC-3F2693C5B07A.jpg" width=450 loop=infinite>
</p>
</td>
<td>
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/11A05602-C078-44B3-8E86-E92EEF6F9DEF.jpg" width=450 loop=infinite>
</p>
</td>
<td>
</td>
</tr>
</table>
>좌측 맨 위에부터 비글보드, 만능기판, USB이더넷, SD카드, 점퍼케이블,
FTDI시리얼케이블, 이더넷케이블, 온오프파워코드


BOOT SDCARD 만들기
==============
<p align="center">
  <p style="text-align: left"></p>
  <img src="http://www.guileschool.com/image2/08276603-2FE3-46F5-987B-D7E02FE04495.jpg" width=450 loop=infinite>
</p>


###4 ~ 8GB 용량의 SD카드를 준비한다
<p align="center">
    <img src="http://www.guileschool.com/image2/D1948DDD-CDFD-4976-B2E2-6981A43CCC51.jpg" width=1050 loop=infinite>
</p>

다양한 실험(활용)을 위하여 **펌웨어**, **리눅스(ARAGO)**, **리눅스(앵스트롬)**, **욕토프로젝트**, **안드로이드** 종류별로 총 **5개**의 메모리 카드를 준비하였다.

###SDCARD을 컴퓨터 슬롯에 넣는다
<p align="center">
  <img src="http://www.guileschool.com/image2/5656A8C2-90C7-446B-AEC7-F415785F944B.jpg" width=650 loop=infinite>
</p>

###SD카드 포맷및 이미지 파일 복사
☞ 본문예에서 사용된 SD카드는 4G capacity임

    $ cd ~/BEAGLEBONE-tutorials/BBB-firmware/mkcard
    $ sudo ./mkcard.sh

```
################################################################################

This script will create a bootable SD card from custom or pre-built binaries.

The script must be run with root permissions and from the bin directory of
the SDK

Example:
 $ sudo ./mkcard.sh

Formatting can be skipped if the SD card is already formatted and
partitioned properly.

################################################################################

ROOTDRIVE=sda

Availible Drives to write images to: 

#  major   minor    size   name 
1:   8       16   52428800 sdb
2:   8       32  262144000 sdc
3:   8      112    3872256 sdh <------------------- SDCARD
 
Enter Device Number or n to exit: 3
```
  
###SD카드로 부팅하기
SD카드를 비글보드 SD슬롯에 넣고 boot 스위치를 누른채로 
전원을 넣고 부팅한다.  

<p align="center">
  <img src="http://www.guileschool.com/image2/BB6F3D79-4DD0-4478-80C5-00C4E23BC4BD.jpg" width=650 loop=infinite>
</p>

###전원을 끄는 방법
파워버튼을 8초 이상 누른채로 있는다.

<p align="center">
    <img src="http://www.guileschool.com/image2/FE58A356-A464-4248-82F7-155B995FEB85.jpg" width=650 loop=infinite>
</p>

처음사용자를 위한 eMMC 초기화 방법(Angstrom)
===

§ [Beagleboard:UpdatingTheSoftware-eLinux.org](http://elinux.org/Beagleboard:Updating_The_Software)

1. 이미지 파일을 다운로드한다  
[https://s3.amazonaws.com/angstrom/demo/beaglebone/BBB-eMMC-flasher-2013.09.04.img.xz](https://s3.amazonaws.com/angstrom/demo/beaglebone/BBB-eMMC-flasher-2013.09.04.img.xz)

2.  7z 프로그램으로 압축된 이미지 파일을 unzip 한다  
$ 7z x BBB-eMMC-flasher-2013.09.04.img.xz
BBB-eMMC-flasher-2013.09.04.img

3.  [DiskImager](https://wiki.ubuntu.com/Win32DiskImager) software 을 다운로드 받은 후 설치한다  

4.  DiskImager 프로그램을 이용하여 4G SD카드에 BBB-eMMC-flasher-2013.09.04.img 이미지를 기록한다  

5. SD카드를 전원이 꺼져있는 비글본 보드 SD 슬롯에 꼽는다  
6. S2 (Boot Switch, see picture below) 을 누른채로 전원을 넣는다  
7. User LED 가 켜질 때까지 S2 버튼을 누른채로 손을 떼지말고 기다린다  
8. User LED 가 켜졌다면 S2 버튼에서 손을 떼고 User LED 4개 모두 켜질 때까지 약 20여 분 간 기다린다.  
☞ eMMC 기록 중에는 User LED가 빠르게 점멸(꺼지고 켜지는 동작이 반복)할 것이다
9. 기록이 모두 끝난 후 SD카드는 제거한다  
10. 비글본 보드의 전원을 켠다  

비글보드의 연결 방법
===
##방법1.USB 케이블의 활용
- 스토리지 디바이스로 연결 가능
- RNDIS 이더넷 연결 가능
- 테더링이 가능( PC에서는 크롬이나 파이어폭스를 사용해야 함 )

<p align="center">
    <img src="http://www.guileschool.com/image2/40578CCC-CB34-41B2-8885-3F5072371D80.jpg" width=650 loop=infinite>
</p>

##방법2.이더넷 케이블의 활용

<p align="center">
    <img src="http://www.guileschool.com/image2/94A928CC-5EC4-43E7-859C-57E62BBFA5D5.jpg" width=650 loop=infinite>
</p>


하드웨어 분석
===
##메모리
| 종류 | 부품명 | 설명 
| :-----: | :-----: | :-----
| 512MB DDR3L | D2516EC4BXGGB from Kingston | 256Mb x16 DDR3L 4Gb (512MB) memory
| 4KB EEPROM | 24LC32A | I2C0( board name, serial number, and revision information )
| 4GB Embedded MMC | 32E77 from Micron | MMC1 port of the processor, allowing for 8bit wide access
| iROM | Cell | 롬 48KB(0x20000 ~ 0x2BFFF)
| iRAM | Cell | 램 127KB(0x402F0400 ~ 0x4030FFFF)

확장커넥터 핀정의
===
<p align="center">
    <img src="http://www.guileschool.com/image2/5A3F3F3F-3C61-4D15-BA56-718D0177A0A1.jpg" width=1000 loop=infinite>
</p>

<p align="center">
    <img src="http://www.guileschool.com/image2/B89A3EF0-BCFB-4CEC-B945-067B289F313F.jpg" width=1700 loop=infinite>
</p>

##부팅과정
ROM code (we can't edit this) -> X-loader (in SRAM) -> u-boot (in DDR
RAM) -> Linux system.

(for details see here: [http://omappedia.org/wiki/Bootloader_Project](http://omappedia.org/wiki/Bootloader_Project.jpg)  or  
[http://lists.denx.de/pipermail/u-boot/2013-December/168481.html](http://lists.denx.de/pipermail/u-boot/2013-December/168481.html)
)

<p align="center">
    <img src="http://www.guileschool.com/image2/081ED1F3-1383-41AD-94BF-F3AC00B40C71.jpg" width=1200 loop=infinite>
</p>

[http://diydrones.com/profiles/blogs/booting-up-a-beaglebone-black](http://diydrones.com/profiles/blogs/booting-up-a-beaglebone-black)
