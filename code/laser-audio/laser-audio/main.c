/*
laser-audio - LM386 (Audio Amplifier), 3.5mm audio pin, laser diode, mirrors

The set-up samples incoming audio stream collected from an audio source using 3.5mm audio pin.
The rms value of multiple adc samples is calculated and acts as PWM duty cycle for first motor
resulting in varying motor speed while the second motor runs at a constant speed.
This combination of varying motor speeds along with the reflected light beam from the 
connected laser imparts beautiful patterns corresponding to the music.

author : electronut Labs
email  : electronut.in
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

// function to control motor speed and direction.
void setSpeed(int motor, int speed)
{
  // DDRV8835 - enable MODE pin for PWM operation
  if (!(PORTA & (1<<PA7))){
    PORTA |= (1<<PA7);
  }

  // left motor
  if(motor == 1){
    // forward
    if(speed > 0){
      PORTA |= (1<<PA3);
      OCR1A = speed;
    }
    // reverse
    else if(speed < 0){
      speed = -speed;
      PORTA &= ~(1<<PA3);
      OCR1A = speed;
    }
    // stop motor
    else if(speed == 0){
        OCR1A = 0;
    }    
  }
  // right motor
  else if(motor == 2){
    // forward
    if(speed > 0){
      PORTA |= (1<<PA4);
      OCR1B = speed;
    }
    // reverse
    else if(speed < 0){
      speed = -speed;
      PORTA &= ~(1<<PA4);
      OCR1B = speed;
    }
    // stop motor
    else if(speed == 0){
        OCR1B = 0;
    }
  }
}

volatile unsigned int adcVal;

ISR(ADC_vect)
{
  adcVal = ADC;
}

float adcMovingrms()
{
    static unsigned char haveMinimum = 0;
    unsigned int currentSample=0, previousSample=0;
    float rms = 0;
    if(!(haveMinimum)){
        for(int i=0; i<5; i++){
            currentSample = adcVal;
            rms += (currentSample * currentSample);
        }
        haveMinimum = 1;
    }
    else{
        currentSample = adcVal;
        rms -= (previousSample * previousSample);
        rms += (currentSample * currentSample);             
    }    
    previousSample = adcVal;
    rms /= 10;
    rms = sqrt(rms); 
    
    return rms;   
}
int main(void)
{
    // pin set-up
    
    // PA0 = laser diode.
    // PA1 = audio input
    // PA3 = GPIO for motor 1, PA4 = GPIO for motor 2
    // PA6, PA5 = OC1A, OC1B - PWM
    // PA7 = MODE pin HIGH for PWM mode in DRV8835
    DDRA |= (1<<PA7) | (1<<PA6) | (1<<PA5) | (1<<PA4) | (1<<PA3) | (1<<PA0);

    // TIMER1 PWM configurations
    // Phase Correct PWM, TOP = 0xFF
    TCCR1A |= 1<<WGM10;
    // CLK/64 pre-scalar for 125KHz motor frequency
    TCCR1B |= (1<<CS11);
    // register clear while in PWM mode
    TCCR1C = 0x00;

    // Phase correct pwm ON - non-inverting mode
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1);

    // adc configurations - free running mode
    PRR &= ~(1<<PRADC);  // clear adc power-off bit
    ADCSRA |= 1<<ADEN;                //Power up the ADC
    ADCSRA |= 1<<ADIE;
    ADMUX = (1<<MUX0);          // setting PA1 as ADC input
    ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));    //Prescaler at 128 so we have an 62.5Khz clock source
    ADCSRA |= (1<<ADATE);                //Signal source, in this case is the free-running
    sei();
    ADCSRA |= (1<<ADSC);                //Start converting
    
    // laser ON
    PORTA |= (1<<PA0);
    // motor ON at constant speed
    setSpeed(2, 15);
    
    while (1){
        float sampledValue = adcMovingrms();
        if(sampledValue == 0){
            setSpeed(1,0);
        }
        else{
            setSpeed(1, sampledValue/3.5);
        } 
        _delay_ms(200);
    }
    return 1;
}     