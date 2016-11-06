beaglebone 지원 gdb 설치와 활용
===

# Prerequisite
- gdb 활용법 숙지

# 소스 내려받기
    git clone https://github.com/guileschool/BEAGLEBONE-tutorials.git

# Compiling OpenOCD v07 설치하기[^1]
## 필수 패키지 설치하기( essential packages )
openocd을 컴파일과 설치하기 위해서 다음의 패키지를 먼저 설치한다.

    $ sudo apt-get install libtool autoconf texinfo libusb-dev
  
## libFTDI의 설치
파일 다운로드
[http://www.intra2net.com/en/developer/libftdi/download/libftdi-0.19.tar.gz](http://www.intra2net.com/en/developer/libftdi/download/libftdi-0.19.tar.gz)

컴파일및 설치
```shell
$ cd ~/libftdi-0.19/src
$ sudo cp ftdi.h /usr/include
$ cd /usr/local/include
$ sudo ln -s /usr/include/ftdi.h ftdi.h
$ cd ~/libftdi-0.19
$ ./configure
$ make
$ sudo make install
```

라이브러리
```shell
$ cd /usr/lib
$ sudo ln -s /usr/local/lib/libftdi.a libftdi.a
$ sudo ln -s /usr/local/lib/libftdi.la libftdi.la
$ sudo ln -s /usr/local/lib/libftdi.so.1.19.0 libftdi.so.1.19.0
$ sudo ln -s /usr/local/lib/libftdi.so.1.19.0 libftdi.so
$ sudo ln -s /usr/local/lib/libftdi.so.1.19.0 libftdi.so.1
```

## openocd의 설치
파일 다운로드
[https://sourceforge.net/projects/openocd/files/openocd/0.7.0/openocd-0.7.0.tar.gz/download](https://sourceforge.net/projects/openocd/files/openocd/0.7.0/openocd-0.7.0.tar.gz/download)

컴파일및 설치
```shell
$ cd ~/openocd-0.7.0
$ sudo ./configure --disable-werror --enable-ft2232_libftdi
$ sudo make
$ sudo make install

$ cd ~
$ mkdir openocd-bin
$ cd ~/openocd-0.7.0/tcl
$ cp -r * ~/openocd-bin
$ cd ~/openocd-0.7.0/src
$ cp openocd ~/openocd-bin
```

설치가 완료되었다. 이제 디렉토리 ~/openocd-bin 로 이동하여 **openocd** 을 실행할 수 있다

# 하드웨어 JTAG 디버거
[Flyswatter2](http://www.tincantools.com/JTAG/Flyswatter2.html)

<p align="center">
  <img src="http://d.pr/i/Vler/5b4RAq2D+" width=650 loop=infinite>
</p>
## beaglebone black's openocd 설정파일 준비
파일 다운로드
[https://www.tincantools.com/w/images/f/f7/ti_beaglebone_with_fs2.cfg](https://www.tincantools.com/w/images/f/f7/ti_beaglebone_with_fs2.cfg)

다운로드한 파일을 디렉토리 경로 ~/openocd-bin/board 로 복사한다
 
## openocd 의 실행
```shell
$ cd ~/openocd-bin
$ sudo ./openocd -f interface/flyswatter2.cfg -f board/ti_beaglebone_with_fs2.cfg -c init -c "reset init"
```

# gdb

<p align="center">
  <img src="http://d.pr/i/sTWO/1kwFXPFE+" width=650 loop=infinite>
</p>

## Connecting to OpenOCD
gdb을 실행한다   

     $ arm-none-eabi-gdb

gdb 프롬프트 대기행에서 다음을 입력한다

     $(gdb) target remote localhost:3333

원하는 프로그램을 로딩후 실행

     $(gdb) load firmware-a.elf
     $(gdb) c

[^1]: [https://www.tincantools.com/wiki/Compiling_OpenOCD_v07_Linux](https://www.tincantools.com/wiki/Compiling_OpenOCD_v07_Linux)
