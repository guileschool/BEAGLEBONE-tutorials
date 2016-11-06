9U-BOOT 
===

##U-BOOT란?
U-Boot is a stage-2 bootloader. ( [A Handy U-Boot Trick - Linux Journal](http://www.linuxjournal.com/content/handy-u-boot-trick) )

##U-BOOT 부팅 원리
<p align="center">
    <img src="http://d.pr/i/NwQa/4jXlFlqo+" width=650 loop=infinite>
</p>

##비글본블랙 부팅 화면 캡쳐
U-Boot v2013.04-dirty 기준

```logs
U-Boot SPL 2013.04-dirty (Jul 10 2013 - 14:02:53)
musb-hdrc: ConfigData=0xde (UTMI-8, dyn FIFOs, HB-ISO Rx, HB-ISO Tx, SoftConn)
musb-hdrc: MHDRC RTL version 2.0 
musb-hdrc: setup fifo_mode 4
musb-hdrc: 28/31 max ep, 16384/16384 memory
USB Peripheral mode controller at 47401000 using PIO, IRQ 0
musb-hdrc: ConfigData=0xde (UTMI-8, dyn FIFOs, HB-ISO Rx, HB-ISO Tx, SoftConn)
musb-hdrc: MHDRC RTL version 2.0 
musb-hdrc: setup fifo_mode 4
musb-hdrc: 28/31 max ep, 16384/16384 memory
USB Host mode controller at 47401800 using PIO, IRQ 0
OMAP SD/MMC: 0
mmc_send_cmd : timeout: No status update
reading u-boot.img
reading u-boot.img


U-Boot 2013.04-dirty (Jul 10 2013 - 14:02:53)

I2C:   ready
DRAM:  512 MiB
WARNING: Caches not enabled
NAND:  No NAND device found!!!
0 MiB
MMC:   OMAP SD/MMC: 0, OMAP SD/MMC: 1
*** Warning - readenv() failed, using default environment

musb-hdrc: ConfigData=0xde (UTMI-8, dyn FIFOs, HB-ISO Rx, HB-ISO Tx, SoftConn)
musb-hdrc: MHDRC RTL version 2.0 
musb-hdrc: setup fifo_mode 4
musb-hdrc: 28/31 max ep, 16384/16384 memory
USB Peripheral mode controller at 47401000 using PIO, IRQ 0
musb-hdrc: ConfigData=0xde (UTMI-8, dyn FIFOs, HB-ISO Rx, HB-ISO Tx, SoftConn)
musb-hdrc: MHDRC RTL version 2.0 
musb-hdrc: setup fifo_mode 4
musb-hdrc: 28/31 max ep, 16384/16384 memory
USB Host mode controller at 47401800 using PIO, IRQ 0
Net:   <ethaddr> not set. Validating first E-fuse MAC
cpsw, usb_ether
Hit any key to stop autoboot:  0 
gpio: pin 53 (gpio 53) value is 1

Card did not respond to voltage select!
mmc0(part 0) is current device
mmc_send_cmd : timeout: No status update
Card did not respond to voltage select!
No micro SD card found, setting mmcdev to 1
mmc_send_cmd : timeout: No status update
mmc1(part 0) is current device
gpio: pin 54 (gpio 54) value is 1
SD/MMC found on device 1
reading uEnv.txt
26 bytes read in 2 ms (12.7 KiB/s)
Loaded environment from uEnv.txt
Importing environment from mmc ...
gpio: pin 55 (gpio 55) value is 1
4385024 bytes read in 750 ms (5.6 MiB/s)
gpio: pin 56 (gpio 56) value is 1
24808 bytes read in 34 ms (711.9 KiB/s)
Booting from mmc ...
## Booting kernel from Legacy Image at 80007fc0 ...
   Image Name:   Angstrom/3.8.13/beaglebone
   Image Type:   ARM Linux Kernel Image (uncompressed)
   Data Size:    4384960 Bytes = 4.2 MiB
   Load Address: 80008000
   Entry Point:  80008000
   Verifying Checksum ... OK
## Flattened Device Tree blob at 80f80000
   Booting using the fdt blob at 0x80f80000
   XIP Kernel Image ... OK
OK
   Using Device Tree in place at 80f80000, end 80f890e7

Starting kernel ...

Uncompressing Linux... done, booting the kernel.
[    0.195745] omap2_mbox_probe: platform not supported
[    0.206348] tps65217-bl tps65217-bl: no platform data provided
[    0.282940] bone-capemgr bone_capemgr.8: slot #0: No cape found
[    0.320047] bone-capemgr bone_capemgr.8: slot #1: No cape found
[    0.357155] bone-capemgr bone_capemgr.8: slot #2: No cape found
[    0.394265] bone-capemgr bone_capemgr.8: slot #3: No cape found
[    0.414037] bone-capemgr bone_capemgr.8: slot #6: BB-BONELT-HDMIN conflict P8
.45 (#5:BB-BONELT-HDMI)
[    0.423646] bone-capemgr bone_capemgr.8: slot #6: Failed verification
[    0.444129] omap_hsmmc mmc.4: of_parse_phandle_with_args of 'reset' failed
[    0.451422] bone-capemgr bone_capemgr.8: loader: failed to load slot-6 BB-BON
ELT-HDMIN:00A0 (prio 2)
[    0.517814] pinctrl-single 44e10800.pinmux: pin 44e10854 already requested by
 44e10800.pinmux; cannot claim for gpio-leds.7
[    0.529545] pinctrl-single 44e10800.pinmux: pin-21 (gpio-leds.7) status -22
[    0.536872] pinctrl-single 44e10800.pinmux: could not request pin 21 on devic
e pinctrl-single
[    5.782631] libphy: PHY 4a101000.mdio:01 not found
[    5.787679] net eth0: phy 4a101000.mdio:01 not found on slave 1
systemd-fsck[85]: Angstrom: clean, 49255/231536 files, 292132/925745 blocks

.---O---.                                           
|       |                  .-.           o o        
|   |   |-----.-----.-----.| |   .----..-----.-----.
|       |     | __  |  ---'| '--.|  .-'|     |     |
|   |   |  |  |     |---  ||  --'|  |  |  '  | | | |
'---'---'--'--'--.  |-----''----''--'  '-----'-'-'-'
                -'  |
                '---'

The Angstrom Distribution beaglebone ttyO0

Angstrom v2012.12 - Kernel 3.8.13

beaglebone login: 
mmc_send_cmd : timeout: No status update

```

##비글본블랙 환경변수 세팅값(공장 세팅값)
```logs
                                                        
U-Boot# printenv                                                                            
arch=arm                                                                                    
baudrate=115200                                                                             
board=am335x                                                                                
board_name=A335BNLT                                                                         
board_rev=00C0                                                                              
boot_fdt=try                                                                                
bootcmd=gpio set 53; i2c mw 0x24 1 0x3e; run findfdt; run mmcboot;gpio clear 56; gpio clear 
55; gpio clear 54; setenv mmcdev 1; setenv bootpart 1:1; run mmcboot;run nandboot;          
bootcount=12                                                                                
bootdelay=1                                                                                 
bootenv=uEnv.txt                                                                            
bootfile=zImage                                                                             
bootpart=0:1                                                                                
console=ttyO0,115200n8                                                                      
cpu=armv7                                                                                   
dfu_alt_info_emmc=rawemmc mmc 0 3751936                                                     
dfu_alt_info_mmc=boot part 0 1;rootfs part 0 2;MLO fat 0 1;MLO.raw mmc 100 100;u-boot.img.ra
w mmc 300 400;spl-os-args.raw mmc 80 80;spl-os-image.raw mmc 900 2000;spl-os-args fat 0 1;sp
l-os-image fat 0 1;u-boot.img fat 0 1;uEnv.txt fat 0 1                                      
dfu_alt_info_nand=SPL part 0 1;SPL.backup1 part 0 2;SPL.backup2 part 0 3;SPL.backup3 part 0 
4;u-boot part 0 5;u-boot-spl-os part 0 6;kernel part 0 8;rootfs part 0 9                    
dfu_alt_info_ram=kernel ram 0x80200000 0xD80000;fdt ram 0x80F80000 0x80000;ramdisk ram 0x810
00000 0x4000000                                                                             
eth1addr=6c:ec:eb:af:03:be                                                                  
ethact=cpsw                                                                                 
ethaddr=6c:ec:eb:af:03:bc                                                                   
fdt_high=0xffffffff                                                                         
fdtaddr=0x88000000                                                                          
fdtdir=/dtbs                                                                                
fdtfile=undefined                                                                           
findfdt=if test $board_name = A335BONE; then setenv fdtfile am335x-bone.dtb; setenv fdtbase 
am335x-bone; fi; if test $board_name = A335BNLT; then setenv fdtfile am335x-boneblack.dtb; s
etenv fdtbase am335x-boneblack; fi; if test $board_name = A33515BB; then setenv fdtfile am33
5x-evm.dtb; fi; if test $board_name = A335X_SK; then setenv fdtfile am335x-evmsk.dtb; fi; if
 test $fdtfile = undefined; then echo WARNING: Could not determine device tree to use; fi;  
importbootenv=echo Importing environment from mmc ...; env import -t $loadaddr $filesize    
loadaddr=0x82000000                                                                         
loadbootenv=load mmc ${bootpart} ${loadaddr} ${bootenv}                                     
loadfdt=load mmc ${bootpart} ${fdtaddr} ${fdtdir}/${fdtfile}                                
loadimage=load mmc ${bootpart} ${loadaddr} ${bootdir}/${bootfile}                           
loadramdisk=load mmc ${mmcdev} ${rdaddr} ramdisk.gz                                         
mmcargs=setenv bootargs console=${console} ${optargs} root=${mmcroot} rootfstype=${mmcrootfs
type}                                                                                       
mmcboot=mmc dev ${mmcdev}; if mmc rescan; then gpio set 54;echo SD/MMC found on device ${mmc
dev};if run loadbootenv; then gpio set 55;echo Loaded environment from ${bootenv};run import
bootenv;fi;if test -n $cape; then if test -e mmc ${bootpart} ${fdtdir}/$fdtbase-$cape.dtb; t
hen setenv fdtfile $fdtbase-$cape.dtb; fi; echo using: $fdtfile...; fi; echo Checking if uen
vcmd is set ...;if test -n $uenvcmd; then gpio set 56; echo Running uenvcmd ...;run uenvcmd;
fi;echo; echo uenvcmd was not defined in uEnv.txt ...;fi;                                   
mmcdev=0                                                                                    
mmcloados=run mmcargs; if test ${boot_fdt} = yes || test ${boot_fdt} = try; then if run load
fdt; then bootz ${loadaddr} - ${fdtaddr}; else if test ${boot_fdt} = try; then bootz; else e
cho WARN: Cannot load the DT; fi; fi; else bootz; fi;                                       
mmcpart=1                                                                                   
mmcroot=/dev/mmcblk0p2 ro fixrtc                                                            
mmcrootfstype=ext4 rootwait                                                                 
mtdids=nand0=omap2-nand.0                                                                   
mtdparts=mtdparts=omap2-nand.0:128k(SPL),128k(SPL.backup1),128k(SPL.backup2),128k(SPL.backup
3),1792k(u-boot),128k(u-boot-spl-os),128k(u-boot-env),5m(kernel),-(rootfs)                  
nandargs=setenv bootargs console=${console} ${optargs} root=${nandroot} rootfstype=${nandroo
tfstype}                                                                                    
nandboot=echo Booting from nand ...; run nandargs; nand read ${fdtaddr} u-boot-spl-os; nand 
read ${loadaddr} kernel; bootz ${loadaddr} - ${fdtaddr}                                     
nandroot=ubi0:rootfs rw ubi.mtd=7,2048                                                      
nandrootfstype=ubifs rootwait=1                                                             
netargs=setenv bootargs console=${console} ${optargs} root=/dev/nfs nfsroot=${serverip}:${ro
otpath},${nfsopts} rw ip=dhcp                                                               
netboot=echo Booting from network ...; setenv autoload no; dhcp; tftp ${loadaddr} ${bootfile
}; tftp ${fdtaddr} ${fdtfile}; run netargs; bootz ${loadaddr} - ${fdtaddr}                  
nfsopts=nolock                                                                              
partitions=uuid_disk=${uuid_gpt_disk};name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs} 
ramargs=setenv bootargs console=${console} ${optargs} root=${ramroot} rootfstype=${ramrootfs
type}                                                                                       
ramboot=echo Booting from ramdisk ...; run ramargs; bootz ${loadaddr} ${rdaddr} ${fdtaddr}  
ramroot=/dev/ram0 rw ramdisk_size=65536 initrd=${rdaddr},64M                                
ramrootfstype=ext2                                                                          
rdaddr=0x88080000                                                                           
rootpath=/export/rootfs                                                                     
soc=am33xx                                                                                  
spiargs=setenv bootargs console=${console} ${optargs} root=${spiroot} rootfstype=${spirootfs
type}                                                                                       
spiboot=echo Booting from spi ...; run spiargs; sf probe ${spibusno}:0; sf read ${loadaddr} 
${spisrcaddr} ${spiimgsize}; bootz ${loadaddr}                                              
spibusno=0                                                                                  
spiimgsize=0x362000                                                                         
spiroot=/dev/mtdblock4 rw                                                                   
spirootfstype=jffs2                                                                         
spisrcaddr=0xe0000                                                                          
static_ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off                    
stderr=serial                                                                               
stdin=serial                                                                                
stdout=serial                                                                               
usbnet_devaddr=6c:ec:eb:af:03:be                                                            
vendor=ti                                                                                   
ver=U-Boot 2014.04-00014-g47880f5 (Apr 22 2014 - 13:23:54)                                  
                                                                                            
Environment size: 4711/131068 bytes                                                         
U-Boot#
```

##비글본블랙 수정된 환경변수 값 세팅
```logs
U-Boot# set serverip 192.168.0.2                                                            
U-Boot# set ipaddr 192.168.0.3                                                              
U-Boot# setenv bootcmd 'tftp ${fdtaddr} rtosdemo.bin; bootz 0x88000000'                     
U-Boot# saveenv                                                                             
Saving Environment to NAND...                                                               
Erasing NAND...                  
```

##U-BOOT 소스 컴파일하기
U-Boot v2015.10-rc5 기준

Download  

```shell
$ git clone git://git.denx.de/u-boot.git
$ cd u-boot/
$ git checkout v2015.10-rc5 -b tmp
```

Configure and Build  

```shell
$ cd u-boot
$ make ARCH=arm CROSS_COMPILE=arm-none-eabi- distclean
$ make ARCH=arm CROSS_COMPILE=arm-none-eabi- am335x_boneblack_defconfig
$ make ARCH=arm CROSS_COMPILE=arm-none-eabi-
```

#uEnv.txt 
```logs
##Video: Uncomment to override:
##see: https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/fb/modedb.txt
#kms_force_mode=video=HDMI-A-1:1024x768@60e

##Enable systemd
systemd=quiet init=/lib/systemd/systemd

##BeagleBone Cape Overrides

##BeagleBone Black:
##Disable HDMI/eMMC
#optargs=capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN,BB-BONE-EMMC-2G

##Disable HDMI
#optargs=capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN

##WIP: v3.13+ capes..
#cape=lcd4-01
#cape=

##note: the eMMC flasher script relies on the next line
mmcroot=UUID=ba488d7b-2c19-4578-a962-fa72c48e93d6 ro
mmcrootfstype=ext4 rootwait fixrtc

##These are needed to be compliant with Angstrom's 2013.06.20 u-boot.
console=ttyO0,115200n8

kernel_file=zImage
initrd_file=initrd.img

loadaddr=0x82000000
initrd_addr=0x88080000
fdtaddr=0x88000000

initrd_high=0xffffffff
fdt_high=0xffffffff

loadkernel=load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${kernel_file}
loadinitrd=load mmc ${mmcdev}:${mmcpart} ${initrd_addr} ${initrd_file}; setenv initrd_size ${filesize}
loadfdt=load mmc ${mmcdev}:${mmcpart} ${fdtaddr} /dtbs/${fdtfile}

loadfiles=run loadkernel; run loadinitrd; run loadfdt
mmcargs=setenv bootargs console=tty0 console=${console} ${optargs} ${kms_force_mode} root=${mmcroot} rootfstype=${mmcrootfstype} ${systemd}

uenvcmd=run loadfiles; run mmcargs; bootz ${loadaddr} ${initrd_addr}:${initrd_size} ${fdtaddr}
#
```

