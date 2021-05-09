/*LEDasSensor
   Author: Forrest Erickson
   Date: 20210503
   Description: Experiments on Arduino Uno for sensing light with LED as a photo cell.
   Added a EMA (Exponential Moving Average) to the serial output.  
   In Ardunio, view serial ouput with Serial Ploter CTRL+SHIFT+L. 
   Ref: https://en.wikipedia.org/wiki/Moving_average#Cumulative_moving_average

   Results:
   Comment out all but the AnalogRead for Sensor 1 and the set LED1 code and set for ADC clock 1MHz
   But leaving in the map function.
   The onboard LED is pulsed around the AnalogRead reset to trigger an oscilliscope for measurement.
   The frequency of the main loop is 9.7 to 10.13 KHz. 
   By reducing the The output LED 
   Adding the print statement back in loop reduces the sample rate to 2.5 to 2.9 KHz.

   Based on:
   LED light sensing
   Hunter Carlson
   June 9 2009
   From: https://forum.arduino.cc/t/leds-as-photo-diodes/114569
*/

/*Set frequency. Set 4000, however the measures loop frequency is about 820Hz
 * 
 */

#define SAMPLEFREQ 4000                       //About 66.6 samples per cycle of 60 hz.
 
const int SAMPLEPERIOD = 1000000/SAMPLEFREQ;  //Microseconds. 

//Light sensor LEDs wired for
int senseReset = 0;     // A0 input tied to GND.
int sense01 = 5;        // sensing LED Cathode connected to analog5. Anode to GND.
int sense02 = 4;        // sensing LED Cathode connected to analog4. Anode to GND.

int LED01 = 9;          // 330 Ohm resistor, to LED Annode on dig9,
int LED01GND = 8;       // LED Cathode to be set to ground

int LED02 = 11;         // 330 Ohm resistor, to LED Annode on dig11,
int LED02GND = 12;      // LED Cathode to be set to ground

int val01 = 0;          // variable to store the value read from sense01
int val02 = 0;          // variable to store the value read from sense02
float val01EMA = 0;     // variable to store the exponential moving average sense01
float val02EMA = 0;     // variable to store the exponential moving average sense02


//Threshold for dark indication.
//Note there is no hysterisis on this comparision.
//int light = 900;        // set in !AS lab with red LED as sensor before adding read to A0 normalization.
//int light = 217;        // set light threshold
//int light = 150;        // set in !AS lab with red LED as sensor
//int light = 50;         // set in !AS lab with red LED as sensor
//int light = 35;         // set in !AS lab with red LED as sensor
int light = 10;         // set in !AS lab with red LED as sensor
//int light = 3;            // set in !AS lab with red LED as sensor

//An exponential moving average (EMA),
//float alpha = 0.99;
//float alpha = 0.25;
//float alpha = 0.125;
//float alpha = 0.0625;
//float alpha = 0.0375;     //P-P is 23 to 34 under lab lights.
//float alpha = 0.01;
float alpha = 0.001;        //P-P is 29 to 30 under lab lights.

//Timer setup for ADC
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  //LED goes high as setup() starts

  // set ADC prescale to 16 for faster conversion, 1MHz conversion. Measured at 25uS conversion
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

//  Serial.begin(115200);          //  setup serial
//  Serial.begin(1000000);          //  setup serial
  Serial.begin(2000000);          //  setup serial  1.8K sampe / second at 1MHz ADC clock
  
  pinMode(LED01, OUTPUT);
  pinMode(LED02, OUTPUT);
  pinMode(LED01GND, OUTPUT);
  pinMode(LED02GND, OUTPUT);
  digitalWrite(LED01GND, LOW);    //Ground LED cathode pins.
  digitalWrite(LED02GND, LOW);

  // Get intial values for EMA
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val01EMA = map(analogRead(sense01), 0 , 1024, 0, 5000);
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val02EMA = map(analogRead(sense02), 0 , 1024, 0, 5000);

  digitalWrite(LED_BUILTIN, LOW); //LED goes low as end of setup() 
}// end setup()

/* Read the light (LED Voltages) and scale to 0 to 5000 mV.
 * Take EMA of readings.
 * Compair to a threshold value and turn on LEDs if measured light below threshold.
 * Wait till time to take next sample.
 */

void loop()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //Toggle built in LED so we can sample rate.
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC. About 120uS for read.
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //Toggle built in LED so we can sample rate.
  val01 = map(analogRead(sense01), 0 , 1024, 0, 5000);    // read sense01 led and map to mV.
//  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
//  val02 = map(analogRead(sense02), 0 , 1024, 0, 5000);    // read sense02 led

  //Print out the EMA smoothed value.
  val01EMA = alpha * val01 + (1 - alpha) * val01EMA;
//  val01EMA = val01;         // No EMA

  //Serial print adds significant delay. Even at 2Mbaud the sample rate is limited to: 1.6 to 2.0 KS/Sec
  Serial.println(val01EMA);     // 4.3KS/Sec when commented and with ADC clock 1MHz.

  //LED 1
//  if (val01EMA >= light) {         
//    digitalWrite(LED01, LOW);     
//  } else {
//    digitalWrite(LED01, HIGH);    
//  }
  digitalWrite(LED01, !(val01EMA >= light));

//  val02EMA = alpha * val02 + (1 - alpha) * val02EMA;
//  Serial.println(val01EMA);

  //LED 2
  //About 26uS to write the LED
//  if (val02EMA >= light) {              
//    digitalWrite(LED02, LOW);     
//  } else {
//    digitalWrite(LED02, HIGH);    
//  }

//  digitalWrite(LED02, (val02EMA <= light));

  //delay(1); //Slow down data.
//  delayMicroseconds(SAMPLEPERIOD); //Slow down data. With out delay, and 1MHz clock sample freq 1.7KHz,

  
}//end loop()
