

#include "p33fxxxx.h"

#define  NUM_CHS2SCAN			2		// Number of channels enabled for channel scan

/*=============================================================================
ADC INITIALIZATION FOR CHANNEL SCAN
=============================================================================*/
void InitAdc1(void)
{

		AD1CON1bits.FORM   = 0;		// Data Output Format: Signed Fraction (Q15 format)
		AD1CON1bits.SSRC   = 2;		// Sample Clock Source: GP Timer starts conversion
		AD1CON1bits.ASAM   = 1;		// ADC Sample Control: Sampling begins immediately after conversion
		AD1CON1bits.AD12B  = 0;		// 10-bit ADC operation

		AD1CON2bits.CSCNA = 1;		// Scan Input Selections for CH0+ during Sample A bit
		AD1CON2bits.CHPS  = 0;		// Converts CH0

		AD1CON3bits.ADRC = 0;		// ADC Clock is derived from Systems Clock
		AD1CON3bits.ADCS = 1000;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
									// ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us

		AD1CON2bits.SMPI    = (NUM_CHS2SCAN-1);	// 4 ADC Channel is scanned

		//AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
	//	_AD1CSSH = 0x0000;			
		AD1CSSLbits.CSS0=1;			// Enable AN4 for channel scan
		AD1CSSLbits.CSS1=1;			// Enable AN5 for channel scan
//		AD1CSSLbits.CSS2=1;		// Enable AN10 for channel scan
//		AD1CSSLbits.CSS3=1;		// Enable AN13 for channel scan
 
 		//AD1PCFGH/AD1PCFGL: Port Configuration Register
		AD1PCFGL=0xFFFF;
	//	AD1PCFGLbits.PCFG0 = 0;		// AN0 as Analog Input
	//	AD1PCFGLbits.PCFG1 = 0;		// AN1 as Analog Input 
 	//	AD1PCFGLbits.PCFG2 = 0;	// AN10 as Analog Input
	//	AD1PCFGLbits.PCFG3 = 0;	// AN13 as Analog Input 
	
        
        IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
        IEC0bits.AD1IE = 1;			// Enable A/D interrupt 
        AD1CON1bits.ADON = 1;		// Turn on the A/D converter	


}

/*=============================================================================  
ADC INTERRUPT SERVICE ROUTINE
=============================================================================*/

int adc0;
int adc1;

int  scanCounter=0;

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{

	switch (scanCounter)
	{
		case 0:	
			adc0=ADC1BUF0; 
			break;

		case 1:
			adc1=ADC1BUF0; 
			break;			
		default:
			break;			
			
	}

	scanCounter++;
    IFS0bits.AD1IF = 0;		// Clear the ADC1 Interrupt Flag

}
