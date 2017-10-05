#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#include <Arduino.h>

#define CAT(A, B) A##B
#define PORT(P) CAT(PORT, P)
#define PIN(P) CAT(PIN, P)
#define DDR(P) CAT(DDR, P)

#define NIN_SLEEP1 asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" \
            "nop\nnop\n")
#define NIN_SLEEP3 asm("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" \
            "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" \
            "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" \
            "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" \
            "nop\nnop\nnop\nnop\nnop\nnop\nnop\n")
	
#define DATA_SAMPLE_TYPE uint8_t	
#define DATA_PORT_TYPE volatile DATA_SAMPLE_TYPE

#define START_TRANSMISSION *ddrAddress |= g_dataPortMask;
#define STOP_TRANSMISSION *ddrAddress &= ~g_dataPortMask;

#define BUF_SIZE 1024

#define SAMPLE_DATA(VAR) \
{\
    DATA_SAMPLE_TYPE *s = VAR; \
	while (s - VAR < BUF_SIZE) \
	{ \
        *s++ = *g_inDataPortAddress; \
	} \
}

extern volatile uint8_t *ddrAddress;

void NIN_arduinoSetDataPort(char port);

#endif /* _ARDUINO_H_ */