/* =========================================================================
 * P02_SevenSegmentNumberDisplay
 * =========================================================================
 *
 * The segments are connected to the SevenSegmentBoard in the following order:
 *
 *             a
 *           #####
 *          #     #
 *        f #  g  # b
 *           ##### 
 *          #     #
 *        e #  d  # c
 *           #####
 *
 * This information is relevant for forming the patterns for the digits, 
 * provided the bit order is set correctly when writing to the register.
 *
 */
 
 /*
  * This array holds the patterns to display on a single digit of the
  * display. Index 0 to 9 are the regular numbers, 10 is a completely
  * blank number and anything above index 10 are reserved symbols for
  * special purposes. Using the index, it is quite easy to maintain the
  * complete display state in a numeric array.
  */
 byte digitPatterns[] = {
   B11111100, // 0
   B01100000, // 1
   B11011010, // 2
   B11110010, // 3
   B01100110, // 4
   B10110110, // 5
   B10111110, // 6
   B11100000, // 7
   B11111110, // 8
   B11110110, // 9
   B00000000, // 10 - blank digit
 };

int rClockPin = 12;
int rDataPin = 11;
int rLatchPin = 10;

int dClockPin = 9;
int dResetPin = 8;

// the number of digits in the display
byte digitCount = 8;

// the digit that is currently lit
int currentDigit = 0;

// the index of the pattern currently displayed
int currentPattern = 0;

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
  
  // Calculation: 16 MHz / 1024 Prescaler / 223 Preload (i.e. 32 Overflow Limit) = ~488 Hz
  // 488 interrupts per seconds means 61 interrupts per digit, thus 61 complete display 
  // refreshs per seconds (~60 Hz) 
  // 16,000,000 : 1024 = 15625
  // 15625 : 32 = 488.28...
  TCCR2A = 0;                       // normal operation for timer 2
  TCCR2B = (1<<CS22) | (1<<CS20);   // select prescaler 1024
  TCNT2 = 223;                      // set preload to 223 (leaving a 32 divisor)
  TIMSK2 = (1<<TOIE2);              // enable overflow interrupt
}

/*
 * Applies a pattern to the digits and alternates between this pattern and
 * it's inversion every seconds. 
 */ 
void loop()
{
  digitalWrite(rLatchPin, LOW);
  shiftOut(rDataPin, rClockPin, LSBFIRST, digitPatterns[currentPattern]);
  digitalWrite(rLatchPin, HIGH);
  
  delay(250);
  
  if (currentPattern == 9)
  {
    currentPattern = 0;
  }
  else
  {
    currentPattern++;
  }
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
