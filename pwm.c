
#include "p33fxxxx.h"
#include "pwm.h"

void InitPWM()
{
	/* PWM time base operates in a Free Running mode */
	P1TCONbits.PTMOD = 0b00;
	/* PWM time base input clock period is TCY (1:1 prescale) */
	/* PWM time base output post scale is 1:1 */
	P1TCONbits.PTCKPS = 0b00;
	P1TCONbits.PTOPS = 0b00;
	/* Choose PWM time period based on input clock selected */
	/* Refer to Equation 14-1 */
	/* PWM switching frequency is 20 kHz */
	/* FCY is 20 MHz */
	P1TPER = 999;

	/* PWM I/O pairs 1 to 3 are in independent mode */
	/* PWM pins are enabled for PWM output */
	PWM1CON1bits.PMOD1 = 1;
	PWM1CON1bits.PMOD2 = 1;
	PWM1CON1bits.PMOD3 = 1;
	PWM1CON1bits.PEN1H = 0;
	PWM1CON1bits.PEN2H = 0;
	PWM1CON1bits.PEN3H = 1;
	PWM1CON1bits.PEN1L = 0;
	PWM1CON1bits.PEN2L = 1;
	PWM1CON1bits.PEN3L = 0;
	/* Immediate update of PWM enabled */
	PWM1CON2bits.IUE = 1;
	// PWM I/O pin controlled by PWM Generator
	P1OVDCONbits.POVD3H = 1;
	P1OVDCONbits.POVD2H = 0;
	P1OVDCONbits.POVD1H = 0;
	P1OVDCONbits.POVD3L = 0;
	P1OVDCONbits.POVD2L = 1;
	P1OVDCONbits.POVD1L = 0;

	/* Initialize duty cycle values for PWM1, PWM2 and PWM3 signals */
	P1DC3 = 0; // testing only !!! TODO : set to zero
	P1DC2 = 0; // testing only !!! TODO : set to zero


	P1TCONbits.PTEN = 1;

}

void SetPWM1(int pwm)
{
	if(pwm<0)
	{
		P1DC2 = pwm * -1;
		PWM1_DIR = 0;
	} else
	{
		P1DC2 = pwm;
		PWM1_DIR = 1;
	}
}

void SetPWM2(int pwm)
{
	if(pwm<0)
	{
		P1DC3 = pwm * -1;
		PWM2_DIR = 1;
	} else
	{
		P1DC3 = pwm;
		PWM2_DIR = 0;
	}
}
