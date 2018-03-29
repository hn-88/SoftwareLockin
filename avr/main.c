#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


volatile uint8_t count;



// interupt 0 function
ISR(INT0_vect)
{
    count++;
}



// timer 0 function
ISR(TIMER0_COMPA_vect)
{
    /*
    when 200Hz (x1) count = 35
         400Hz (x10)        70
         600Hz (x100)       105
         800Hz (x1000)      130
         
     */
    
    register uint8_t a = count;
    count = 0;
    

    if (a < (35 + 17)) {
        PORTB = 0x00;
    } else if (a < (70 + 17)) {
        PORTB = 0x01;
    } else if (a < (105 + 17)) {
        PORTB = 0x08;
    } else {
        PORTB = 0x09;
    }

}



int main(void)
{
    // set PORT B as output
    DDRB = 0x09;
    
    // inisialize PORTB
    PORTB = 0x00;
    // and the counter
    count = 0;


    // initialize the timer0
    // Clear Timer on Compare Match (CTC) mode
    TCCR0A = (1<<WGM01);
    // select the clock divisor : clkI/O / 1024
    TCCR0B = (1<<CS02) | (1<<CS00);
    // Output Compare Register A
    OCR0A  = 170;
    // Timer/Counter0 Output Compare Match A Interrupt Enable
    TIMSK |= (1<<OCIE0A);

    // initialize the interupt0 
    // rising edge
    MCUCR = (1<<ISC01) | (1<<ISC00);
    // int0 enabled
    GIMSK = (1<<INT0);


    // Enables interrupts by setting the global interrupt mask
    sei(); 

    // infinit loop
    for (;;) {
        // sleep mode to reduce the power
        sleep_mode();
    }

    return 0;
}
