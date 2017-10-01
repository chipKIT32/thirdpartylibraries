// Eddystone.ino
// Author: M16946
// Date: 2017/01/13

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <Arduino.h>
#include <RN487x_BLE.h>
#include <EddystoneBeacon.h>

#define debugSerial SerialUSB
#if defined(ARDUINO_SODAQ_EXPLORER)
#define bleSerial Serial1
#else 
#define bleSerial Serial
#endif

#define SERIAL_TIMEOUT  10000

const char* myEddystoneURI = "http://www.microchip.com" ;
const uint8_t myTxPower = 0xC5 ;

void initLed()
{
  #if defined(ARDUINO_SODAQ_EXPLORER)
  pinMode(LED_BUILTIN, OUTPUT) ;
  #endif
  pinMode(LED_RED, OUTPUT) ;
  pinMode(LED_GREEN, OUTPUT) ;
  pinMode(LED_BLUE, OUTPUT) ;  
  pinMode(BUTTON, INPUT_PULLUP) ;
}

void turnBlueLedOn()
{
  #if defined(ARDUINO_SODAQ_EXPLORER)
  digitalWrite(LED_BUILTIN, HIGH) ;
  #endif
}

void turnBlueLedOff()
{
  #if defined(ARDUINO_SODAQ_EXPLORER)
  digitalWrite(LED_BUILTIN, LOW) ;
  #endif
}

#define COMMON_ANODE  // LED driving
void setRgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef COMMON_ANODE
  red = 255 - red ;
  green = 255 - green ;
  blue = 255 - blue ;
#endif

  analogWrite(LED_RED, red) ;
  analogWrite(LED_GREEN, green) ;
  analogWrite(LED_BLUE, blue) ;
}

void setup()
{
  while ((!debugSerial) && (millis() < SERIAL_TIMEOUT)) ;

  debugSerial.begin(115200) ;

  initLed() ;

  // Set the optional debug stream
  rn487xBle.setDiag(debugSerial) ;
  // Initialize the BLE hardware
  rn487xBle.hwInit() ;
  // Open the communication pipe with the BLE module
  bleSerial.begin(rn487xBle.getDefaultBaudRate()) ;
  // Assign the BLE serial port to the BLE library
  rn487xBle.initBleStream(&bleSerial) ;
  // Finalize the init. process
  if (rn487xBle.swInit())
  {
    setRgbColor(0, 255, 0) ;
    debugSerial.println("Init. procedure done!") ;
  }
  else
  {
    setRgbColor(255, 0, 0) ;
    debugSerial.println("Init. procedure failed!") ;
    while(1) ;
  }

  // >> Configuring the BLE
  // First enter in command/configuration mode
  rn487xBle.enterCommandMode() ;
  rn487xBle.stopAdvertising() ;
  // Set the advertising output power (range: min = 5, max = 0)
  rn487xBle.setAdvPower(3) ;
  // Clear Beacon adv. packet
  rn487xBle.clearImmediateBeacon() ;

  // >> Start and generate Eddystone URI frame
  eddystoneBeacon.beginURI(myTxPower, myEddystoneURI) ;

  debugSerial.println("Starter Kit as an EddyStone Beacon") ;
  debugSerial.println("==================================") ;
  debugSerial.print("Started to generate URI frame: ") ;
  debugSerial.println(myEddystoneURI) ;

}

void loop()
{ 

}
