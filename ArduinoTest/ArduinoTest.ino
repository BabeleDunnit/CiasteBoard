
void setup() { 
 //Initialize serial and wait for port to open:
  Serial.begin(38400); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
} 

void loop() { 

  Serial.print("0 1 2 3 4 5\r\n"); 
  
  delay(50);
  
  Serial.print("1 6 7 8 9 10\r\n"); 
  
  delay(50);
} 
