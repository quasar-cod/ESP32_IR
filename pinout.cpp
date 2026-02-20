

#include <Arduino.h>


void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  Serial.println("-------------");
  Serial.println("Initialized serial communications with the PC.");
// const int myArray []= {  2, 3, 4, 5, 12, 13, 14, 15, 16};
// per 8266
//  for (int i : myArray) {
//     Serial.println(i);
//     pinMode(i,OUTPUT);
//     digitalWrite(i, HIGH); 
//     delay(1000);    
//     digitalWrite(i, LOW); 
//     delay(1000);
//   }

}
void loop() {

  Serial.println("LED_BLU");

  delay(5000);


}