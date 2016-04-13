/**
 * This is an Arduino based clock source for the 6502 (CMOS).
 *
 * The clock source can be started, stopped and single-stepped.
 */

#define LED 13
#define startButton 2
#define stepButton 3

volatile int ledState;

bool run = false;

int buttonState;
int lastButtonState;

void setupButtons()\
{
  pinMode(LED, OUTPUT);
  pinMode(startButton, INPUT);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(stepButton, INPUT);
  pinMode(stepButton, INPUT_PULLUP);
}

void setupTimer1()
{
  // Initialize timer1
  TCCR1A = 0;  // Clear timer1 A and B registers
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);  // Set CTC mode

  // Set up a 2Hz clock signal
  TCCR1B |= (1 << CS12);   // Set prescaler to 256
  OCR1A = 15625;           // 16MHz / 256 (prescaler) / 15625 (TOP) / 2 (on/off) = 2Hz clock

  // Examples of alternate times.
  //  TCCR1B |= (1 << CS10);   // Set prescaler to 256
  //  OCR1A = 16;              // 16MHz / 1 (prescaler) / 16 (TOP) / 2 (on/off) = .5MHz clock
  //  OCR1A = 8;               // 16MHz / 1 (prescaler) / 8 (TOP) / 2 (on/off) = 1MHz clock


  TIMSK1 |= (1 << OCIE1A); // Set interrupt on compare match

  sei();                   // Enable interrupts
}

void setup()
{
  setupButtons();
  setupTimer1();
}

void loop() {

  buttonState = digitalRead(startButton);

  if (buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
        // Stop timer1 if it is running
        if (run)
        {
          TCCR1B = 0;
          run = false;
        }
        else
        {
          TCNT2 = 0;                // Reset counter to 0
          TCCR1B |= (1 << WGM12);   // Restore timer1 settings
          TCCR1B |= (1 << CS12);
          run = true;
        }
    }
    lastButtonState = buttonState;
  }

  if (digitalRead(stepButton) == LOW)
  {
    // Stop timer1 if it is running
    if (run)
    {
      TCCR1B = 0;
      run = false;
    }

    // Send a one-shot clock pulse
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    }
}

ISR (TIMER1_COMPA_vect)
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
