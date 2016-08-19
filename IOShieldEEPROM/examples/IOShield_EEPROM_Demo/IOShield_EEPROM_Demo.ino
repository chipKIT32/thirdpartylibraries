
/* Note: For chipKit Max users you must manually 
** connect SDA and SCL pins(20 and 21) to pins A4 and A5 of 
** IO Shield 
**
** Note: For chipKit Uno Users you must have Jumpers JP6 and JP8 
** set in the RG3 and RG2 positions
*/

#include <IOShieldEEPROM.h>
#include <Wire.h>

void setup()
{
  char buf[26];
  char single;
  int i;
  Serial.begin(9600);
  
  //Write the alphabet to the EEPROM
  //The data length is assumed in this example because the data
  //is a string. If you write an array numbers to EEPROM then the
  //length of the array must be provided.
  IOShieldEEPROM.writeString(0,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  
  //Read alphabet back and print to monitor
  IOShieldEEPROM.readString(0, buf, 26);
  Serial.print(buf);

  
  Serial.println();
  
  //Write a single character
  IOShieldEEPROM.write(20,'#');
  
  //Read all characters back
  IOShieldEEPROM.readString(0, buf, 26);
  Serial.print(buf);
  
  Serial.println();
  
  //Read back last letter
  single = IOShieldEEPROM.read(25);
  Serial.print(single);
}

void loop()
{
}
