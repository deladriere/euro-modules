/*
  
 */

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);  
  
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
