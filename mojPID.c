#include "p33fxxxx.h"	//zawiera opis w�a�ciwo�ci PIC
#include "pwm.h"	//sterowanie silnikami
#include "qei.h"	//odczyt warto�ci z enkoder�w
#include "config.h"	//zawiera mi�dzy innymi rozdzielczo�� enkodera i TIMEOUT

#include "control.h"
#include <dsp.h>
#include "nf/nfv2.h"

#define SAMPLES 4		//ilosc probek szybkosci
#define MAX_OUTPUT 32767	//maksymalne sterowanie wyjsciowe
#define B 1;				//wsp�czynnik proporcjonalno�ci do si�y przeciwelektromotorycznej

extern NF_STRUCT_ComBuf NFComBuf;	//struktura do komunikowania si� z komputerem

int kCoeffs[3];			//tablica wsp�czynnik�w regulator�w PID
int Ki;					//zapami�tana warto�� wzmocnienia cz�onu ca�kuj�cego w przypdaku zaj�cia ograniczenia ca�kowania

int speed1 = 0;		//obecna pr�dko�� silnika lewego
int speed2 = 0;		//obecna pr�dko�� silnika prawego

int tspeed1 = 0;		//po��dana pr�dko�� silnika lewego
int tspeed2 = 0;		//po��dana pr�dko�� silnika prawego

int lpos1 = 0;		//poprzednia ilo�� impuls�w lewego ko�a
int lpos2 = 0;		//poprzednia ilo�� impuls�w prawego ko�a

int spi = 0;		//licznik miejsca w tabeli stan�w kt�re b�dziemy nadpisywa�
int time = 0;		//licznik obieg�w nadzoruj�cy wy��czenie silnik�w po zadanym okresie braku ��czno�ci

tPID pid1;	//sterownik PID silnika lewego
tPID pid2;	//sterownik PID silnika prawego

fractional abcCoefficient1[3] __attribute__ ((section (".xbss, bss, xmemory")));	//tabela wsp�czynnik�w regulatora PID lewego silnika
fractional controlHistory1[3] __attribute__ ((section (".ybss, bss, ymemory")));	//tabela zapami�tanych warto�ci b��du regultora PID lewego silnika

fractional abcCoefficient2[3] __attribute__ ((section (".xbss, bss, xmemory")));	//tabela wsp�czynnik�w regulatora PID prawego silnika
fractional controlHistory2[3] __attribute__ ((section (".ybss, bss, ymemory")));	//tabela zapami�tanych warto�ci b��du regultora PID prawego silnika

int speedbuf1[SAMPLES];	//bufor pomiar�w pr�dko�ci silnika lewego
int speedbuf2[SAMPLES];	//bufor pomiar�w pr�dko�ci slinika prawego



