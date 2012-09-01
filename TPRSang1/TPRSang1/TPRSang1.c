/*
 * TPRSang1.c
 *
 * The ACTUAL program (not finished) we will be using, for now
 * Licensed under CC-By Sa
 *
 * Created: 21/08/2012 21:16:04
 *  Author: George Ainscough
 */ 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CPU 16000000L
#define HK_INTERVAL 8
#define HEAD_THRESHOLD 50
#define HEAD_CORRECT 10
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t dir = 5;
uint16_t head;
uint16_t starthead;

uint8_t hk_count;
uint8_t hk_flag;

//void setup(void);
//void loop(void);
void smallwait(void);

void error(void);
void GetDir(void);
void GetHead(void);
void HouseKeep(void);

void N_go(void);
void NE_go(void);
void E_go(void);
void SE_go(void);
void S_go(void);
void SW_go(void);
void W_go(void);
void NW_go(void);
void L_trn(void);
void R_trn(void);
void halt(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	DDRA = 0xFF; //GPIO Init
	DDRB = 0x01;
	DDRD = 0xF8;
	
	TWSR = 0x00; //TWI Init
	TWBR = 0x0C;
	TWCR = (1<<TWEN);
	
	TCCR0B |= ((1 << CS00) | (1 << CS01)); //TCC0 Init
	
	TCCR1B |= (1 << WGM12); //TCC1 Init
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 0x3D08;
	TCCR1B |= ((1 << CS10) | (1 << CS12));
	
	cli(); //WDT Setup
	wdt_reset();
	WDTCSR = (1<<WDCE)|(1<<WDE);
	WDTCSR |= 0x1F;
	sei();
	wdt_reset();
	
	GetDir(); //First Find Ball
	
	if (MCUSR & (1 << BORF)) //BORF Rep
	error();
	
    if (MCUSR & (1 << WDRF)) {} //WDR Detect
    else while ((PIND & 0x04) == 0) //Start Button
	{wdt_reset();}
		
	hk_flag = 0;
	hk_count = 0;
	
    while(1)
    {
        if (hk_flag) {
        HouseKeep(); }
		
        wdt_reset();
        
        switch (dir) {
	        case 1:
	        halt();
	        S_go();
	        while (dir == 1) {
		        GetDir();
	        }
	        break;
	        case 2:
	        halt();
	        SW_go();
	        while (dir == 2) {
		        GetDir();
	        }
	        break;
	        case 3:
	        halt();
	        W_go();
	        while (dir ==3) {
		        GetDir();
	        }
	        break;
	        case 4:
	        halt();
	        NW_go();
	        while (dir == 4) {
		        GetDir();
	        }
	        break;
	        case 5:
	        halt();
	        N_go();
	        while (dir == 5) {
		        GetDir();
	        }
	        break;
	        case 6:
	        halt();
	        NE_go();
	        while (dir == 6) {
		        GetDir();
	        }
	        break;
	        case 7:
	        halt();
	        E_go();
	        while (dir == 7) {
		        GetDir();
	        }
	        break;
	        case 8:
	        halt();
	        SE_go();
	        while (dir == 8) {
		        GetDir();
	        }
	        break;
	        case 9:
	        halt();
	        S_go();
	        while (dir == 9) {
		        GetDir();
	        }
	        break;
	        default:
	        GetDir();
	        break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ISR(TIMER1_COMPA_vect)
{
	hk_count++;
	if (hk_count == HK_INTERVAL)
	{
		hk_count = 0;
		hk_flag = 1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void GetDir()
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Start
	while ((TWCR & (1<<TWINT)) == 0);
	if ((TWSR & 0xF8) != 0x08) //Val
	error();
	
	TWDR = (0x10 >> 7); //Addr
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	if ((TWSR & 0xF8) != 0x18) //Val
	error();
	
	TWDR = 0x49; //Job -- TODO
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	if ((TWSR & 0xF8) != 0x28) //Val
	error();
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Start
	while ((TWCR & (1<<TWINT)) == 0);
	if ((TWSR & 0xF8) != 0x10) //Val
	error();
	
	TWDR = ((0x10 >> 7) | 1); //Addr Read
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	if ((TWSR & 0xF8) != 0x40) //Val
	error();
	
	TWCR = (1<<TWINT)|(1<<TWEN); //Read Dir
	while ((TWCR & (1<<TWINT)) == 0);
	dir = TWDR;
	if ((TWSR & 0xF8) != 0x58) //Val
	error();
}

void GetHead()
TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Start
while ((TWCR & (1<<TWINT)) == 0);
if ((TWSR & 0xF8) != 0x08) //Val
error();

TWDR = (0x42 >> 7); //Addr
TWCR = (1<<TWINT)|(1<<TWEN);
while ((TWCR & (1<<TWINT)) == 0);
if ((TWSR & 0xF8) != 0x18) //Val
error();

TWDR = "A"; //Job -- TODO
TWCR = (1<<TWINT)|(1<<TWEN);
while ((TWCR & (1<<TWINT)) == 0);
if ((TWSR & 0xF8) != 0x28) //Val
error();

TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Start
while ((TWCR & (1<<TWINT)) == 0);
if ((TWSR & 0xF8) != 0x10) //Val
error();

TWDR = ((0x10 >> 7) | 1); //Addr Read
TWCR = (1<<TWINT)|(1<<TWEN);
while ((TWCR & (1<<TWINT)) == 0);
if ((TWSR & 0xF8) != 0x40) //Val
error();

TWCR = (1<<TWINT)|(1<<TWEN); //Read Dir
while ((TWCR & (1<<TWINT)) == 0);
dir = TWDR;
if ((TWSR & 0xF8) != 0x58) //Val
error();
}

void HouseKeep()
{
	wdt_reset();
	GetHead();
	if(head <= starthead - HEAD_THRESHOLD || head >= starthead + HEAD_THRESHOLD)
	{
		if(head <= starthead - HEAD_THRESHOLD)
		{
			wdt_reset();
			halt();
			R_trn();
			while (head <= starthead - HEAD_CORRECT)
			{
				GetHead();
			}
			halt();
		}
		else if(head >= starthead + HEAD_THRESHOLD)
		{
			wdt_reset();
			halt();
			L_trn();
			while (head >= starthead + HEAD_CORRECT)
			{
				GetHead();
			}
			halt();
		}
	}
	
	hk_flag = 0;
	hk_count = 0;
}


void error()
{
	PORTB |= 0x01; //Notify, then wait for watchdog
	for(;;){}
}

void smallwait()
{
	TCNT0 = 0x00;
	while (TCNT0 >= 0x5E){}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void N_go()
{
	PORTD |= 0xF0;
	PORTA = 0x69;
}
void NE_go()
{
	PORTD |= 0xF0;
	PORTA = 0x21;
}
void E_go()
{
	PORTD |= 0xF0;
	PORTA = 0xA5;
}
void SE_go()
{
	PORTD |= 0xF0;
	PORTA = 0x84;
}
void S_go()
{
	PORTD |= 0xF0;
	PORTA = 0x96;
}
void SW_go()
{
	PORTD |= 0xF0;
	PORTA = 0x12;
}
void W_go()
{
	PORTD |= 0xF0;
	PORTA = 0x5A;
}
void NW_go()
{
	PORTD |= 0xF0;
	PORTA = 0x48;
}

void R_trn()
{
	PORTD |= 0xF0;
	PORTA = 0x55;
}
void L_trn()
{
	PORTD |= 0xF0;
	PORTA = 0xAA;
}

void halt()
{
	PORTD |= 0xF0;
	PORTA = 0xFF;
	smallwait();
	PORTA = 0x00;
	PORTD &= 0x0F;
}