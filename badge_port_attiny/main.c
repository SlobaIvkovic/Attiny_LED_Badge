#define F_CPU 3000000UL			// cpu frequency. By default with prescaller it is about 3MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <sys/types.h>

#define EEPROM_ADDR 1

#define OUT1_1        PORTA.OUTSET = 0x80	// set PA7			// out1 on
#define OUT1_0        PORTA.OUTCLR = 0x80	// clear PA7		// out1 off
#define OUT2_1        PORTA.OUTSET = 0x40	// set PA6			// out2 on
#define OUT2_0        PORTA.OUTCLR = 0x40	// clear PA6		// out2 off

#define OUTS_1        PORTA.OUTSET = 0xF0	// set PA4-7		// all outs on
#define OUTS_0        PORTA.OUTCLR = 0xF0	// clear PA4-7		// all outs off

#define OUTPB2_1        PORTB.OUTSET = 0x04
#define OUTPB2_0       PORTB.OUTCLR = 0x04	

#define ON 1
#define OFF 0

#define strobemode 2

volatile uint8_t buttonFlag = 0;
uint8_t startsequence = 1;

void delay(int ms)
{
	while (0 < ms)
	{
		_delay_ms(10);
		ms -= 10;
	}
}

void strobe(int which, int state)
{
	if (which == 1)
	{
		if (state == ON) OUT1_1;
		if (state == OFF) OUT1_0;
	}
	
	if (which == 2)
	{
		if (state == ON) OUT2_1;
		if (state == OFF) OUT2_0;
	}
	
	if (which == 0)
	{
		if (state == ON) OUTS_1;
		if (state == OFF) OUTS_0;
	}
}

void timestrobe(int which, int length)
{
	if (which == 1)
	{
		OUT1_1;
		OUTPB2_1;
		delay(length);
		OUT1_0;
		OUTPB2_0;
		delay(length);
	}
	
	if (which == 2)
	{
		OUT2_1;
		OUTPB2_1;
		delay(length);
		OUT2_0;
		OUTPB2_0;
		delay(length);
	}
	
	if (which == 0)
	{
		OUTS_1;
		OUTPB2_1;
		delay(length);
		OUTS_0;
		OUTPB2_0;
		delay(length);
	}
}

int checkbutton(void)
{
	if(buttonFlag)
	{
		while((PORTB.IN & PIN1_bm)==0);		// wait button released
		_delay_ms(100);                     // wait a bit for debounce purposes
		buttonFlag = 0;                     // reset flag
		PORTB.INTFLAGS |= PIN1_bm;		    // clear interrupt for sure
		return 1;                           // go away from this infinite loop
	}
	return 0;
}

void timestrobeloop(int which, int length, int loop)
{
	for (int x = 0; x < loop; x++) timestrobe(which,length);
}

void readEEPROMSettings(void)
{
	startsequence = eeprom_read_byte((uint8_t*)EEPROM_ADDR);
}

void saveEEPROMSettings(int which)
{
	eeprom_write_byte((uint8_t*)EEPROM_ADDR, which);
}


