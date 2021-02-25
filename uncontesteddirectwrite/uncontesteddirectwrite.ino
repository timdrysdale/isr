#define CANARY 7
bool toggle = false;
#define encoderPinA 3     //these pins all have interrupts on them.

float interval = 0.005; //in seconds

// Timer
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024
float timer_interrupt_freq = 1 / interval;

// direct write
// https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/wiring_digital.c#L86
EPortType port = g_APinDescription[CANARY].ulPort;
uint32_t pin = g_APinDescription[CANARY].ulPin;
uint32_t pinMask = (1ul << pin);

void canary(void) {
  PORT->Group[port].OUTSET.reg = pinMask;
  digitalWrite(CANARY,toggle);
  PORT->Group[port].OUTCLR.reg = pinMask;
}


//This function is run on a timer interrupt defined by PIDInterval/timer_interrupt_freq.
void TimerInterrupt(void) {
  // do nothing
}

//===================================================================================
//====================== SETUP AND LOOP =============================================
//===================================================================================

void setup() {
  pinMode(CANARY, OUTPUT);
  pinMode(encoderPinA, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), canary, CHANGE);
  startTimer(timer_interrupt_freq);




  
}

void loop() {
  // nothing to see here!
}

 

//===================================================================================
//======================TIMER INTERRUPT FUNCTIONS====================================
//      FROM https://github.com/nebs/arduino-zero-timer-demo/
//===================================================================================

void setTimerFrequencyHz(int frequencyHz) {
  int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
  TcCount16* TC = (TcCount16*) TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

/*
This is a slightly modified version of the timer setup found at:
https://github.com/maxbader/arduino_tools
 */
void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  // Set prescaler to 1024
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  setTimerFrequencyHz(frequencyHz);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we run the TimerInterrupt() function.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    TimerInterrupt();           //THE FUNCTION TO RUN ON THE TIMER
  }
}
