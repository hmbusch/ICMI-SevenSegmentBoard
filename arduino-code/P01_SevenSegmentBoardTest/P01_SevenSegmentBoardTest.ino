/* =========================================================================
 * P01_SevenSegmentBoardTest
 * =========================================================================
 *
 * Test program to verify the proper functionality of the SevenSegment board
 * by displaying an alternating pattern on all digits.
 *
 */

int rClockPin = 12;
int rDataPin = 11;
int rLatchPin = 10;

int dClockPin = 9;
int dResetPin = 8;

// the number of digits in the display
byte digitCount = 8;

// the digit that is currently lit
int currentDigit = 0;

// The initial pattern displayed
byte pattern = B10010010;

/*
 * 'Pulses' the given pin by switching it high and then low once
 * in very short sequence. (20 microseconds between state changes).
 */
void pulse(int pin)
{
  delayMicroseconds(20);
  digitalWrite(pin, HIGH);
  delayMicroseconds(20);
  digitalWrite(pin, LOW);
}

/*
 * Sets up the pins and configures the internal timer to about 75Hz.
 */
void setup()
{
  pinMode(rDataPin, OUTPUT);
  pinMode(rClockPin, OUTPUT);
  pinMode(rLatchPin, OUTPUT);
  
  pinMode(dClockPin, OUTPUT);
  pinMode(dResetPin, OUTPUT); 
  
  // Calculation: 16 MHz / 1024 Prescaler / 48 Preload (i.e. 207 Overflow Limit) = ~75 Hz
  // 16,000,000 : 1024 = 15625
  // 15625 : 207 = 75,48...
  TCCR2A = 0;                       // normal operation for timer 2
  TCCR2B = (1<<CS22) | (1<<CS20);   // select prescaler 1024
  TCNT2 = 48;                       // set preload to 48 (leaving a 207 divisor)
  TIMSK2 = (1<<TOIE2);              // enable overflow interrupt
}

/*
 * Applies a pattern to the digits and alternates between this pattern and
 * it's inversion every seconds. 
 */ 
void loop()
{
  digitalWrite(rLatchPin, LOW);
  shiftOut(rDataPin, rClockPin, LSBFIRST, pattern);
  digitalWrite(rLatchPin, HIGH);
  
  delay(1000);
  
  pattern = pattern ^ B11111111;
}

/*
 * This is the method called when the internal timer trigger. The
 * execution is then delegated to the displayNextSegment() method.
 */
ISR(TIMER2_OVF_vect)
{
  displayNextSegment();
};

/*
 * Switches supply power to the next digit by clocking the 
 * decade counter. If the last digit is reached, the cycle starts
 * over with the first digit.
 */
void displayNextSegment()
{
  // This modulo division ensures that the digit index is always 0 <= digitIndex < digitCount.
  // It also causes the first cycle to be unsychronized because the decade displays the first digit, whereas
  // the shift register already displays the next one. Due to the high speed refresh and the reset of the
  // decade counter once per cycle, register and counter will be in sync again after the first cycle.
  currentDigit = (currentDigit + 1) % digitCount;

  // We need to reset the decade once per cycle  
  if(currentDigit == 0)
  {
    pulse(dResetPin);
  }
  else
  {
    pulse(dClockPin);
  } 
}
