#include <Arduino.h>
#include <avr/sleep.h>

void power_off() {
    sleep_enable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    digitalWrite(LED_BUILTIN, LOW);

    // disable and turn off modules
    ADCSRA &= 0x7f;  // disable ADC, needed before turning it off in PRR
    PRR = 0xEF;      // turn off all modules

    // enable id change interrupts for paddle and encoder sw
    cli();
    // PCICR |= 0b00000001;    // enable Port B id change interrupts
    // PCICR &= ~0b00000110;   // disable Port D&C id change interrupts
    // PCMSK0 |= 0b00001100;   // PCINT0..7    enable paddle switch
    //    PCMSK1 |= 0b00000000;   // PCINT8..15   enable 10 (encoder sw)
    //    PCMSK2 |= 0b11000000; // PCINT16..23, enable 22 & 23
    sei();
    sleep_cpu();

    // never gets here

    // PRR = 0x00;     // turn on all modules
    // ADCSRA |= 0x80; // enable ADC

    // restore pullup state of input pins
    // pinMode(STEPPER_AT0_PIN, INPUT_PULLUP);
    // pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    // pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    // pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    // pinMode(PADDLE_PIN, INPUT_PULLUP);
    // delay(1);
    //    PCICR |= 0b00000010;  // enable Port C id change interrupts
    // PCICR &= ~0b00000111; // disable Port D, C & B id change interrupts
    digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
    // put your setup code here, to run once:
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    // put your main code here, to run repeatedly:
    for (int i = 0; i < 4; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
    }
    power_off();
}
