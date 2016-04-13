/**
 * This is an Arduino based clock source for the 6502 (CMOS).
 *
 * This version is meant to be run on an Attiny85.
 * The clock source can be started, stopped and single-stepped.
 */

#define LED 4
#define startButton 2
#define stepButton 3

volatile int ledState;

bool run = false;

int buttonState;
int lastButtonState;

void setupButtons()
{
  pinMode(LED, OUTPUT);
  pinMode(startButton, INPUT);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(stepButton, INPUT);
  pinMode(stepButton, INPUT_PULLUP);
}

void setupTimer0()
{
  // Initialize timer1
  TCCR0A = 0;  // Clear timer0 A and B registers
  TCCR0B = 0;

  TCCR0A |= (1 << WGM01);  // Set CTC mode

  // Set up a 2Hz clock signal
  TCCR0B |= (1 << CS02) | (1 << CS00);   // Set prescaler to 1024
  OCR0A = 255;           // 8MHz / 1024 (prescaler) / 255 (TOP) / 2 (on/off) = 2Hz clock

  TIMSK |= (1 << OCIE0A); // Set interrupt on compare match

  sei();                   // Enable interrupts
}

void setup()
{
  setupButtons();
  setupTimer0();
}

void loop() {

  buttonState = digitalRead(startButton);

  if (buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
        // Stop timer0 if it is running
        if (run)
        {
          TCCR0B = 0;
          run = false;
        }
        else
        {
          TCNT0 = 0;                // Reset counter to 0
          TCCR0A |= (1 << WGM01);   // Set CTC mode

          // Set up a 2Hz clock signal
          TCCR0B |= (1 << CS02) | (1 << CS00);
          run = true;
        }
    }
    lastButtonState = buttonState;
  }

  if (digitalRead(stepButton) == LOW)
  {
    // Stop timer0 if it is running
    if (run)
    {
      TCCR0B = 0;
      run = false;
    }

    // Send a one-shot clock pulse
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    }
}

ISR (TIMER0_COMPA_vect)
{
  if (ledState == LOW)
  {
    ledState = HIGH;
  }
  else
  {
    ledState = LOW;
  }
  digitalWrite(LED, ledState);
}
