#include "p33fxxxx.h"	//zawiera opis w³aœciwoœci PIC
#include "pwm.h"	//sterowanie silnikami
#include "qei.h"	//odczyt wartoœci z enkoderów
#include "config.h"	//zawiera miêdzy innymi rozdzielczoœæ enkodera i TIMEOUT

#include "control.h"
#include <dsp.h>
#include "nf/nfv2.h"

#define SAMPLES 4		//ilosc probek szybkosci
#define MAX_OUTPUT 32767	//maksymalne sterowanie wyjsciowe
#define B 1;				//wspó³czynnik proporcjonalnoœci do si³y przeciwelektromotorycznej

extern NF_STRUCT_ComBuf NFComBuf;	//struktura do komunikowania siê z komputerem

int kCoeffs[3];			//tablica wspó³czynników regulatorów PID
int Ki;					//zapamiêtana wartoœæ wzmocnienia cz³onu ca³kuj¹cego w przypdaku zajœcia ograniczenia ca³kowania

int speed1 = 0;		//obecna prêdkoœæ silnika lewego
int speed2 = 0;		//obecna prêdkoœæ silnika prawego

int tspeed1 = 0;		//po¿¹dana prêdkoœæ silnika lewego
int tspeed2 = 0;		//po¿¹dana prêdkoœæ silnika prawego

int lpos1 = 0;		//poprzednia iloœæ impulsów lewego ko³a
int lpos2 = 0;		//poprzednia iloœæ impulsów prawego ko³a

int spi = 0;		//licznik miejsca w tabeli stanów które bêdziemy nadpisywaæ
int time = 0;		//licznik obiegów nadzoruj¹cy wy³¹czenie silników po zadanym okresie braku ³¹cznoœci

tPID pid1;	//sterownik PID silnika lewego
tPID pid2;	//sterownik PID silnika prawego

fractional abcCoefficient1[3] __attribute__ ((section (".xbss, bss, xmemory")));	//tabela wspó³czynników regulatora PID lewego silnika
fractional controlHistory1[3] __attribute__ ((section (".ybss, bss, ymemory")));	//tabela zapamiêtanych wartoœci b³êdu regultora PID lewego silnika

fractional abcCoefficient2[3] __attribute__ ((section (".xbss, bss, xmemory")));	//tabela wspó³czynników regulatora PID prawego silnika
fractional controlHistory2[3] __attribute__ ((section (".ybss, bss, ymemory")));	//tabela zapamiêtanych wartoœci b³êdu regultora PID prawego silnika

int speedbuf1[SAMPLES];	//bufor pomiarów prêdkoœci silnika lewego
int speedbuf2[SAMPLES];	//bufor pomiarów prêdkoœci slinika prawego



