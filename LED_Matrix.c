#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

//#include "timer_tools.h"
#include "printf_tools.h"

//#include <stdio.h>
//#include <stdlib.h>

#define	baud 57600  // baud rate
// Caution: erroneous result if F_CPU is not correctly defined
#define baudgen ((F_CPU/(16*baud))-1)  //baud divider


volatile uint16_t milisegundos=0, milisegundos2=0;


void init_USART(void)
{
  UBRR0 = baudgen;
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);		// enable read e transmit
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);		// 1 stop bit e 8 data bits
}

ISR(TIMER0_COMPA_vect)
{
milisegundos+=1;
TCNT0=0;
}


ISR(TIMER2_COMPB_vect)
{
milisegundos2+=1;
TCNT2=0;
}


void escreve(uint8_t vector[])
{

	PORTC &= ~((1<<2)|(1<<1));		//coloca latch em 0 e SHCP em 0


for (uint8_t n=0; n<7; n++)
	{
	if (vector[n]==0)
		PORTC &= ~(1<<0);

	else if (vector[n]==1)
		PORTC |= (1<<0);

	PORTC |= (1<<1);					// envia um impulso
	PORTC &= ~(1<<1);					// para PC1	(SHCP shift register clock input)
	}

PORTC |= (1<<2);			//	envia um impulso
PORTC &= ~(1<<2);			//	para PC2  (STCP storage register clock input)

}



void escreve_0(void)
{

eeprom_write_byte(0,0);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;
		
		escreve_1();		
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_9();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2) || (i==3) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2) || (i==3) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}




void escreve_1(void)
{

eeprom_write_byte(0,1);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;

while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);
	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;
		escreve_2();
		}
	
	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;
		escreve_0();
		}
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)			// escreve sete 1's no vector
			vector[i]=1;

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)			// escreve sete 1's no vector
			{
			if ((i==1) || (i==6))
				vector[i]=0;
			else
				vector[i]=1;
			}
		
		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)	
			vector[i]=0;

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)	
			{
			if (i==6)
				vector[i]=0;
			else
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{


		for (uint8_t i=0; i<7; i++)	
			vector[i]=1;

		coluna=1;
		PORTB = 0b00011110;	
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}


void escreve_2(void)
{

eeprom_write_byte(0,2);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla=0 se PD7=0 tecla!=0 se PD2=1

	tecla2 = PIND & (1<<6);		// tecla2=0 se PD6=0 e tecla2_antes!=0 se PD6=1

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla passa para o 3
		{
		acabar=1;

		escreve_3_animado();
		}
	
	if ((tecla2==0) && (tecla2_antes!=0))	
		{
		desce=1;

		escreve_1();
		}
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==6))
				vector[i]=0;
			else
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==5) || (i==6))
				vector[i]=0;
			else
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==4) || (i==6))
				vector[i]=0;
			else
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1)||(i==2)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}



void escreve_3(void)
{

eeprom_write_byte(0,3);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;

//printf("entrou em escreve_3() \n");

while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);
	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_4();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;
		escreve_2();
		}
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==2)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==1) || (i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}


void escreve_4(void)
{

eeprom_write_byte(0,4);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_5();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_3();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==3) ||(i==4))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==2) || (i==4))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1)||(i==4))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			vector[i]=0;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if (i==4)
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}



void escreve_5(void)
{

eeprom_write_byte(0,5);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_6();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_4();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) ||(i==1) || (i==2) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}


void escreve_6(void)
{

eeprom_write_byte(0,6);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_7();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_5();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==2) || (i==3) || (i==4) || (i==5))
				vector[i]=0;
			else
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3)||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}


void escreve_7(void)
{

eeprom_write_byte(0,7);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_8();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_6();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if (i==0)
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if (i==0)
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==3) || (i==4) || (i==5) ||(i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0)||(i==2))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==1))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}



void escreve_8(void)
{

eeprom_write_byte(0,8);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;

		escreve_9();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_7();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2) || (i==4) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}

void escreve_9(void)
{

eeprom_write_byte(0,9);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;


while((acabar==0) && (desce==0))
	{
	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	
	if ((tecla==0) && (tecla_antes!=0))		// se houver rising edge da tecla acaba
		{
		acabar=1;
		
		escreve_0();		
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		escreve_8();
		}	
	
	
	if ((coluna==1) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=2;
		milisegundos=0;
		PORTB = 0b00001111;					// liga a coluna 1 da matriz
		escreve(vector);
		}

	else if ((coluna==2) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}


		coluna=3;
		PORTB = 0b00010111;					// liga a coluna 2 da matriz	
		milisegundos=0;
		escreve(vector);
		}
	
	else if ((coluna==3) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==6))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=4;
		PORTB = 0b00011011;					// liga a coluna 3 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==4) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==0) || (i==3) || (i==5))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=5;
		PORTB = 0b00011101;					// liga a coluna 4 da matriz	
		milisegundos=0;
		escreve(vector);
		}

	else if ((coluna==5) && (milisegundos>=5))
		{
		for (uint8_t i=0; i<7; i++)
			{
			if ((i==1) || (i==2) || (i==3) || (i==4))
				vector[i]=0;
			else 
				vector[i]=1;
			}

		coluna=1;
		PORTB = 0b00011110;					// liga a coluna 5 da matriz
		milisegundos=0;
		escreve(vector);
		}
	


	tecla_antes = tecla;

	tecla2_antes = tecla2;

	}


}



