# MPLAB IDE generated this makefile for use with GNU make.
# Project: ServoControl.mcp
# Date: Wed Aug 06 16:00:44 2014

AS = xc16-as.exe
CC = xc16-gcc.exe
LD = xc16-ld.exe
AR = xc16-ar.exe
HX = xc16-bin2hex.exe
RM = rm

ServoControl.hex : ServoControl.cof
	$(HX) "ServoControl.cof" -omf=coff

ServoControl.cof : main.o uart.o qei.o timer.o adc.o pwm.o serial.o control.o nfv2.o
	$(CC) -omf=coff -mcpu=33FJ32MC302 "main.o" "uart.o" "qei.o" "timer.o" "adc.o" "pwm.o" "serial.o" "control.o" "nfv2.o" -o"ServoControl.cof" -Wl,--script="linkerscript.gld",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,--defsym=__ICD2RAM=1,-Map="ServoControl.map",--report-mem

main.o : timer.h pwm.h qei.h uart.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h main.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

uart.o : nfv2_config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdint.h nf/nfv2.h config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h uart.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "uart.c" -o"uart.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

qei.o : ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdint.h config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h qei.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "qei.c" -o"qei.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

timer.o : config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h timer.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "timer.c" -o"timer.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

adc.o : ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h adc.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "adc.c" -o"adc.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

pwm.o : pwm.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h pwm.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "pwm.c" -o"pwm.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

serial.o : nfv2_config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdint.h nf/nfv2.h serial.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h serial.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "serial.c" -o"serial.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

control.o : nfv2_config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdint.h nf/nfv2.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/math.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stddef.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdlib.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/generic/h/dsp.h control.h config.h qei.h pwm.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33FJ32MC302.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/support/dsPIC33F/h/p33fxxxx.h control.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "control.c" -o"control.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

nfv2.o : nfv2_config.h ../../../../program\ files\ (x86)/microchip/xc16/v1.21/include/stdint.h nf/nfv2.h nf/nfv2.c
	$(CC) -omf=coff -mcpu=33FJ32MC302 -x c -c "nf\nfv2.c" -o"nfv2.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

clean : 
	$(RM) "main.o" "uart.o" "qei.o" "timer.o" "adc.o" "pwm.o" "serial.o" "control.o" "nfv2.o" "ServoControl.cof" "ServoControl.hex"

