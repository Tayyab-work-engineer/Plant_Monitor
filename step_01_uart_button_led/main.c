#include <avr/io.h>
#include <string.h>
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define BAUD 9600
#include <util/setbaud.h>

void uart_init(void){
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
    //character size 8
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
    
    UCSR0B = (1<<TXEN0);
}

void uart_transmit(unsigned char data){
    while ( !(UCSR0A & (1<<UDRE0))); //wait to get ready to transmit
    UDR0 = data;
}

void print_ln(const char* str){
    int length = strlen(str);
    int i;

    for (i=0;i<length;i++){
        uart_transmit(str[i]);
    }
    
    uart_transmit(10); //new line


}

int main(void){
    uart_init();
    //uint8_t is
    uint8_t buttonPrev = 1;  // Previous state of the button (1 = not pressed)

    
    DDRB = (1<<PB5); //Set pin 5 as output
    PORTB = 0;
    
    DDRD = 0; //Set Port D as input
    PORTD |= (1<<PD6); // Pull up resistor
    
    while(1){
        uint8_t buttonNow;
        
        if (PIND & (1<<PD6)){
            buttonNow = 1;
        }else{
            buttonNow = 0;
        }
        
        
        if ( buttonPrev == 1 && buttonNow == 0 ){
            PORTB = (1<<PB5);
            print_ln("LED ON");
        }
        else if (buttonPrev == 0 && buttonNow == 1){
            PORTB = (0<<PB5);
            buttonPrev = 1;

        }
        
        buttonPrev = buttonNow;
        _delay_ms(20); // Debounce delay

    }
    
    return 0;
}


