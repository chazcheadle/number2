/**
 * This is an AVR based clock source for the 6502 (CMOS).
 *
 * This version is can be flashed onto a non-Arduino'd chip.
 * The clock source can be started, stopped and single-stepped.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#define startButton 2
#define stepButton 3


volatile int ledState;

int run = 0;

int buttonState = 1;
int lastButtonState = 1;

void setupButtons()
{
  DDRB |= (1 << PB3); // Set pin 4 as output
  DDRB &= ~(1 << PB1); // Set startButton as input
  PORTB |= (1 << PB1); // enable input_pullup
  DDRB &= ~(1 << PB2); // Set stepButton as input
  PORTB |= (1 << PB2); // enable input_pullup
}

void setupTimer0()
{
  // Initialize timer1
  TCCR0A = 0;  // Clear timer0 A and B registers
  TCCR0B = 0;

  TCCR0A |= (1 << WGM01);  // Set CTC mode

  // Set up a 2Hz clock signal
  TCCR0B |= (1 << CS02) | (1 << CS00);   // Set prescaler to 1024
  OCR0A = 255;           // 8MHz / 1024 (prescaler) / 255 (TOP) / 2 (on/off) = 15Hz clock

  TIMSK |= (1 << OCIE0A); // Set interrupt on compare match

  sei();                   // Enable interrupts
}

int main() {

  setupButtons();
  setupTimer0();

  while (1)
  {
    buttonState = (PINB & PB1);

    if (lastButtonState != buttonState)
    {
      if (buttonState == 0)
      {
          // Stop timer0 if it is running
          if (run)
          {
            TCCR0B = 0;
            run = 0;
          }
          else
          {
            TCNT0 = 0;                // Reset counter to 0
            TCCR0A |= (1 << WGM01);   // Set CTC mode

            // Set up a 2Hz clock signal
            TCCR0B |= (1 << CS02) | (1 << CS00);
            run = 1;
          }
      }
      lastButtonState = buttonState;
    }


    if (PINB & PB2)
    {
      // Stop timer0 if it is running
      if (run)
      {
        TCCR0B = 0;
        run = 0;
      }

      // Send a one-shot clock pulse
      PORTB |= (1 << PB3);
      _delay_ms(200);
      PORTB ^= (1 << PB3);
      _delay_ms(200);
    }
  }
}

ISR (TIMER0_COMPA_vect)
{
  if (ledState == 1)
  {
    PORTB ^= (1 << PB3);
  }
  else
  {
    PORTB |= (1 << PB3);
  }
}