void selectstrobe(int which)
{
	if (strobemode == 2)
	{
		switch (which)
		{
			case 1:
			timestrobeloop(0,40,4);
			delay(1000);
			break;
			case 2:
			timestrobeloop(1,40,3);
			delay(80);
			timestrobe(2,40);
			timestrobe(1,40);
			timestrobe(2,40);
			timestrobe(1,40);
			delay(80);
			timestrobeloop(2,40,3);
			delay(80);
			break;
			case 3:
			timestrobeloop(1,40,3);
			delay(2500);
			timestrobeloop(2,40,3);
			delay(2500);
			break;
			case 4:
			timestrobeloop(1,40,5);
			delay(80);
			timestrobe(2,40);
			timestrobe(1,40);
			timestrobe(2,40);
			timestrobe(1,40);
			timestrobe(2,40);
			timestrobe(1,40);
			timestrobe(2,40);
			timestrobe(1,40);
			delay(80);
			timestrobeloop(2,40,5);
			delay(80);
			break;
			case 5:
			timestrobeloop(2,40,2);
			timestrobeloop(1,40,3);
			delay(1500);
			timestrobeloop(1,40,2);
			timestrobeloop(2,40,3);
			delay(1500);
			break;
			case 6:
			timestrobeloop(1,30,3);
			delay(100);
			timestrobeloop(1,50,5);
			delay(300);
			timestrobeloop(2,30,3);
			delay(100);
			timestrobeloop(2,50,5);
			delay(300);
			timestrobeloop(0,30,3);
			timestrobeloop(0,80,3);
			timestrobeloop(0,30,3);
			timestrobeloop(0,80,3);
			delay(300);
			break;
			case 7:
			timestrobeloop(1,30,4);
			delay(700);
			timestrobeloop(2,30,4);
			delay(700);
			break;
			case 8:
			timestrobeloop(0,40,8);
			delay(100);
			timestrobeloop(1,40,2);
			delay(100);
			timestrobeloop(2,40,2);
			delay(100);
			break;
			case 9:
			timestrobeloop(0,40,5);
			delay(175);
			break;
			case 10:
			timestrobeloop(1,40,6);
			timestrobeloop(2,40,6);
			delay(100);
			timestrobeloop(1,40,9);
			timestrobeloop(2,40,9);
			delay(150);
			break;
		}
	}
	
	if (strobemode == 1)
	{
		switch (which)
		{
			case 1:
			timestrobeloop(0,40,4);
			delay(1000);
			break;
			case 2:
			timestrobeloop(0,40,6);
			delay(150);
			break;
			case 3:
			timestrobeloop(0,40,6);
			delay(500);
			break;
			case 4:
			timestrobeloop(0,40,6);
			delay(2500);
			break;
			case 5:
			timestrobeloop(0,40,6);
			delay(5000);
			break;
			case 6:
			timestrobeloop(0,150,2);
			delay(150);
			break;
			case 7:
			timestrobeloop(0,150,2);
			delay(400);
			break;
			case 8:
			timestrobeloop(0,40,4);
			delay(100);
			timestrobeloop(0,80,2);
			delay(100);
			break;
			case 9:
			timestrobeloop(0,40,8);
			delay(100);
			timestrobeloop(0,40,2);
			delay(100);
			timestrobeloop(0,40,2);
			delay(100);
			break;
			case 10:
			timestrobeloop(0,40,5);
			delay(175);
			break;
		}
	}
	
	saveEEPROMSettings(which);
}

int main(void)
{
	PORTB.PIN1CTRL &= ~PORT_ISC_gm;			// enable external int on falling edge on PB1
	PORTB.PIN1CTRL |= PORT_ISC_FALLING_gc;
	
	PORTA.DIRSET = 0xF0;			// set PA4-7 as outputs
	PORTB.DIRSET = 0x04;
	
	sei();			// enable interrupts globally
	
	readEEPROMSettings();
	if(startsequence > 10)
	{
		startsequence = 1;
	}
	
	if (startsequence != 0)
	{
		while(1) 
		{ 
			selectstrobe(startsequence); 
			if (checkbutton()) break;
			
		}
	}

	uint8_t ws = ++startsequence;
	
	if(ws == 11)
	{
		ws = 1;
	}

	while (1)
	{
		selectstrobe(ws);
		if((checkbutton()))
		{
			ws++;
			if(ws == 11)
			{
				ws = 1;
			}
		}
	}
		
/*		
		if (strobemode == 2)
		{
			while(1) { selectstrobe(1); if (checkbutton()) break; }
			while(1) { selectstrobe(2); if (checkbutton()) break; }
			while(1) { selectstrobe(3); if (checkbutton()) break; }
			while(1) { selectstrobe(4); if (checkbutton()) break; }
			while(1) { selectstrobe(5);	if (checkbutton()) break; }
			while(1) { selectstrobe(6); if (checkbutton()) break; }
			while(1) { selectstrobe(7); if (checkbutton()) break; }
			while(1) { selectstrobe(8); if (checkbutton()) break; }
			while(1) { selectstrobe(9); if (checkbutton()) break; }
			while(1) { selectstrobe(10); if (checkbutton()) break; }
		}

		if (strobemode == 1)
		{
			while(1) { selectstrobe(1); if (checkbutton()) break; }
			while(1) { selectstrobe(2); if (checkbutton()) break; }
			while(1) { selectstrobe(3); if (checkbutton()) break; }
			while(1) { selectstrobe(4); if (checkbutton()) break; }
			while(1) { selectstrobe(5); if (checkbutton()) break; }
			while(1) { selectstrobe(6); if (checkbutton()) break; }
			while(1) { selectstrobe(7); if (checkbutton()) break; }
			while(1) { selectstrobe(8); if (checkbutton()) break; }
			while(1) { selectstrobe(9); if (checkbutton()) break; }
			while(1) { selectstrobe(10); if (checkbutton()) break; }
		}





	}
*/	
}


// button press interrupt handler
ISR(PORTB_PORT_vect)
{
	buttonFlag = 1;					 // set flag
	PORTB.INTFLAGS |= PIN1_bm;		 // clear interrupt
}