void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
	int pos;
	if(NFComBuf.dataReceived == 1) 
	{
		if(NFComBuf.SetDrivesSpeed.updated)
		{
			tspeed1 = NFComBuf.SetDrivesSpeed.data[0];		//przepisanie prêdkoœci zadanej silnika lewego do zmiennej lokalnej
			tspeed2 = NFComBuf.SetDrivesSpeed.data[1];		//przepisanie prêdkoœci zadanej silnika prawego do zmiennej lokalnej
			time = 0;										//zerowanie licznika wykonanych pêtli bez otrzymania nowego polecenia sterowania
			NFComBuf.SetDrivesSpeed.updated = 0;
		}
		if (NFComBuf.SetDrivesMode.updated) 
		{
			if(NFComBuf.SetDrivesMode.data[0] == NF_DrivesMode_ERROR) 
			{
				PIDInit(&pid1);
				PIDInit(&pid2);
				time = TIMEOUT;
			}
			NFComBuf.SetDrivesMode.updated = 0;
		}
		NFComBuf.dataReceived = 0;
	}

	//wyliczanie obecnej prêdkoœci lewych kó³
	pos = GetPos1();					//iloœæ impulsów niepe³nego obrotu
	pos += 4 * ENCRES * GetIndex1();	//sumowanie z iloœi¹ pe³nych obrotów razy rozdzielczoœæ enkodera
	
	speedbuf1[spi] = pos - lpos1;	//obecny wzrost impulsów = obecna iloœæ impulsów - poprzednia wartoœæ impulsów
	lpos1 = pos;					//nadpisanie poprzedniej wartoœci impulsów
	
	speed1 = 0;
	int i;
	for( i = 0; i < SAMPLES; i++)
	{
		speed1 += speedbuf1[i];		//wyliczanie prêdkoœci ko³a
	}
	b = SAMPLES*tspeed1*B;
	pid1.measuredOutput = speed1;
	pid1.controlReference = SAMPLES*tspeed1;
	PID(&pid1);									//wykonanie obliczeñ regulatora PID w celu wyznaczenia bie¿¹cego sterowania

	//ograniczenie calkowania
	if(pid1.controlOutput+b <= -MAX_OUTPUT || pid1.controlOutput >= MAX_OUTPUT)
	{
		kCoeffs[1] = Q15(0.0);		//zerowanie wspó³czynnika ca³kuj¹cego
		PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie nowych wspó³czynniów regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	else
	{
		kCoeffs[1] = Ki;		//przywrócenie wartoœci wspó³czynnika ca³kuj¹cego
		PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie nowych wspó³czynniów regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	
	//ustawienie sterowania lewego silnika
	if(time < TIMEOUT)
	{
		SetPWM1(pid1.controlOutput);				//ustawienie sterowania lewego silnika zgodnie z wartoœci¹ wyliczon¹ przez regulator PID
	} else 
	{
		SetPWM1(0);									//zatrzymanie lewego silnika
	}

	//wyliczanie obecnej prêdkoœci prawych kó³	
	pos = GetPos2();					//iloœæ impulsów niepe³nego obrotu
	pos += 4 * ENCRES * GetIndex2();	//sumowanie z iloœi¹ pe³nych obrotów razy rozdzielczoœæ enkodera
	
	speedbuf2[spi] = pos - lpos2;	//obecny wzrost impulsów = obecna iloœæ impulsów - poprzednia wartoœæ impulsów
	lpos2 = pos;					//nadpisanie poprzedniej wartoœci impulsów

	
	speed2 = 0;
	for( i = 0; i < SAMPLES; i++)
	{
		speed2 += speedbuf2[i];
	}
	b = SAMPLES*tspeed2*B;	
	pid2.measuredOutput = speed2;
	pid2.controlReference = SAMPLES*tspeed2*B;
	PID(&pid2);									//wykonanie obliczeñ regulatora PID w celu wyznaczenia bie¿¹cego sterowania

	//ograniczenie calkowania
	if(pid2.controlOutput <= -MAX_OUTPUT || pid2.controlOutput >= MAX_OUTPUT)
	{
		kCoeffs[1] = Q15(0.0);		//zerowanie wspó³czynnika ca³kuj¹cego
		PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie nowych wspó³czynniów regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	else
	{
		kCoeffs[1] = Ki;		//przywrócenie wartoœci wspó³czynnika ca³kuj¹cego
		PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie nowych wspó³czynniów regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}

	//ustawienie sterowania prawego silnika	
	if(time < TIMEOUT)
	{
		SetPWM2(pid2.controlOutput);				//ustawienie sterowania prawego silnika zgodnie z wartoœci¹ wyliczon¹ przez regulator PID
	}else
	{
		SetPWM2(0);									//zatrzymanie prawego silnika
	}
	
	if(time < TIMEOUT)
	{
		++time;
	}
	
	NFComBuf.ReadDrivesPosition.data[0] = speed1 / SAMPLES;
	NFComBuf.ReadDrivesPosition.data[1] = speed2 / SAMPLES;

	spi++;
	if(spi >= SAMPLES)
	{
		spi = 0;
	}
	
	IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
}

void Init()
{
	kCoeffs[0] = Q15(0.9);		//wspó³czynnik linowy regultorów PID
	kCoeffs[1] = Q15(0.8);		//wspó³czynnik ca³kuj¹cy regultorów PID
	Ki = kCoeffs[1];			//zapamiêtanie wartoœci wspó³czynnika ca³kuj¹cego regulatorów PID
	kCoeffs[2] = Q15(0.0);		//wspó³czynnik ró¿niczkowy regultorów PID

	pid1.abcCoefficients = &abcCoefficient1[0];    //Przekazanie wskaznika regulatorowi PID lewego silnika na tablicê jego wspó³czynników
    pid1.controlHistory = &controlHistory1[0];     //Przekazanie wskaznika regulatorowi PID lewego silnika na tablicê jego historii b³êdów

	pid2.abcCoefficients = &abcCoefficient2[0];    //Przekazanie wskaznika regulatorowi PID prawego silnika na tablicê jego wspó³czynników
    pid2.controlHistory = &controlHistory2[0];     //Przekazanie wskaznika regulatorowi PID prawego silnika na tablicê jego historii b³êdów

	PIDInit(&pid1);		//inicializacja regulatora PID lewego silnika- czyszczenie controlHistory i controlOutput
	PIDInit(&pid2);		//inicializacja regulatora PID prawego silnika- czyszczenie controlHistory i controlOutput

	PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie wspó³czynniów regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie wspó³czynniów regulatora PID prawego silnika i wpisanie ich do tablicy abcCoefficient2[]; 
}
