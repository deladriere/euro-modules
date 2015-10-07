/*
  http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/
 */



// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);



// the setup routine runs once when you press reset:


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);  
  pinMode(5, INPUT_PULLUP);  

   // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_128;    // set our own prescaler to 64 

  
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  
  // print out the value you read:
  Serial.print(analogRead(A0));
  Serial.print("  ");
   Serial.print(analogRead(A1));
  Serial.print("  ");
   Serial.print(analogRead(A2));
  Serial.print("  ");
   Serial.print(analogRead(A3));
  Serial.print("  ");
  Serial.print(analogRead(A4));
  Serial.print("  ");

  Serial.print(digitalRead(2));
  Serial.print("  ");
  
  Serial.print(digitalRead(4));
  Serial.print("  ");

  Serial.print(digitalRead(5));
  Serial.println("  ");
  
  delay(200);        // delay in between reads for stability
}
