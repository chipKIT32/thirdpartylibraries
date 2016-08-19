/* Note: For chipKit Max users you must manually 
** connect SDA and SCL pins(20 and 21) to pins A4 and A5 of 
** IO Shield 
**
** Note: For chipKit Uno Users you must have Jumpers JP6 and JP8 
** set in the RG3 and RG2 positions
** 
** This example also checks the status of the Alert pin. Normally this
** could be used to send some information to an external device, but
** for this example it's just checked when we check the tempereture.
** to use this part of the example connect the A pin on header JP4
** to pin 2 on your ChipKit board.
**
*/

#include <IOShieldTemp.h>
#include <Wire.h>

#define ALERT_PIN 2

void setup()
{
  Serial.begin(9600);
  pinMode(ALERT_PIN, INPUT);  
  //Initialize Configuration register for oneshot with 11 bit
  //resolution
  IOShieldTemp.config(IOSHIELDTEMP_ONESHOT | IOSHIELDTEMP_RES11 | IOSHIELDTEMP_ALERTHIGH);
  
  //Set the range to bring the alert pin high if it's above 78F (25.5C), alert will stay
  //high until the temp drops below 75.2F (24C).
  IOShieldTemp.setTempLimit(IOShieldTemp.convFtoC(78)); // 78.0F ~ 25.5C
  IOShieldTemp.setTempHyst(24); // 75.2F ~ 24.0C
}

void loop() 
{
  float tempF, tempC;
  
  //Get Temperature in Celsius.
  tempC = IOShieldTemp.getTemp();
  
  // Convert the result to Fahrenheit.
  tempF = IOShieldTemp.convCtoF(tempC);

  //Print Temperature to serial port
  Serial.print(tempC);
  Serial.print(" C, ");
  Serial.print(tempF);
  Serial.print(" F");
  if(digitalRead(ALERT_PIN) == HIGH)
    Serial.print(" ALERT!");
  Serial.println();
  
  delay(1000);  
}
