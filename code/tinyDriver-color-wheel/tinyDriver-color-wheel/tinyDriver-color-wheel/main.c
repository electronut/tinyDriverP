/*
 * tinyDriver-color-wheel.c
 *
 * Created: 21-04-2016 14:02:31
 * Author : rishi
 */ 
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

volatile signed int adcValue;

int main(void)
{
    // PA7 - mode pin for DRV8835
    // PA6 - OCR1A, Timer1 in PWM pin
    // PA3 - GPIO for motor
    
    DDRA |= (1<<PA7) | (1<<PA6) | (1<<PA3);
    
    // timer1 as pwm configurtations
    // phase correct PWM, top = 0xFF
    TCCR1A |= 1<<WGM10;
    // CLK/8 pre-scalar for 2KHz frequency
    TCCR1B |= 1<<CS11;
    // register clear while in PWM mode
    TCCR1C = 0x00;

    // Phase correct pwm ON - non-inverting mode
    TCCR1A |= (1<<COM1A1);

    // adc configuration
    // adc configurations
    PRR &= ~(1 << PRADC);
    ADCSRA |= 1 << ADEN;
    ADMUX = (1 << MUX0) | (1 << MUX2);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // CLK/64 , 125KHz
    sei();
    
    ADCSRA |= 1<<ADSC;
    while(ADCSRA & (1<<ADSC));
    adcValue = ADC;
    
    // mode pin HIGH
    PORTA |= (1<<PA7);
    OCR1A = 50;
    _delay_ms(500);
    while (1) 
    {
        ADCSRA |= 1<<ADSC;
        while(ADCSRA & (1<<ADSC));
        adcValue = ADC;
        
        if(adcValue < 180){
            OCR1A = 60;
            _delay_ms(2000);
        }
    }
}

