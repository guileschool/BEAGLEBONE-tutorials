/* Standard includes. */
#include <stdlib.h>
#include <string.h>

#include "am335.h"
#include "serial.h"

static void prvSetupHardware( void );

static void vBlink(void)
{
	unsigned int msDelay;

	serial_puts(UART0_BASE,"blinktask\n");

	while(1){
		serial_puts(UART0_BASE,"LED FLASH\n");
		(*(REG32(GPIO1_BASE + GPIO_CLEARDATAOUT))) = PIN19;
		(*(REG32(GPIO1_BASE + GPIO_CLEARDATAOUT))) = PIN17;
		(*(REG32(GPIO1_BASE + GPIO_CLEARDATAOUT))) = (PIN21|PIN22|PIN23|PIN24);

				msDelay=0x1FFFFFF;
				while(msDelay--){;}

		(*(REG32(GPIO1_BASE + GPIO_SETDATAOUT))) = PIN19;
		(*(REG32(GPIO1_BASE + GPIO_SETDATAOUT))) = PIN17;
		(*(REG32(GPIO1_BASE + GPIO_SETDATAOUT))) = (PIN21|PIN22|PIN23|PIN24);
				msDelay=0x1FFFFFF;
				while(msDelay--){;}
	}
}

/*
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{   

	/* Initialise the LED outputs */
	prvSetupHardware();

	//INIT SERIAL
	init_serial(UART0_BASE);
	serial_puts(UART0_BASE,"Starting BeagleBone\n");

	// LED TEST
	vBlink();

	return 0;
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{

	/* Initialize GPIOs */

  /* BONE */
  /* Enabling the GPIO0 and GPIO1 clocks */
  (*(REG32(PRCM_REG + CM_PER_GPIO1_CLKCTRL))) =0x2;
  (*(REG32(CM_WKUP + 0x8))) =0x2;
  /* Enabling the UART0 and UART4 clocks */
  (*(REG32(CM_WKUP + 0xB4))) =0x2;
  (*(REG32(PRCM_REG + 0x78))) =0x2;

	/* Disable WatchDog */ 
	(*(REG32(WDT_BASE + WDT_WSPR))) = 0xAAAA; // WDT Disable Sequence #1
	while( (*(REG32(WDT_BASE + WDT_WWPS))) & (1<<4) ); //wait till Write pending ready
	(*(REG32(WDT_BASE + WDT_WSPR))) = 0x5555; // WDT Disable Sequence #2
	while( (*(REG32(WDT_BASE + WDT_WWPS))) & (1<<4) ); //wait till Write pending ready
//	(*(REG32(WDT_BASE + WDT_WLDR))) = 0x0;

	//Setup GPIO pins
	(*(REG32(CONTROL_MODULE + 0x95c))) = 0x07; // --> 3(h)
	(*(REG32(CONTROL_MODULE + 0x958))) = 0x07; // --> 10(f)
	(*(REG32(CONTROL_MODULE + 0x954))) = 0x07; // --> 2(d)
	(*(REG32(CONTROL_MODULE + 0x984))) = 0x07; // --> 12(DIG1)
	(*(REG32(CONTROL_MODULE + 0x980))) = 0x07; // not use
	(*(REG32(CONTROL_MODULE + 0x990))) = 0x07; // --> 1(e) NEW
	(*(REG32(CONTROL_MODULE + 0x998))) = 0x07; // --> 11(a) NEW
	//Setup UART4 pins
	(*(REG32(CONTROL_MODULE + 0x840))) = 0x07; // --> 4(c)
	(*(REG32(CONTROL_MODULE + 0x844))) = 0x07; // --> 1(e)
	(*(REG32(CONTROL_MODULE + 0x848))) = 0x07; // --> 8(DIG3)
	(*(REG32(CONTROL_MODULE + 0x84C))) = 0x07; // --> 9(DIG2)
	(*(REG32(CONTROL_MODULE + 0x870))) = 0x07; // --> 6(DIG4)
	(*(REG32(CONTROL_MODULE + 0x874))) = 0x07; // --> 5(g)
	(*(REG32(CONTROL_MODULE + 0x99C))) = 0x07; // --> 7(b) NEW

// 7seg's A~H
	(*(REG32(CONTROL_MODULE + 0x890))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x89C))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x834))) = 0x07;	// 7 seg(GND)
	(*(REG32(CONTROL_MODULE + 0x824))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x83C))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x838))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x828))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x830))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x87C))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x894))) = 0x07;	// 7 seg
// 7seg's DIG1~4
	(*(REG32(CONTROL_MODULE + 0x82C))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x820))) = 0x07;	// 7 seg
	(*(REG32(CONTROL_MODULE + 0x88C))) = 0x07;	// 7 seg

	(*(REG32(CONTROL_MODULE + 0x844))) = 0x07;	// Buzzer

