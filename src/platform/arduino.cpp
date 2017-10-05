#include <nin_comm.h>

volatile uint8_t *ddrAddress;

void NIN_arduinoSetDataPort(char port)
{
	volatile uint8_t *portAddress;
	
	if (port >= 'B' && port <= 'D')
	{
		ddrAddress = &DDRB + (port - 'B') * 3;
		
		portAddress = &PORTB + (port - 'B') * 3;
		NIN_setOutDataPort(portAddress);
		
		portAddress = &PINB + (port - 'B') * 3;
		NIN_setInDataPort(portAddress);
	}
}