void escreve_3_animado(void)
{

eeprom_write_byte(0,3);

uint8_t coluna=1, acabar=0, tecla=1, tecla_antes=0, vector[7]={1,1,1,1,1,1,1};

uint8_t desce=0, tecla2=1, tecla2_antes=0;

uint8_t transicao=1;

milisegundos=0;


TCNT2 = 0;
milisegundos2=0;
TIMSK2 |= (1<<OCIE2B);		//enable as interrupcoes




milisegundos2=0;


//printf("entrou em escreve_3_animado() \n");


while((acabar==0) && (desce==0))
	{

	tecla = PIND & (1<<7);		//tecla_antes=0 se PD7=0 tecla_antes!=0 se PD2=1

	tecla2 = PIND & (1<<6);

	//printf("milisegundos2: %d \n", milisegundos2);	



	if (milisegundos2>=500)
		{
		if (transicao<5)
			{

			//printf("entrou em transicao+=1 \n");

			milisegundos2=0;
			TCNT2=0;
			transicao+=1;
			}
		
		if (transicao>=5)
			{
			milisegundos2=0;
			TCNT2=0;
			TIMSK2 &= ~(1<<OCIE2B);
			
			acabar=1;

			//printf("entrou em milisegundos2>=500 e transicao>=5 \n");

			escreve_3();
			}
		
		}



	if ((tecla==0) && (tecla_antes!=0))
		{
		acabar=1;
		
		TIMSK2 &= ~(1<<OCIE2B);		//desativa interrupçoes

		escreve_4();
		}

	if ((tecla2==0) && (tecla2_antes!=0))
		{
		desce=1;

		TIMSK2 &= ~(1<<OCIE2B);		//desativa interrupçoes

		escreve_2();
		}
	

	

	
	if ((coluna==1) && (milisegundos==5))
		{
		switch(transicao)
		{
		case(1):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==4) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=2;
			milisegundos=0;
			PORTB = 0b00001111;
			escreve(vector);
			break;
			}
		case(2):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==1) || (i==3) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}

		

			coluna=2;
			milisegundos=0;
			PORTB = 0b00001111;
			escreve(vector);
			break;
			}
		case(3):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==2) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=2;
			milisegundos=0;
			PORTB = 0b00001111;
			escreve(vector);
			break;
			}
		case(4):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=2;
			milisegundos=0;
			PORTB = 0b00001111;
			escreve(vector);
			break;
			}
		case(5):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=2;
			milisegundos=0;
			PORTB = 0b00001111;
			escreve(vector);
			break;
			}
		}	
		}
	
	else if ((coluna==2) && (milisegundos==5))
		{
		switch(transicao)
		{
		case(1):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=3;
			milisegundos=0;
			PORTB = 0b00010111;
			escreve(vector);
			break;
			}
		case(2):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==4) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=3;
			milisegundos=0;
			PORTB = 0b00010111;
			escreve(vector);
			break;
			}
		case(3):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==1) || (i==3) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=3;
			milisegundos=0;
			PORTB = 0b00010111;
			escreve(vector);
			break;
			}
		case(4):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==2) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=3;
			milisegundos=0;
			PORTB = 0b00010111;
			escreve(vector);
			break;
			}
		case(5):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=3;
			milisegundos=0;
			PORTB = 0b00010111;
			escreve(vector);
			break;
			}
		}		
		}
	
	else if ((coluna==3) && (milisegundos==5))
		{
		switch(transicao)
		{
		case(1):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=4;
			milisegundos=0;
			PORTB = 0b00011011;
			escreve(vector);
			break;
			}
		case(2):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=4;
			milisegundos=0;
			PORTB = 0b00011011;
			escreve(vector);
			break;
			}
		case(3):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==4) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=4;
			milisegundos=0;
			PORTB = 0b00011011;
			escreve(vector);
			break;
			}
		case(4):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==1) || (i==3) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=4;
			milisegundos=0;
			PORTB = 0b00011011;
			escreve(vector);
			break;
			}
		case(5):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==2) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=4;
			milisegundos=0;
			PORTB = 0b00011011;
			escreve(vector);
			break;
			}
		}		
		}
	
	else if ((coluna==4) && (milisegundos==5))
		{
		switch(transicao)
		{
		case(1):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=5;
			milisegundos=0;
			PORTB = 0b00011101;
			escreve(vector);
			break;
			}
		case(2):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=5;
			milisegundos=0;
			PORTB = 0b00011101;
			escreve(vector);
			break;
			}
		case(3):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=5;
			milisegundos=0;
			PORTB = 0b00011101;
			escreve(vector);
			break;
			}
		case(4):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==4) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=5;
			milisegundos=0;
			PORTB = 0b00011101;
			escreve(vector);
			break;
			}
		case(5):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==1) || (i==3) || (i==6))
					vector[i]=0;
				else
					vector[i]=1;
				}


			coluna=5;
			milisegundos=0;
			PORTB = 0b00011101;
			escreve(vector);
			break;
			}
		}		
		}

	else if ((coluna==5) && (milisegundos==5))
		{
		switch(transicao)
		{
		case(1):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=1;
			milisegundos=0;
			PORTB = 0b00011110;
			escreve(vector);
			break;
			}
		case(2):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=1;
			milisegundos=0;
			PORTB = 0b00011110;
			escreve(vector);
			break;
			}
		case(3):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=1;
			milisegundos=0;
			PORTB = 0b00011110;
			escreve(vector);
			break;
			}
		case(4):
			{
			for (uint8_t i=0; i<7; i++)
				{
				vector[i]=1;
				}


			coluna=1;
			milisegundos=0;
			PORTB = 0b00011110;
			escreve(vector);
			break;
			}
		case(5):
			{
			for (uint8_t i=0; i<7; i++)
				{
				if ((i==0) || (i==4) || (i==5))
					vector[i]=0;
				else
					vector[i]=1;
				}

			milisegundos=0;
			PORTB = 0b00011110;
			escreve(vector);

			// Desativa interrupçoes de timer2
			TIMSK2 &= ~(1<<OCIE2B);
			TCNT2=0;
			milisegundos2=0;

			//escreve 3 normal
			escreve_3();
			break;
			}
		}		
		}
	
	

	
	
	tecla_antes=tecla;
	
	tecla2_antes=tecla2;	
	}



}







