#include "p33fxxxx.h"

#include	"serial.h"

#include 	"nf/nfv2.h"

NF_STRUCT_ComBuf NFComBuf;

uint8_t rxBuf[128];
uint8_t txBuf[128];
uint8_t rxPt = 0;
uint8_t u1commArray[15];
uint8_t u1commCnt = 0;

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
	if(rxPt > 127)
		rxPt = 0;
	// Read one byte from the receive data register
	rxBuf[rxPt] = U1RXREG;
	
	if(NF_Interpreter(&NFComBuf, (uint8_t*) rxBuf, (uint8_t*) &rxPt, u1commArray, &u1commCnt) > 0){
		NFComBuf.dataReceived = 1;
		if(u1commCnt > 0){
			uint8_t u1BytesToSend = NF_MakeCommandFrame(&NFComBuf, (uint8_t*)txBuf, (const uint8_t*)u1commArray, u1commCnt, NFComBuf.myAddress);
			if(u1BytesToSend > 0){
				WriteUART1((uint8_t*)txBuf, u1BytesToSend);
			}
		}
	}
	// Clear the USARTx Receive interrupt
	IFS0bits.U1RXIF = 0;
}
