/*
 * Example code for interfacing stepper motor(28BYJ-48) with tinyDriver using Half-Step Sequence.
 * Step angle : 360/64 = 5.625 i.e. angle per step = 5.625.
 * As Half-Step sequence is used, step angle = 5.625/2 = 2.8125
 
 * Author : electronut Labs
 */ 
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

// function computes the step limit for desired angle  
int angle(int angle)
{
    return (2.8125 * angle);
}

// clock-wise rotation
// Step Sequence : A-AB-B-BC-C-CD-D-DA
void runClockWise(int rotation)
{
    int i;
    for(i=0; i<angle(rotation); i++){
        PORTA= 0x08;
        _delay_us(1200);
        PORTA = 0x18;
        _delay_us(1200);
        PORTA = 0x10;
        _delay_us(1200);
        PORTA = 0x30;
        _delay_us(1200);
        PORTA = 0x20;
        _delay_us(1200);
        PORTA = 0x60;
        _delay_us(1200);
        PORTA = 0x40;
        _delay_us(1200);
        PORTA = 0x48;
        _delay_us(1200);
    }
}

// counter-clockwise rotation
// Step sequence : DA-D-CD-C-BC-B-AB-A
void runCounterClockWise(int rotation)
{
    int j;
    for(j=0; j<angle(rotation); j++){
        PORTA= 0x48;
        _delay_us(1200);
        PORTA = 0x40;
        _delay_us(1200);
        PORTA = 0x60;
        _delay_us(1200);
        PORTA = 0x20;
        _delay_us(1200);
        PORTA = 0x30;
        _delay_us(1200);
        PORTA = 0x10;
        _delay_us(1200);
        PORTA = 0x18;
        _delay_us(1200);
        PORTA = 0x08;
        _delay_us(1200);
    }
}

// function to disable output pins
void disablePins()
{
    PORTA = 0x00;
}

// function to select between clockwise and counter-clockwise rotation
void run(int dir)
{
    switch(dir){
        case 1:
        runClockWise(60);
        break;
        
        case -1:
        runCounterClockWise(45);
        break;
    }
}

int main(void)
{
    DDRA |= 0x78;
    while(1){
        run(1);
        disablePins();
        _delay_ms(1000);
        run(-1);
        disablePins();
        _delay_ms(1000);
    }
    return 1;
}

/*const int val[8] = {0x08,0x12,0x04,0x06,0x02,0x03,0x01,0x09};
void runVal()
{
    int i = 0;
    while(i<8){
        PORTA = val[i];
        _delay_ms(100);
        i++;
    }
}
*/