int main(void)
{

	uint8_t mostrador=0, mostrador_antes=0;	
  
  init_USART();  
  sei();
  init_printf_tools();

  printf(" --------------   MATRIZ LEDS  ---------------------------\n");

// Configurar PINOS do Arduino

DDRC |= 0b00000111;				// configurar PC0 a PC2 como saidas

DDRD &= 0b00111111;				// configurar PD7 e PD6 como entrada


DDRB |= 0b00011111;			//configurar PB0 a PB4 como saidas para controlar transistores



uint8_t vector[7] = {1,1,1,1,1,1,1};		// coloca todas as linhas a 1
escreve(vector);

PORTB = 0b00011111;							// desliga todos os transistores



// Configurar o TIMER0			16MHZ, prescaler 64, a cada 250ticks passam 1ms


	TCNT0=0;

  
  TCCR0A |= ((1<<COM0A1)|(1<<COM0A0));			// ativar OC0A - Set on Compare
  TCCR0A &= ~((1<<WGM00)|(1<<WGM01));			// colocar WGM00 e WGM01 a 0

  TCCR0B &= ~(1<<WGM02);						//colocar WGM02 a 0
  TCCR0B |= ((1<<CS00)|(1<<CS01));				//colocar CS00 e CS01 a 1
  TCCR0B &= ~(1<<CS02);							//colocar CS02 a 0

  OCR0A = 250;

  TIMSK0 &= ~(1<<OCIE0A);					//desativar interrupcoes ao comparar com OC0A




// Configurar o TIMER2			16MHZ, prescaler 64, a cada 250 ticks passam 1ms
	

	TCCR2A &= ~((1<<COM2A1) | (1<<COM2A0));
	
	TCCR2A |= ((1<<COM2B1)|(1<<COM2B0));

	TCCR2A &= ~((1<<WGM20) | (1<<WGM21));

	TCCR2B &= ~((1<<WGM22)|(1<<CS21)|(1<<CS20));
	TCCR2B |= (1<<CS22);
	
	OCR2B = 250;
	
	TIMSK2 &= ~((1<<OCIE2A) | (1<<OCIE2B));		//desativa as interrupcoes

	TCNT2 = 0;



// procedimento para limpar a eeprom

 uint8_t limpa = PIND & (1<<6);				// pressionar Botao de PD6 e Reset ao mesmo tempo
											// 			para limpar a EEPROM
 if (limpa == 0)
 	{
	eeprom_write_byte(0,0);
	}


  while (1) 
  	{
		mostrador = PIND & (1<<7);		// mostrador fica 0 se PD7=0 ou fica !=0 se PD2=1
		
		if ((mostrador_antes!=0) && (mostrador ==0))
			{
			TIMSK0 |= (1<<OCIE0A);		// ativa interruçoes
			TCNT0 = 0;
			
			uint8_t memoria = eeprom_read_byte(0);
			
			if (memoria==0)
				escreve_0();
			
			else if (memoria==1)
				escreve_1();
			
			else if (memoria==2)
				escreve_2();
			
			else if (memoria==3)
				escreve_3();
			
			else if (memoria==4)
				escreve_4();
			
			else if (memoria==5)
				escreve_5();

			else if (memoria==6)
				escreve_6();
			
			else if (memoria==7)
				escreve_7();

			else if (memoria==8)
				escreve_8();

			else if (memoria==9)
				escreve_9();
			}
			
		mostrador_antes = mostrador;


		//printf("mostrador: %d \n", mostrador);

	}



}

