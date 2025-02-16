#include "common.h"

void common::setup()
{
    __builtin_avr_cli();

    DDRD = _BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
    DDRC = _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3);
    DDRB = _BV(PB1) | _BV(PB2);

    // 7-segment and led's initially off
    PORTC |= _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3);
    PORTD |= _BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
    PORTB &= ~(_BV(PB1) | _BV(PB2));

    __builtin_avr_sei();
}

void common::uto7(const uint16_t value, uint8_t *buffer)
{
    uint16_t quotient = value;
    uint16_t remainder = 0;

    do
    {
        remainder = quotient % 10;
        *buffer++ = pgm_read_byte(&common::SEVEN_SEGMENT_MAP[remainder]);
        quotient /= 10;
    } while (quotient > 0);
}