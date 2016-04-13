#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


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
  }
}


volatile unsigned int adcVal;
volatile unsigned int sampledValue;
int i;
unsigned int prevVal;
ISR(ADC_vect)
{
  adcVal = ADCH; // only 8 bit value  is read.
  //sampledValue = adcVal/2;
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
  // laser on
  PORTA |= (1<<PA0);

  // TIMER1 PWM configurations
  // Phase Correct PWM, TOP = 0xFF
  TCCR1A |= 1<<WGM10;
  // CLK/8 pre-scalar for 2KHz motor frequency
  TCCR1B |= (1<<CS11);
  // register clear while in PWM mode
  TCCR1C = 0x00;

  // Phase correct pwm ON - non-inverting mode
  TCCR1A |= (1<<COM1A1)|(1<<COM1B1);

  // adc configurations - free running mode
  PRR &= ~(1<<PRADC);  // clear adc power-off bit
  ADCSRA |= 1<<ADEN;                //Power up the ADC
  ADCSRA |= 1<<ADIE;
  ADMUX = 0x01;          // setting MUX1
  ADCSRA |= ((1<<ADPS2));// | (1<<ADPS0));    //Prescaler at 64 so we have an 125Khz clock source
  ADCSRB = 1<<ADLAR;    // left adjust enabled
  ADCSRA |= (1<<ADATE);                //Signal source, in this case is the free-running
  sei();
  ADCSRA |= (1<<ADSC);                //Start converting


  while (1){
    for(i=0; i<100; i += 10)
    setSpeed(1, -i);
    setSpeed(2, adcVal);
    _delay_ms(20);
  }
  return 1;
}
