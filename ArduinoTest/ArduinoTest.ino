
void setup() { 
 //Initialize serial and wait for port to open:
  Serial.begin(38400); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
} 

int val = 4096;
int conta = 0;

void loop() { 

  //Serial.print("0 1 2 3 4 5\r\n"); 
  Serial.print("0 ");
  
  for(int i = 0; i < 4; ++i)
  {
    Serial.print(val--);
    Serial.print(" ");
  }
  
  // canale 0 senza ultimo spazio tra sesto valore e CRLF
  Serial.print(val--);
  Serial.print("\r\n"); 
  
  delay(20);
  
  // Serial.print("1 6 7 8 9 10\r\n"); 
  Serial.print("1 ");
  
  for(int i = 0; i < 5; ++i)
  {
    Serial.print(val--);
    Serial.print(" ");
  }
  
  Serial.print("\r\n"); 
  
  if((++conta % 196) == 0)
  {
    Serial.print("lungo messaggio di debug che viene da arduino con contatore che vale ");
    Serial.print(val);
    Serial.print(" e continua fino a qui\r\n"); 
  }
  
  delay(30);
  
  if(val <= -4096)
    val = 4096;
} 
