
#include <avr/io.h>
#include <string.h>
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define BAUD 9600
#include <util/setbaud.h>

typedef struct {
    char name[20];
    uint16_t moisture_thresholds[3];   // [dry, optimal, wet]
    uint16_t light_thresholds[3];      // [dark, normal, bright]
    uint16_t temp_thresholds[3];       // [cold, normal, hot]
} PlantProfile;


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

void classify_and_print(const char* label, uint16_t reading, uint16_t thresholds[3]){
    uart_transmit('[');
    print_ln(label);
    
    if (reading<thresholds[0]){
        print_ln(": Too Low");
    }
    else if (reading<thresholds[1]){
        print_ln(": Optimal");
    }
    else{
        print_ln(": Too High");
    }
}

int main(void){
    PlantProfile plants[] = {
        {"Aloe Vera", {300, 500, 700}, {200, 500, 800}, {15, 25, 35}},
        {"Basil",     {400, 600, 800}, {300, 600, 900}, {20, 27, 35}}
    };
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
            print_ln("Conditions");
            
            
            //These are placeholder values for testing the logic until real sensor input is added.
            uint16_t moisture = 550; //simulate a moisture reading
            uint16_t light = 250; //simulate a light reading
            uint16_t temp = 30; //simulate a temp reading
            
            PlantProfile current = plants[0];

            classify_and_print("Moisture", moisture, current.moisture_thresholds);
            classify_and_print("Light", light, current.light_thresholds);
            classify_and_print("Temp", temp, current.temp_thresholds);

            
            
            
        }
        else if (buttonPrev == 0 && buttonNow == 1){
            PORTB = (0<<PB5);
            //print_ln("LED OFF");


        }
        
        buttonPrev = buttonNow;
        _delay_ms(20); // Debounce delay

    }
    
    return 0;
}