/*
GPIO2_2(7)		--> 1(e)TIMER4      	GPIO0_27(17)--> 12(DIG1)GPIO0_27
GPIO2_5(9)		--> 2(d)TIMER5      	GPIO1_12(12)--> 11(a)GPIO1_12
GPIO1_15(15)	--> 3(h)GPIO1_15    	GPIO1_29(10)--> 10(f)GPIO1_29
GPIO0_23(13)	--> 4(c)EHRPWM2B    	GPIO0_22(19)--> 9(DIG2)EHRPWM2A
GPIO2_3(8)		--> 5(g)TIMER7      	GPIO2_1(18)	--> 8(DIG3)GPIO2_1
GPIO1_13(11)	--> 6(DIG4)GPIO1_13 	GPIO1_14(16)--> 7(b)GPIO1_14

890h GPMC_ADVN_ALE/TIMER4/GPIO2_2(7) --> 1(e)      
89Ch GPMC_BE0N_CLE/TIMER5/GPIO2_5(9) --> 2(d)
83ch GPMC_AD15/LCD_DATA16/MMC1_DAT7/MMC2_DAT3/EQEP2_STROBE/PR1_ECAP0_ECAP_CAPIN_APWM_O/PR1_PRU0_PRU_R31_15/GPIO1_15(15) --> 3(h)
824h GPMC_AD9/LCD_DATA22/MMC1_DAT1/MMC2_DAT5/EHRPWM2B/PR1_MII0_CRS//GPIO0_23(13) --> 4(c)      
894h GPMC_OEN_REN/TIMER7/EMU4/GPIO2_3(8) --> 5(g)    
834h GPMC_AD13/LCD_DATA18/MMC1_DAT5/MMC2_DAT1/EQEP2B_IN/PR1_MII0_TXD1/PR1_PRU0_PRU_R30_15/GPIO1_13(11) --> 6(DIG4)
838h GPMC_AD14/LCD_DATA17/MMC1_DAT6/MMC2_DAT2/EQEP2_INDEX/PR1_MII0_TXD0/PR1_PRU0_PRU_R31_14/GPIO1_14(16) --> 7(b)
88Ch GPMC_CLK/LCD_MEM_CLK/GPMC_WAIT1/MMC2_CLK/PRT1_MII1_TXEN/MCASP0_FSR/GPIO2_1(18) --> 8(DIG3)
820h GPMC_AD8/LCD_DATA23/MMC1_DAT0/MMC2_DAT4/EHRPWM2A/PR1_MII_MT0_CLK//GPIO0_22(19) --> 9(DIG2)
87Ch GPMC_CSN0/GPIO1_29(10) --> 10(f) 
830h GPMC_AD12/LCD_DATA19/MMC1_DAT4/MMC2_DAT0/EQEP2A_IN/PR1_MII0_TXD2/PR1_PRU0_PRU_R30_14/GPIO1_12(12) --> 11(a)
82Ch GPMC_AD11/LCD_DATA20/MMC1_DAT3/MMC2_DAT7/EHRPWM2_SYNCI_O/PR1_MII0_TXD3//GPIO0_27(17) --> 12(DIG1)
844h GPMC_A1/GMII2_RXDV/RGMII2_RCTL/MMC2_DAT0/GPMC_A17/PR1_MII1_TXD3/EHRPWM1_SYNCI_O/GPIO1_17
*/

  /* Controlling the output capability */
  (*(REG32(GPIO1_BASE+GPIO_OE))) = ~(PIN9|PIN12|PIN13|PIN14|PIN17|PIN15|PIN21|PIN22|PIN23|PIN24|PIN16|PIN17|PIN18|PIN19|PIN29);  

//	(*(REG32(GPIO0_BASE+0x30))) |= (PIN2 | PIN14 | PIN15);  //IRQ Status  
//	(*(REG32(GPIO0_BASE+0x34))) |= (PIN2 | PIN14 | PIN15);  //IRQ Status  set
//	(*(REG32(GPIO0_BASE+0x14C))) |= (PIN2 | PIN14 | PIN15);  //Falling edge
	(*(REG32(GPIO0_BASE+GPIO_OE))) = ~(PIN3 | PIN4 | PIN5 | PIN15 | PIN22 | PIN23 | PIN26 | PIN27 | PIN30 | PIN31);  

  (*(REG32(GPIO0_BASE+GPIO_SETDATAOUT))) |= (PIN3 | PIN4 | PIN5 | PIN15 | PIN27 | PIN30 | PIN31);
//	(*(REG32(GPIO0_BASE+GPIO_SETDATAOUT))) = (PIN22|PIN23|PIN26|PIN27|PIN23); // 7seg
	(*(REG32(GPIO0_BASE+GPIO_CLEARDATAOUT))) = (PIN23|PIN23|PIN26|PIN27|PIN23); // 7seg
 
  /* Switch off the leds */ 
  (*(REG32(GPIO1_BASE+GPIO_SETDATAOUT))) = PIN12|PIN13|PIN24|PIN23|PIN22|PIN21|PIN16|PIN18|PIN19; 
//	(*(REG32(GPIO1_BASE+GPIO_SETDATAOUT))) = (PIN14|PIN15|PIN29); // 7seg
	(*(REG32(GPIO1_BASE+GPIO_CLEARDATAOUT))) = (PIN14|PIN15|PIN29); // 7seg

  (*(REG32(GPIO3_BASE+GPIO_OE))) = ~(PIN9|PIN14|PIN16|PIN17);  
  (*(REG32(GPIO3_BASE+GPIO_SETDATAOUT))) = (PIN9|PIN14|PIN16|PIN17);

  (*(REG32(GPIO2_BASE+GPIO_OE))) = ~(PIN1|PIN2|PIN3|PIN5);
//  (*(REG32(GPIO2_BASE+GPIO_SETDATAOUT))) = (PIN1|PIN2|PIN3|PIN5); // 7seg
  (*(REG32(GPIO2_BASE+GPIO_CLEARDATAOUT))) = (PIN1|PIN2|PIN3|PIN5); // 7seg

//  while(1);
} 
