/*
2wdbot - HCSR04 (Ultrasonic Sensor), DRV8835 (Motor Driver), RGB Led
HCSR04 uses timer0 with interrupt PCINT0.
DRV8835 uses timer1 in Phase-Correct PWM mode.
Initially, 2wdbot moves forward. On obstacle detection within a range of 10 cms,
2wdbot stops, reverses and takes a left turn.
author : electronut Labs
email  : electronut.in
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/// function to set motor speed.
/// speed can either be a positive or negative integer. Positive value signifies forward direction while negative value signifies reverse direction.
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

/// function to stop motors.
void stopMotors()
{
    OCR1A = OCR1B = 0;
}

/// HCSR04 parameters.
int pinEcho = 0;
volatile char echoDone = 0;
uint32_t countTimer0 = 0;

/// begin HCSR04
float getDistance()
{
    float distance = 0.0;
	
    cli();
	
    // interrupt enable
    GIMSK |= 1<<PCIE1;
    // enable pin
    PCMSK1 |= 1<<PCINT10;
	
    sei();
	
    // reset echo flag
    echoDone = 0;
    // reset counter
    countTimer0 = 0;
    
    // send 10us trigger pulse
	
    PORTB &= ~(1<<PB0);
    _delay_ms(20);
    PORTB |= (1<<PB0);
    _delay_ms(12);
    PORTB &= ~(1<<PB0);
    _delay_ms(20);
    
    // listen for echo
    while(!echoDone);
	
    // disable interrupt
    GIMSK &= ~(1<<PCIE1);
    // disable pin
    PCMSK1 &= ~(1<<PCINT10);

    // calculate duration
    float duration = countTimer0/8000000.0;
	
    // distance = duration * speed of sound * 0.5;
    // distance(in cms) = duration * 340.26 * 0.5 * 100
    // distance = 17013 * duration
    distance = 17013.0 * duration;
	
    return distance;
}

/// timer overflow handler
ISR(TIM0_OVF_vect)
{
    countTimer0 += 256;
}


/// pin-change interrupt handler
ISR(PCINT1_vect)
{
    // read PCINT10
    if (PINB & (1<<PB2)){
        //start 16 bit timer
        // Divide by 1 - prescalar
        TCCR0B |= 1<<CS00;
        // set interrupt overflow flag
        TIMSK0 |= 1<<TOIE0;
    }    
    else{
        // stop timer
        TCCR0B &= ~(1<<CS00);
        // calculate time elapsed
        countTimer0 += TCNT0;
        // reset counter - TCNT0
        TCNT0 = 0;
        // set flag
        echoDone = 1;
    }    
}


unsigned char dir = 1;

int main(void)
{
    // pin set-up
	
    // PB0 - trigger
    // PB2 - echo (input)
	DDRB |= (1<<PB0);
	
    // PA0, PA1, PA2 = R,G,B leads of the LED
    // PA3 = GPIO for motor 1, PA4 = GPIO for motor 2
    // PA6, PA5 = OC1A, OC1B - PWM
    // PA7 = MODE pin HIGH for PWM mode in DRV8835
	DDRA |= (1<<PA7) | (1<<PA6) | (1<<PA5) | (1<<PA4) | (1<<PA3) | (1<<PA2) | (1<<PA1) | (1<<PA0);

    // TIMER1 PWM configurations
    // Phase Correct PWM, TOP = 0xFF
    TCCR1A |= 1<<WGM10;
    // CLK/8 pre-scalar for 2KHz motor frequency
    TCCR1B |= 1<<CS11;
    // register clear while in PWM mode
    TCCR1C = 0x00;
	
    // Phase correct pwm ON - non-inverting mode
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1);
	
    
    float prevDist = 0.0;
    while(1){
        float dist = getDistance();
		
        if(dist>400){
            dist = prevDist;
        }
		
        prevDist = dist;
		
        if(dist >= 10){
            if(!dir){
                // forward
                // led = green
                PORTA |= 1<<PA2;
                setSpeed(1, 150);
                setSpeed(2, 150);
                dir = 1;		
                }
            _delay_ms(100);
        }
        else{
            PORTA &= ~(1<<PA2);
            // stop
            // led = red    
            PORTA |= 1<<PA0;
            stopMotors();
            _delay_ms(1000);
            PORTA &= ~(1<<PA0);	
            // reverse
            // led = blue
            PORTA |= 1<<PA1;
            setSpeed(1, -150);
            setSpeed(2, -150);
            _delay_ms(2000);
            PORTA &= ~(1<<PA1);
            // left
            // led = purple
            PORTA |= ((1<<PA0) | (1<<PA1));
            setSpeed(1, -150);
            setSpeed(2, 150);
            _delay_ms(1000);
            PORTA &= ~((1<<PA0) | (1<<PA1));
        /*    
            // right
            // led = yellow
            PORTA |= ((1<<PA2) | (1<<PA1));
            setSpeed(1, 150);
            setSpeed(2, -150);
            _delay_ms(1000);
            PORTA &= ~((1<<PA2) | (1<<PA1));
        */	
            dir = 0;
        }
        _delay_us(100);
    }
    return 1;
}
