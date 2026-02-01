#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED_PIN PB3
#define GREEN_PIN PD6
#define BLUE_PIN PD5

#define RED_CH OCR2A
#define GREEN_CH OCR0A
#define BLUE_CH OCR0B

uint8_t red_prev = 0;
uint8_t green_prev = 0;
uint8_t blue_prev = 0;

uint16_t total_minutes = 0;

volatile uint8_t master_brightness = 100;

void init_pwm () {
    //init pins output
    DDRB |= (1 << RED_PIN);
    DDRD |= (1 << GREEN_PIN) | (1 << BLUE_PIN);
    
    //INIT Timer0
    TCCR0A |= (1 << WGM01) | (1 << WGM00); //Fast-PWM mode
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); //Connect timer out to pin A and B

    TCCR0B |= (1 << CS01) | (1 << CS00); //Prescaler 64

    //INIT Timer2
    TCCR2A |= (1 << WGM21) | (1 << WGM20); //Fast-PWM mode
    TCCR2A |= (1 << COM2A1); //Connect timer out to pin A

    TCCR2B |= (1 << CS22); //Prescaler 64
}

void set_rgb (uint8_t red, uint8_t green, uint8_t blue) {
    RED_CH = ((uint16_t)red * master_brightness) / 100;
    GREEN_CH = ((uint16_t)green * master_brightness) / 100;
    BLUE_CH = ((uint16_t)blue * master_brightness) / 100;
}

void fade_to(uint8_t r_target, uint8_t g_target, uint8_t b_target, uint16_t duration_ms) {
    const uint8_t step_delay = 1; 
    
    int steps = duration_ms / step_delay; 
    if (steps == 0) steps = 1;

    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;

        uint8_t r_now = red_prev + (int)(r_target - red_prev) * t;
        uint8_t g_now = green_prev + (int)(g_target - green_prev) * t;
        uint8_t b_now = blue_prev + (int)(b_target - blue_prev) * t;

        set_rgb(r_now, g_now, b_now);
        
        _delay_ms(step_delay);
    }

    red_prev = r_target;
    green_prev = g_target;
    blue_prev = b_target;
}

uint8_t bcd_to_dec(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}
uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}
void i2c_init () {
    TWSR = 0x00; //clear register and set prescaler - 1

    TWBR = 72; // 100 kgz
}

void i2c_start () {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    //     clear interp flag      start          enable

    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop () {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write (uint8_t data) {
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_nack () {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t data = TWDR;
    return data;
}

uint8_t i2c_read_ack () {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));

    uint8_t data = TWDR;
    return data;
}

void set_time () {
    i2c_start();
    i2c_write(0xD0);
    i2c_write(0x00);
    
    i2c_write(dec_to_bcd(0));
    i2c_write(dec_to_bcd(31));
    i2c_write(dec_to_bcd(2));
    
    i2c_stop();
}

void get_time () {
        i2c_start();
        i2c_write((0x68 * 2) + 0); //clock adress 0x68 + 0 - WRITE
        i2c_write(0x01); // start with minutes
        i2c_start();
        i2c_write((0x68 * 2) + 1); //clock adress 0x68 + 1 - READ

        uint8_t minutes = bcd_to_dec(i2c_read_ack()); // Read minutes (0x01), ACK - need to read more
        uint8_t hours = bcd_to_dec(i2c_read_nack()); // Read hours (0x02), NACK - stop
        
        total_minutes = (hours * 60) + minutes;

        i2c_stop();
}

void init_buttons () {
    DDRD &= ~((1 << PD2) | (1 << PD3)); //out
    PORTD = (1 << PD2) | (1 << PD3); //pull up

    EICRA = (1 << ISC01) | (1 << ISC11);; // falling edge
    EICRA &= ~((1 << ISC00) | (1 << ISC10));

    EIMSK = (1 << INT0) | (1 << INT1); // INT0 - PD2 pin, INT1 - PD3 pin

    sei(); //start
}

ISR(INT0_vect) {
    if (master_brightness > 5) master_brightness -= 5;
    _delay_ms(40);
}

ISR(INT1_vect) {
    if (master_brightness < 95) master_brightness += 5;
    _delay_ms(40);
}

int main (void) {
    init_pwm();
    
    init_buttons();

    i2c_init();

    //set_time();

    while (1) {

        get_time();

        if ((total_minutes >= 1320) || (total_minutes < 480)) {
             fade_to(50, 0, 0, 5000);
        }
        else if (total_minutes >= 1230) {
             fade_to(255, 140, 20, 5000);
        }
        else {
             fade_to(150, 0, 255, 5000);
        }
        
        _delay_ms(1000);
    }
}