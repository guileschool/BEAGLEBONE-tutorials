StarterWare Version 02.00.01.01
===
Date: 12 July, 2013.

TI(Texas Instruments) 에서 제작한 beaglebone 기반 펌웨어및 드라이버

#Introduction
StarterWare 02.00.01.01 provides no-OS platform support for TI AM335x GP EVM, TI AM335x EVM-SK and
Beaglebone which are based on TI AM335x SOC. StarterWare provides Device Abstraction Layer, libraries,
peripheral/board level sample/demo examples that demonstrate the capabilities of the peripherals of AM335x.
StarterWare comes with pre-compiled binaries for bootloader and example applications which can be run from an
SD card.

#Features
| Features | Beagle Bone Examples |
| ---------- | :--------- |
| UART    | ● Echo Application in Interrupt Mode<br>● Echo Application in DMA Mode|
| High Speed-I2C| ● EEPROM Read in Interrupt Mode<br>● EEPROM Read in DMA Mode<br>● Temperature sensor and Accelerometer are not connected on BB (base board)|
|Multi-channel SPI |● No device is connected to  McSPI on BB (base board) |
|DMTimer|● ISR Counting |
|Watchdog Timer |● WDT Demonstration |
|GPIO |● LED On/Off <br>● Audio Buzzer is NOT available in BB (base board) |
|High Speed MMCSD |● SD Card Access with FAT File  System |
|Real-Time Clock (RTC) |● Real-Time Clock (RTC) |
|USB|● CDC Serial Device Mode <br>● Custom Bulk Device Mode <br>● MSC Device Mode <br>● MSC Host <br>● MSC Host + MSC Device <br>● CDC Serial (device) + CDC<br>● Serial (device)<br><br>Other examples are not supported due to limitations on beaglebone(baseboard)<br>1. USB0 port is used for debugger connection and serial port emulation<br>2. LCD cape daughter board is not supported in StarterWare |
|Ethernet|● Embedded Web Page Access<br>● Echo Application with socket-programming|
|McASP |*Audio Jacks are not available in BB(base board)*|
|EDMA|● Memory to memory transfer |
|Raster|*LCD cape daughter board is not supported in StarterWare* |
|GPMC|*No device is connected to GPMC on BB (base board)* |
|ELM|*No device is connected to ELM on BB (base board)* |
|ECAP|*LCD cape daughter board is not supported in StarterWare* |
|DCAN|*The DB9 connectors are not available in BB (base board)* |
|TSC-ADC|*LCD cape daughter board is not supported in StarterWare*|
|EHRPWM |*No device is connected to EHRPWM in BB (base board)*|
|Bootloader|● MMCSD Boot<br>● UART Boot<br>SPI and NAND flash devices are not available in BB (base board)|
|Graphics Library|*LCD cape daughter board is not supported in StarterWare*|
|Out Of Box Demo|● Driven via Ethernet (webbrowser)<br>● Peripherals demonstrated -Ethernet, MMC/SD, Uart,Timer, RTC<br><br>● Supports Power Management<br>  ・ DS0, DS1, STANDBY<br>  ・ Wake Sources - UART and Timer<br><br>● Not Supports DVFS<br>  ・ OPP50<br>  ・ OPP100<br>  ・ OPP120<br>  ・ SR-TURBO|
|RTC Only<br>(Low powermode)|Not supported in BB due to <br>1. External wake is not accessible in BB <br>2. Silicon issue (Refer errata - Advisory 1.0.5 RTC: 32.768-kHZ Clock is Gating Off) |
|Neon/Vfp|Example includes perrformance measurement of <br>1. Vectorized float operations <br>2. Math lib (Sine and Cosine functions with and without intrinsics) - GCC alone |
|SGX GFX |*Not supported in StarterWare*|
|Crypto  |*Not supported in StarterWare*|
|PRU-ICSS|*Not supported in StarterWare*|
|EMAC-IEEE1588|*Not supported in StarterWare*|
|eQEP |*No device is connected in BB*|
|NOR |*No device is connected in BB*|

- Ethernet lwIP (a light weight IP stack) Port for AM335X
- MMC/SD Library
- NAND Library
- USB library
- APIs to configure Cache and MMU.
- Support for IRQ Preemption. StarterWare IRQ handler is now prioritized IRQ handler. However, if preemption is not desired, all interrupts can be assigned the same priority level.
- Support for switching between Privileged and Non-Privileged modes.


#Document License
This work is licensed under the Creative Commons Attribution-Share Alike 3.0 United States License. To view a
copy of this license, visit http:/ / creativecommons. org/ licenses/ by-sa/ 3. 0/ us/ or send a letter to Creative
Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.


