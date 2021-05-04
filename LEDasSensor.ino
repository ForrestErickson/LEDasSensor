/*LEDasSensor
   Author: Forrest Erickson
   Date: 20210503
   Description: Experiments on Arduino Uno for sensing light with LED as a photo cell.
   Added a EMA (Exponential Moving Average) to the serial output.  
   In Ardunio, view serial ouput with Serial Ploter CTRL+SHIFT+L. 
   Ref: https://en.wikipedia.org/wiki/Moving_average#Cumulative_moving_average

   Based on:
   LED light sensing
   Hunter Carlson
   June 9 2009
   From: https://forum.arduino.cc/t/leds-as-photo-diodes/114569
*/

#define SAMPLEFREQ 4000
const int SAMPLEPERIOD = 1000000/SAMPLEFREQ;  //Microseconds. About 66.6 samples per cycle of 60 hz.

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


//int light = 217;        // set light threshold
//int light = 900;        // set in !AS lab with red LED as sensor before adding read to A0 normalization.
int light = 50;        // set in !AS lab with red LED as sensor
//int light = 10;        // set in !AS lab with red LED as sensor

//An exponential moving average (EMA),
//float alpha = 0.99;
//float alpha = 0.25;
float alpha = 0.125;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);          //  setup serial
  //_SFR_IO8(0x35) |= 0x10;   // global disable pull up resistors
  pinMode(LED01, OUTPUT);
  pinMode(LED02, OUTPUT);
  pinMode(LED01GND, OUTPUT);
  pinMode(LED02GND, OUTPUT);
  digitalWrite(LED01GND, LOW);
  digitalWrite(LED02GND, LOW);

  // Get intial values for EMA
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val01EMA = map(analogRead(sense01), 0 , 1024, 0, 5000);
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val02EMA = map(analogRead(sense02), 0 , 1024, 0, 5000);

  digitalWrite(LED_BUILTIN, LOW);
}// end setup()

/* Read the light (LED Voltages) and scale to 0 to 5V.
 * Take EMA of readings.
 * Compair to a threshold value and turn on LEDs if measured light below threshold.
 * Wait till time to take next sample.
 */

void loop()
{
  //  val01 = analogRead(sense01);      // read sense01 led
  //  val02 = analogRead(sense02);      // read sense02 led
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val01 = map(analogRead(sense01), 0 , 1024, 0, 5000);    // read sense01 led and map to mV.
  analogRead(senseReset);     //Read ground pin to normalize by discharge ADC
  val02 = map(analogRead(sense02), 0 , 1024, 0, 5000);    // read sense02 led

  //Print out the EMA smoothed value.
  //  Serial.println(val01);            //debug print
  //  Serial.println(val02);            //debug print
  val01EMA = alpha * val01 + (1 - alpha) * val01EMA;
  Serial.println(val01EMA);
  val02EMA = alpha * val02 + (1 - alpha) * val02EMA;
//  Serial.println(val01EMA);


  if (val01EMA >= light) {              // check if light
    digitalWrite(LED01, LOW);     // if light, turn off led
  } else {
    digitalWrite(LED01, HIGH);    // if dark, turn on led
  }

  if (val02EMA >= light) {              // check if light
    digitalWrite(LED02, LOW);     // if light, turn off led
  } else {
    digitalWrite(LED02, HIGH);    // if dark, turn on led
  }

  //delay(1); //Slow down data.
  delayMicroseconds(SAMPLEPERIOD); //Slow down data. 
}//end loop()
