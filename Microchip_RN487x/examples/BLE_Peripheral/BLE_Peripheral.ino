// BLE_Peripheral.ino
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
#include "LIS3DE.h"

#define debugSerial SerialUSB

#if defined(ARDUINO_SODAQ_EXPLORER)
#define bleSerial Serial1
#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
#define bleSerial Serial
#endif

#define SERIAL_TIMEOUT  10000

#define ADC_AREF 3.3f
#define BATVOLT_R1 4.7f
#define BATVOLT_R2 10.0f

// Temperature offset for Sodaq One
#define TEMP_OFFSET 20

LIS3DE accelerometer;

const char* myDeviceName = "My SODAQ BOARD" ;  // Custom Device name
const char* myPrivateServiceUUID = "AD11CF40063F11E5BE3E0002A5D5C51B" ; // Custom private service UUID
const char* temperatureCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C501" ;  // custom characteristic GATT
const uint8_t temperatureCharacteristicLen = 2 ;  // data length (in bytes)
const uint16_t temperatureHandle = 0x72 ;
char temperaturePayload[temperatureCharacteristicLen*2] ;
float temperature_s ;
uint8_t newTempValue_u8 = 0 ;
uint8_t prevTempValue_u8 = 0 ;
const char* ledCharacteristicUUID = "BF3FBD80063F11E59E690002A5D5C503" ;  // custom characteristic GATT
const uint8_t ledCharacteristicLen = 6 ;
uint16_t ledHandle = 0x75 ;
const char* ledPayload ;

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

/**
 * Init Temperature for ExpLoRer
 */
void initTemperature()
{
  #if defined(ARDUINO_SODAQ_EXPLORER)
  pinMode(TEMP_SENSOR, INPUT) ;
  //Set ADC resolution to 12 bits
  analogReadResolution(12) ; 
  #endif
}

/**
 * Read ExpLoRer temperature sensor
 */
float getTemperature()
{
  #if defined(ARDUINO_SODAQ_EXPLORER)
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0 ;
  #else
  float mVolts = (float)getBoardTemperature() * 100 + 500;
  #endif
  float temp = (mVolts - 500.0) / 100.0 ;
  return temp ;
}

/**
 * Initializes the accelerometer or puts it in power-down mode
 * for the purpose of reading its temperature delta.
*/
void setAccelerometerTempSensorActive(bool on)
{
    if (on) {
        accelerometer.enable(false, LIS3DE::NormalLowPower100Hz, LIS3DE::XYZ, LIS3DE::Scale2g, true);
        delay(300); // should be enough for initilization and 2 measurement periods
    }
    else {
        accelerometer.disable();
    }
}

/**
 * Returns the board temperature.
 * SDOAQ One
*/
float getBoardTemperature()
{
    setAccelerometerTempSensorActive(true);

    float temp = TEMP_OFFSET + accelerometer.getTemperatureDelta();

    setAccelerometerTempSensorActive(false);

    return temp;
}