void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
	int pos;
	if(NFComBuf.dataReceived == 1) 
	{
		if(NFComBuf.SetDrivesSpeed.updated)
		{
			tspeed1 = NFComBuf.SetDrivesSpeed.data[0];		//przepisanie pr�dko�ci zadanej silnika lewego do zmiennej lokalnej
			tspeed2 = NFComBuf.SetDrivesSpeed.data[1];		//przepisanie pr�dko�ci zadanej silnika prawego do zmiennej lokalnej
			time = 0;										//zerowanie licznika wykonanych p�tli bez otrzymania nowego polecenia sterowania
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

	//wyliczanie obecnej pr�dko�ci lewych k�
	pos = GetPos1();					//ilo�� impuls�w niepe�nego obrotu
	pos += 4 * ENCRES * GetIndex1();	//sumowanie z ilo�i� pe�nych obrot�w razy rozdzielczo�� enkodera
	
	speedbuf1[spi] = pos - lpos1;	//obecny wzrost impuls�w = obecna ilo�� impuls�w - poprzednia warto�� impuls�w
	lpos1 = pos;					//nadpisanie poprzedniej warto�ci impuls�w
	
	speed1 = 0;
	int i;
	for( i = 0; i < SAMPLES; i++)
	{
		speed1 += speedbuf1[i];		//wyliczanie pr�dko�ci ko�a
	}
	b = SAMPLES*tspeed1*B;
	pid1.measuredOutput = speed1;
	pid1.controlReference = SAMPLES*tspeed1;
	PID(&pid1);									//wykonanie oblicze� regulatora PID w celu wyznaczenia bie��cego sterowania

	//ograniczenie calkowania
	if(pid1.controlOutput+b <= -MAX_OUTPUT || pid1.controlOutput >= MAX_OUTPUT)
	{
		kCoeffs[1] = Q15(0.0);		//zerowanie wsp�czynnika ca�kuj�cego
		PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie nowych wsp�czynni�w regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	else
	{
		kCoeffs[1] = Ki;		//przywr�cenie warto�ci wsp�czynnika ca�kuj�cego
		PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie nowych wsp�czynni�w regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	
	//ustawienie sterowania lewego silnika
	if(time < TIMEOUT)
	{
		SetPWM1(pid1.controlOutput);				//ustawienie sterowania lewego silnika zgodnie z warto�ci� wyliczon� przez regulator PID
	} else 
	{
		SetPWM1(0);									//zatrzymanie lewego silnika
	}

	//wyliczanie obecnej pr�dko�ci prawych k�	
	pos = GetPos2();					//ilo�� impuls�w niepe�nego obrotu
	pos += 4 * ENCRES * GetIndex2();	//sumowanie z ilo�i� pe�nych obrot�w razy rozdzielczo�� enkodera
	
	speedbuf2[spi] = pos - lpos2;	//obecny wzrost impuls�w = obecna ilo�� impuls�w - poprzednia warto�� impuls�w
	lpos2 = pos;					//nadpisanie poprzedniej warto�ci impuls�w

	
	speed2 = 0;
	for( i = 0; i < SAMPLES; i++)
	{
		speed2 += speedbuf2[i];
	}
	b = SAMPLES*tspeed2*B;	
	pid2.measuredOutput = speed2;
	pid2.controlReference = SAMPLES*tspeed2*B;
	PID(&pid2);									//wykonanie oblicze� regulatora PID w celu wyznaczenia bie��cego sterowania

	//ograniczenie calkowania
	if(pid2.controlOutput <= -MAX_OUTPUT || pid2.controlOutput >= MAX_OUTPUT)
	{
		kCoeffs[1] = Q15(0.0);		//zerowanie wsp�czynnika ca�kuj�cego
		PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie nowych wsp�czynni�w regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}
	else
	{
		kCoeffs[1] = Ki;		//przywr�cenie warto�ci wsp�czynnika ca�kuj�cego
		PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie nowych wsp�czynni�w regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	}

	//ustawienie sterowania prawego silnika	
	if(time < TIMEOUT)
	{
		SetPWM2(pid2.controlOutput);				//ustawienie sterowania prawego silnika zgodnie z warto�ci� wyliczon� przez regulator PID
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
	kCoeffs[0] = Q15(0.9);		//wsp�czynnik linowy regultor�w PID
	kCoeffs[1] = Q15(0.8);		//wsp�czynnik ca�kuj�cy regultor�w PID
	Ki = kCoeffs[1];			//zapami�tanie warto�ci wsp�czynnika ca�kuj�cego regulator�w PID
	kCoeffs[2] = Q15(0.0);		//wsp�czynnik r�niczkowy regultor�w PID

	pid1.abcCoefficients = &abcCoefficient1[0];    //Przekazanie wskaznika regulatorowi PID lewego silnika na tablic� jego wsp�czynnik�w
    pid1.controlHistory = &controlHistory1[0];     //Przekazanie wskaznika regulatorowi PID lewego silnika na tablic� jego historii b��d�w

	pid2.abcCoefficients = &abcCoefficient2[0];    //Przekazanie wskaznika regulatorowi PID prawego silnika na tablic� jego wsp�czynnik�w
    pid2.controlHistory = &controlHistory2[0];     //Przekazanie wskaznika regulatorowi PID prawego silnika na tablic� jego historii b��d�w

	PIDInit(&pid1);		//inicializacja regulatora PID lewego silnika- czyszczenie controlHistory i controlOutput
	PIDInit(&pid2);		//inicializacja regulatora PID prawego silnika- czyszczenie controlHistory i controlOutput

	PIDCoeffCalc(&kCoeffs[0], &pid1);	//wyliczenie wsp�czynni�w regulatora PID lewego silnika i wpisanie ich do tablicy abcCoefficient1[];
	PIDCoeffCalc(&kCoeffs[0], &pid2);	//wyliczenie wsp�czynni�w regulatora PID prawego silnika i wpisanie ich do tablicy abcCoefficient2[]; 
}
