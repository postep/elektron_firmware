#define __sdPIC33F__

#include "p33fxxxx.h"

#include	"uart.h"
#include	"qei.h"
#include	"pwm.h"
#include	"timer.h"

_FOSC( FCKSM_CSECME & OSCIOFNC_ON & POSCMD_NONE & IOL1WAY_OFF );
_FWDT( FWDTEN_OFF );

void InitIO()
{
	__builtin_write_OSCCONL(OSCCON & ~(1<<6));
	AD1PCFGL = 0xFFFF;

	//_TRISB7 = 0;
//	_ODCB7 = 0;

	_U1RXR = 3; // connect UART reciver to RP3
	_RP4R = 3; // connect UART transcriver to RP4
	
	//QEI1
	_QEA1R = 1;
	_QEB1R = 0;
	_INDX1R = 2;
	//QEI2
	_QEA2R = 8;
	_QEB2R = 7;
	_INDX2R = 9;


	/* PWM direction pins */
	_TRISB15 = 0;
	_ODCB15 = 0;
	_RB15 = 1;

	_TRISB12 = 0;
	_ODCB12 = 0;
	_RB12 = 1;

	_TRISB11 = 0;
	_ODCB11 = 0;
	_RB11 = 0;

	_TRISB14 = 0;
	_ODCB14 = 0;
	_RB14 = 0;

	_TRISA4 = 0;
	_ODCA4 = 0;
	_RA4 = 0;
	__builtin_write_OSCCONL(OSCCON | (1<<6));
}

void InitClock() 
{
	PLLFBD = 41;	// M = 40
	CLKDIVbits.PLLPOST = 0;	// N1 = 2
	CLKDIVbits.PLLPRE = 0;	// N2 = 2
	OSCTUN = 0;
	RCONbits.SWDTEN = 0;

	// Clock switch to incorporate PLL
	__builtin_write_OSCCONH(0x01);		// Initiate Clock Switch to 
													// FRC with PLL (NOSC=0b001)
	__builtin_write_OSCCONL(0x01);		// Start clock switching
	
	// disable two next lines when emulating in proteus VMS
	while (OSCCONbits.COSC != 0b001);	// Wait for Clock switch to occur	
	while(OSCCONbits.LOCK != 1) {};
}

void initTmr3() 
{
        TMR3 = 0x0000;
        PR3 = 4999;
        IFS0bits.T3IF = 0;
        IEC0bits.T3IE = 0;

        //Start Timer 3
        T3CONbits.TON = 1;

}

int main()
{

	InitClock();
	InitIO();
	InitQEI1();
	InitQEI2();
	InitPWM();
	Init();
    InitUART1();
	InitTIMER();

	while(1);

	return 0;
}
