/*
 * program sterownika wykonawczego
 * ALL RIGHT RESERVED ©
 *
 * ATMEGA8A, 8MHZ
 */
#define F_CPU 8000000UL   
#define BAUDRATE 9600        

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//# Deklaracja funkcji USART

void usart_begin(void)
{
	UBRRH = 0x00;                                 // Inicjalizacja wartości UBRR dla baudrate
	UBRRL = 0x33;                                 // Ustawienie Baudrate na 9600 - zgodnie z datasheet 51
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);   // Włączenie wysyłania i odbierania
	UCSRC = (1<<URSEL) | (1<<USBS) | (1 << UCSZ0) | (1 << UCSZ1);  // 8 bitów danych, 2 bity stopu
}
char usart_rxd(void)
{
	while (!(UCSRA & (1<<RXC)));        // Czekaj na flagę odebranych danych
	return(UDR);                        // Zwróć dane z UDR
}

void usart_txd(char x)
{
	UDR = x;				// Przenieś dane do rejestru UDR
	while (!(UCSRA & (1<<UDRE)));		// Sprawdź czy UDR jest puste
}

void usart_string(char *c)
{
	while (*c != 0)                     // Sprawdź null bajta
	usart_txd(*c++);                    // inkrementuj wskaźnik
}


//#-----------------------------------------Program Główny-----------------------------------------//


int main(void)
{

//# konfiguracja PWM 

	DDRB = (1<<PB1) | (1<<PB2);		// ustaw PB1,PB2 jako wyjścia

	TCCR1A |= 1<<WGM11 | 1<<COM1A1 | 1<<COM1B1;	 // 16BIT FAST PWM, NON INVERTING MODE	
	TCCR1B |= 1<<WGM13 | 1<<WGM12 | 1<<CS11;     // PRESCALER 8
	ICR1 = 19999;			// ustaw okres 20ms

	OCR1A = 2000;				//wypełnienie 2ms
	OCR1B = 2000;


		usart_begin();           // inicjalizacja USART

//# Zmienne do przetwarzania przychodzącego string na inty
		
		int x,y;				

		char xbufor[8];
		char ybufor[8];
		char *w;
		char ch;
		char gotowe=0;
		int i=0;
		

	while (1)
	{
		
		gotowe=0;
		i=0;
		
		w=xbufor; 

		 do
		 {
			
			ch=usart_rxd();			// oczekiwanie na char

				   switch(ch)
				   {
					   case '.':	//jeśli kropka to xbufor jest uzupełniony
						{
							   *w=0;	// ostatni to null bajt
					   			w=ybufor; // zmien wskaźnik na ybufor
					   			i=0;
								break;
						}
					   case '!':  // jeśli wykrzyknik to ybufor jest uzupełniony
						{
								*w=0;	// ostatni to null bajt
								gotowe=1;
								break;
						}
					   default:
					   {		if(i<8)
								{
								 *w++=ch;
								 ++i;
								}
								break;
					   }					   
				   }		
		} while (!gotowe);


		x=atoi(xbufor);
		y=atoi(ybufor);
	
	
		OCR1A = x;				// wysterowanie PWM
		OCR1B = y;
				
    }
return 0;     
}
