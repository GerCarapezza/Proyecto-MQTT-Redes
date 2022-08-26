#include <AM2320.h>
 
AM2320 th(&Wire);
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
}
 
void loop() {
  switch(th.Read()) {
    case 1:
      Serial.println(F("Sensor offline"));
      break;
    case 0:
      Serial.print(F("\tHumidity = "));
      Serial.print(th.Humidity);
      Serial.println(F("%"));
      Serial.print("\tTemperature = ");
      Serial.print(th.cTemp);
      Serial.println(F("*C"));
      Serial.println();
      break;
  }
  delay(2000);
}
  
