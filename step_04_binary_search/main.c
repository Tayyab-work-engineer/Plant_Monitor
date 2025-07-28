#include <avr/io.h>
#include <string.h>
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>


#define BAUD 9600
#include <util/setbaud.h>

typedef struct {
    char name[20];

    uint16_t moisture_thresholds[4];  // 5 levels = 4 thresholds
    uint16_t light_thresholds[4];
    uint16_t temp_thresholds[4];

    uint8_t num_thresholds; // usually 4 for all
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

uint16_t read_adc(uint8_t channel){
    ADMUX = (1 << REFS0);               // Use AVcc as reference (5V)
    ADMUX |= (channel & 0x0F);          // Set ADC channel (0 to 7)

    ADCSRA = (1 << ADEN)                // Enable ADC
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128

    ADCSRA |= (1 << ADSC);              // Start conversion
    while (ADCSRA & (1 << ADSC));       // Wait for conversion to complete

    return ADC;                         // Return 10-bit result (0–1023)
}

int binary_classify(uint16_t reading, uint16_t* thresholds, uint8_t size){
    
    int low = 0;
    int high = size - 1;
    
    while(low <= high){
        int mid = (low+high) / 2;
        
        if(reading<thresholds[mid]){
            high = mid - 1;
        }
        else{
            low = mid + 1;
        }
    }
    if (low > size)
        low = size; // clamp if needed

    
    return low; // index for the zone/label
    
    
}

void classify_and_print(const char* label, uint16_t reading, uint16_t thresholds[], const char* labels[], uint8_t num_thresholds){
    
    char buffer[50];
    
    //print raw value
    
    sprintf(buffer, "%s: %u", label, reading);
    print_ln(buffer);
    
    int zone = binary_classify(reading, thresholds, num_thresholds);

    
    // Safety check for label index (zone should be between 0 and num_thresholds)
    if (zone >= 0 && zone <= num_thresholds) {
        print_ln(labels[zone]);
    } else {
        print_ln("Unknown zone");
    }
    
}


int main(void){
    const char* moisture_labels[] = {"Very Dry", "Dry", "Optimal", "Wet", "Very Wet"};
    const char* light_labels[] = {"Dark", "Dim", "Normal", "Bright", "Very Bright"};
    const char* temp_labels[] = {"Cold", "Cool", "Warm", "Hot", "Very Hot"};
    
    PlantProfile plants[] = {
        {
            "Aloe Vera",
            {200, 400, 600, 800},  // Moisture: Very Low, Low, Optimal, High, Very High
            {100, 300, 600, 900},  // Light
            {10, 20, 30, 40},      // Temp (°C scaled to ADC or real °C)
            4                     // 4 thresholds → 5 zones
        },
        {
            "Basil",
            {250, 450, 650, 850},  // Moisture
            {150, 350, 650, 950},  // Light
            {15, 22, 30, 38},      // Temp
            4
        }
    };
    
    uart_init();
    //uint8_t is
    
    uint8_t buttonPrev = 1;  // Previous state of the button (1 = not pressed)

    ADCSRA |= (1<<ADEN); //enable adc
    
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
            print_ln("Checking plant conditions:");
            
            uint16_t temp_raw = read_adc(0); //LM35 on A0
            
            float tempC = (temp_raw * 5.0 / 1024.0) * 100;
            
            //These are placeholder values for testing the logic until real sensor input is added.
            uint16_t moisture = 300 + rand() % 400; //simulate a moisture reading
            uint16_t light = 200 + rand() % 600; //simulate a light reading
            uint16_t temp = (uint16_t)tempC;

            PlantProfile current = plants[0];

            classify_and_print("Moisture", moisture, current.moisture_thresholds, moisture_labels, current.num_thresholds);
            classify_and_print("Light", light, current.light_thresholds, light_labels, current.num_thresholds);
            classify_and_print("Temp", temp, current.temp_thresholds, temp_labels, current.num_thresholds);
            
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



