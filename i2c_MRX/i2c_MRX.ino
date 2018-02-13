#include <Wire.h>
int enc;
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(0x62, 3);    // request 6 bytes from slave device #8
  enc = EncoderCount();
  Serial.println();
  //Serial.println(enc);
}

int EncoderCount(){
  int count = 0;
  int dig = 2;
  while (Wire.available()) { // slave may send less than requested
    int b = Wire.read();
    Serial.print(b);
    count += pow(10, dig)*b;
    dig--;
  }
  return count;
}

