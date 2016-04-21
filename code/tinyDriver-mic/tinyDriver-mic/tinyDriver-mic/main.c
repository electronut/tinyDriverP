/*
 * tinyDriver-fingerSnap.c
 *
 * Created: 20-04-2016 12:33:37
 * Author : rishi
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned int adcValue;
/*ISR(ADC_vect)
{
    adcValue = ADC;
}*/
unsigned int count;
int main(void)
{
    // adc configurations
    PRR &= ~(1 << PRADC);
    ADCSRA |= 1 << ADEN;
    ADMUX = (1 << MUX0) | (1 << MUX2);        
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // CLK/64 , 125KHz
    sei();
    
    
    // set outputs
    DDRA |= (1<<PA0) | (1<<PA1) | (1<<PA2);

   ADCSRA |= 1<<ADSC;
  while(ADCSRA & (1<<ADSC));
  adcValue = ADC;
 
    // set to red
    PORTA = (1 << PA2);
    
    while (1) 
    {
       ADCSRA |= 1<<ADSC;
        while(ADCSRA & (1<<ADSC));
        adcValue = ADC;
        
        if(adcValue < 180) {
            
            PORTA &= ~(1 << PA2);
            PORTA = (1 << PA0);
            _delay_ms(1000);
        }
        else {
            PORTA &= ~(1 << PA0);
            PORTA = (1 << PA2);
        }
    }
}

