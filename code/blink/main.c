//
// A simple program for tinyDriver (ATtiny84A) that blinks an RGB LED.
//
// electronut.in
//

#include <avr/io.h>
#include <util/delay.h>
 
#define F_CPU 8000000

int main (void)
{
    // set PA0, PA1, PA2 as output
    DDRA = (1 << PA0) | (1 << PA1) | (1 << PA2);

    // loop
    while (1) {

        // flash red
        PORTA = (1 << PA0);
        _delay_ms(500);
        PORTA = 0;
        _delay_ms(500);

        // flash green
        PORTA = (1 << PA1);
        _delay_ms(500);
        PORTA = 0;
        _delay_ms(500);

        // flash blue
        PORTA = (1 << PA2);
        _delay_ms(500);
        PORTA = 0;
        _delay_ms(500);

    }
}