void setup()
{
  while ((!debugSerial) && (millis() < SERIAL_TIMEOUT)) ;
  
  debugSerial.begin(115200) ;
  
  Wire.begin();

  initLed() ;
  initTemperature() ;

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

  // Fist, enter into command mode
  rn487xBle.enterCommandMode() ;
  // Stop advertising before starting the demo
  rn487xBle.stopAdvertising() ;
  // Set the advertising output power (range: min = 5, max = 0)
  rn487xBle.setAdvPower(3) ;
  // Set the serialized device name
  rn487xBle.setSerializedName(myDeviceName) ;
  rn487xBle.clearAllServices() ;
  rn487xBle.reboot() ;
  rn487xBle.enterCommandMode() ;  
  // Set a private service ...
  rn487xBle.setServiceUUID(myPrivateServiceUUID) ;
  // which contains ...
  // ...a temperature characteristic; readable and can perform notification, 2-octets size
  rn487xBle.setCharactUUID(temperatureCharacteristicUUID, READ_PROPERTY | NOTIFY_PROPERTY, temperatureCharacteristicLen) ;
  // ...an LED characteristic; properties: writable
  rn487xBle.setCharactUUID(ledCharacteristicUUID, WRITE_PROPERTY, ledCharacteristicLen) ;       
  // take into account the settings by issuing a reboot
  rn487xBle.reboot() ;
  rn487xBle.enterCommandMode() ;
  // Clear adv. packet
  rn487xBle.clearImmediateAdvertising() ;
  // Start adv.
  rn487xBle.startImmediateAdvertising(AD_TYPE_MANUFACTURE_SPECIFIC_DATA, "CD00FE14AD11CF40063F11E5BE3E0002A5D5C51B") ;

  debugSerial.println("Starter Kit as a Peripheral with private service") ;
  debugSerial.println("================================================") ;
  debugSerial.print("Private service: ") ;  debugSerial.println(myPrivateServiceUUID) ;
  debugSerial.print("Private characteristic used for the Temperature: ") ; debugSerial.println(temperatureCharacteristicUUID) ;
  debugSerial.print("Private characteristic used for the LED: ") ; debugSerial.println(ledCharacteristicUUID) ;
  debugSerial.println("You can now establish a connection from the Microchip SmartDiscovery App") ;
  debugSerial.print("with the starter kit: ") ; debugSerial.println(rn487xBle.getDeviceName()) ;

}

void loop()
{
  // Check the connection status
  if (rn487xBle.getConnectionStatus())
  {
    // Connected to a peer
    debugSerial.print("Connected to a peer central ") ;
    debugSerial.println(rn487xBle.getLastResponse()) ;

    // Temperature
    prevTempValue_u8 = newTempValue_u8 ;
    temperature_s = getTemperature() ;
    newTempValue_u8 = (int)temperature_s ;
    // Update the local characteristic only if value has changed
    if (newTempValue_u8 != prevTempValue_u8)
    {
      uint8_t data = newTempValue_u8 ;
      temperaturePayload[3] = '0' + (data % 10) ; // LSB
      data /= 10 ;
      temperaturePayload[2] = '0' + (data % 10) ;
      data /= 10 ;
      temperaturePayload[1] = '0' + (data % 10) ;
      if (temperature_s > 0)  temperaturePayload[0] = '0' ; // MSB = 0, positive temp.
      else                    temperaturePayload[0] = '8' ; // MSB = 1, negative temp.  
         
      if (rn487xBle.writeLocalCharacteristic(temperatureHandle, temperaturePayload))
      {
        debugSerial.print("Temperature characteristic has been updated with the value = ") ; debugSerial.println(newTempValue_u8) ;
      }
    }

    // LED
    if (rn487xBle.readLocalCharacteristic(ledHandle))
    {
      // Read the local characteristic written by the client: smartphone/tablet
      // The payload must follow the 6-bit RGB coded format:
      // [0] Red MSB
      // [1] Red LSB
      // [2] Green MSB
      // [3] Green LSB
      // [4] Blue MSB
      // [5] Blue LSB
      // Each color can be coded from range 0x00 to 0xFF
      ledPayload = rn487xBle.getLastResponse() ;
      if (ledPayload != NULL)
      {
        if (ledPayload[0] != 'N') // != "N/A" response
        {
          debugSerial.println(ledPayload) ;
          if (strlen(ledPayload) == ledCharacteristicLen)
          {
            // Filter only the 6-digit len
            // Convert ASCII to integer values
            uint8_t r = (ledPayload[0] - '0') * 10 + (ledPayload[1] - '0') ;
            uint8_t g = (ledPayload[2] - '0') * 10 + (ledPayload[3] - '0') ;
            uint8_t b = (ledPayload[4] - '0') * 10 + (ledPayload[5] - '0') ;
            setRgbColor(r, g, b) ;
          }
        }
      }
    }        
  }
  else
  {
    // Not connected to a peer device
    debugSerial.println("Not connected to a peer device") ;
  }
  // Delay inter connection polling
  delay(3000) ;
}

