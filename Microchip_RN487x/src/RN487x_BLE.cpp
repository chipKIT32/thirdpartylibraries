// RN487x_BLE.cpp
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

#include "RN487x_BLE.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(ARDUINO_SODAQ_ONE)
  #define BLUETOOTH_WAKE  (26u) // A6/D6
  #define BT_RESET  (24u) //A2/D2
#elif defined (_BOARD_FUBARINO_MINI_)
  #define BLUETOOTH_WAKE    (12u)
  #define BT_RESET          (7u) 
#endif

// Bluetooth Reset signal is supported from the BSP version SODAQ_SAMD_BOARD_1.6.10
// Redefine the pin in case of using prior version than v1.6.9
#ifndef BT_RESET
  #define BT_RESET  (46u)
#endif

//#define DEBUG

#ifdef DEBUG
#define debugPrintLn(...) { if (this->diagStream) this->diagStream->println(__VA_ARGS__);  }
#define debugPrint(...)   { if (this->diagStream) this->diagStream->print(__VA_ARGS__);    }
#warning "Debug mode is ON"
#else
#define debugPrintLn(...)
#define debugPrint(...)
#endif

// ---------------------------------------- Private section ----------------------------------------

Rn487xBle rn487xBle;

// *********************************************************************************
// Constructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
Rn487xBle::Rn487xBle(void)
{
  uartBufferLen = DEFAULT_INPUT_BUFFER_SIZE;
  uartBuffer = new char[uartBufferLen];
  memset(uartBuffer, 0, uartBufferLen);
}

// *********************************************************************************
// Destructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
Rn487xBle::~Rn487xBle()
{
  if (uartBuffer != NULL)
  {
    delete []uartBuffer;
    uartBuffer = NULL;
  }
}

// *********************************************************************************
// Hardware Init. procedure
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::hwInit(void)
{
  debugPrintLn("[hwInit]");

  // Hardware reset
  hwReset();

  // Place the BLUETOOTH_WAKE pin in Low level
  hwWakeUp();
}

// *********************************************************************************
// Software Init. procedure
// *********************************************************************************
// Input : void
// Output: bool true if successfully initialized
// *********************************************************************************
bool Rn487xBle::swInit(void)
{
  debugPrintLn("[swInit]");

  bool initOk = false;

  if (reboot())
  {
    // Set a known mode
    setOperationMode(dataMode);
    initOk = true;
  }
  else
  {
    if (enterCommandMode())
    {
      if (reboot())
      {
        // Set a known mode
        setOperationMode(dataMode);
        initOk = true;
      }
    }
  }
  return initOk;
}

// *********************************************************************************
// Reset the module
// *********************************************************************************
// RESET pin (UART_RX_IND) must be pulled LOW to issue an hardware reset.
// Then wait at least 5ms before UART can be operated.
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::hwReset(void)
{
  debugPrintLn("[hwReset]");

  pinMode(BT_RESET, OUTPUT);
  digitalWrite(BT_RESET, LOW);
  delay(1);
  digitalWrite(BT_RESET, HIGH);
  delay(500);
}

// *********************************************************************************
// Wake up the module's UART
// *********************************************************************************
// WAKE pin (UART_RX_IND) must be pulled LOW to start internal 16MHz clock.
// Then wait at least 5ms before UART can be operated.
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::hwWakeUp(void)
{
  debugPrintLn("[wakeUp]");
    
  pinMode(BLUETOOTH_WAKE, OUTPUT);
  digitalWrite(BLUETOOTH_WAKE, LOW);
  delay(5);
}

// *********************************************************************************
// Shutdown the module's UART
// *********************************************************************************
// WAKE pin (UART_RX_IND) must be pulled HIGH to stop internal 16MHz clock and run
// with the internal 32kHz oscillator.
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::hwSleep(void)
{
  debugPrintLn("[sleep]");

  pinMode(BLUETOOTH_WAKE, OUTPUT);
  digitalWrite(BLUETOOTH_WAKE, HIGH);
  delay(1);
}


// *********************************************************************************
// Enter into command mode
// *********************************************************************************
// All UART data is treated as ASCII commands sent to the module's UART interface
// Input : void
// Output: bool true if successfully entered into command mode
// *********************************************************************************
bool Rn487xBle::enterCommandMode(void)
{
  debugPrintLn("[enterCommandMode]");
  
  // To enter Command mode from Data mode, type $$$ character sequence after 100 ms delay before the first $
  delay(DELAY_BEFORE_CMD);

  memset(uartBuffer, 0, uartBufferLen); // clear the buffer
  cleanInputBuffer();

  this->bleSerial->print(ENTER_CMD);
  
  // This command will reply with either "CMD> " or "CMD<CR><LF>" depending on if
  // the command prompt is turned on or off. This routine needs to detect which
  // reply comes back, and set the commandPromptEnabled variable appropriately. This 
  // reply takes less than 15ms to arrive. Each string is 5 bytes long.

  debugPrintLn("[expectResponse] expecting CMD> or CMD");

  unsigned long previous = millis();
  
  while ((millis() - previous < 15) && (this->bleSerial->available() < 5))
  {
    debugPrint(".");
  }

  this->bleSerial->readBytes(this->uartBuffer, this->bleSerial->available());

  debugPrint("(");
  debugPrint(this->uartBuffer);
  debugPrint(")");

  if (strstr(this->uartBuffer, PROMPT) != NULL)
  {
    debugPrintLn(" found a match ! - Command prompts are enabled.");
    setOperationMode(cmdMode);
    return true;
  }
  else if (strstr(this->uartBuffer, PROMPT_CR) != NULL)
  {
    debugPrintLn(" found a match ! - Command prompts are disabled.");
    setOperationMode(cmdMode);
    return true;
  }
  return false;
}

// *********************************************************************************
// Enter into data mode (default mode)
// *********************************************************************************
// When RN487x is connected to another BLE device and is in Data mode
// the RN487x acts as a data pipe: any serial data sent into RN487x UART is
// transfered to the connected peer device via Transparent UART Bluetooth service.
// When data is received from the peer device over the air via Transparent UART
// connection, this data outputs directly to UART.
// Input : void
// Output: bool true if successfully entered into data mode
// *********************************************************************************
bool Rn487xBle::enterDataMode(void)
{
  debugPrintLn("[enterDataMode]");

  sendCommand(EXIT_CMD);
  if (expectResponse(PROMPT_END, DEFAULT_CMD_TIMEOUT))
  {
    setOperationMode(dataMode);
    return true;
  }
  return false;
}

// *********************************************************************************
// Send a command
// *********************************************************************************
// Once in command mode, valid ASCII commands can be issued to control or configure
// the RN487x. All commands end with a carriage return <cr> and are always responded
// to by the RN487x. Any subsequent command must not be issued until a response is
// received for the previous command.
// When data is received from the peer device over the air via Transparent UART
// connection, this data outputs directly to UART.
// Input : String command stream
// Output: void
// *********************************************************************************
void Rn487xBle::sendCommand(String stream)
{
  debugPrint("[sendCommand] " );
  debugPrintLn(stream);
  
  this->flush();
  cleanInputBuffer();
  stream.concat(CR);
  bleSerial->print(stream);
}

void Rn487xBle::sendData(char *data, uint16_t dataLen)
{
  bleSerial->write(data, dataLen);
}


// *********************************************************************************
// Retrieve the BT Address from the settings in configuration flash
// *********************************************************************************
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::retrieveBtAddress(void)
{
  if (getSettings(0, 6))
  {
    btAddress[0] = uartBuffer[10];
    btAddress[1] = uartBuffer[11];
    btAddress[2] = uartBuffer[8];
    btAddress[3] = uartBuffer[9];
    btAddress[4] = uartBuffer[6];
    btAddress[5] = uartBuffer[7];
    btAddress[6] = uartBuffer[4];
    btAddress[7] = uartBuffer[5];
    btAddress[8] = uartBuffer[2];
    btAddress[9] = uartBuffer[3];
    btAddress[10]= uartBuffer[0];
    btAddress[11]= uartBuffer[1];
    return true;
  }
  return false;
}

// *********************************************************************************
// Get the RN487x Firwmare version
// *********************************************************************************
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::getFirmwareVersion(void)
{
  debugPrint("[getFirmwareVersion]");

  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;

  sendCommand(DISPLAY_FW_VERSION);
  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        return true;
      }
    }
  }
  return false;
}

// *********************************************************************************
// Disable the Beacon feature
// *********************************************************************************
// Input : void
// Output: return true if successfully executed
// *********************************************************************************
bool Rn487xBle::disableBeacon(void)
{
  debugPrintLn("[disableBeacon]");
  return(setBeaconFeatures(BEACON_OFF));
}

// *********************************************************************************
// Enable Beacon feature, connectable advertisement is disabled
// *********************************************************************************
// Input : void
// Output: return true if successfully executed
// *********************************************************************************
bool Rn487xBle::enableBeacon(void)
{
  debugPrintLn("[enableBeacon]");
  return(setBeaconFeatures(BEACON_ON));
}

// *********************************************************************************
// Non-connectable Beacon and connectable advertisement are both enabled
// *********************************************************************************
// Input : void
// Output: return true if successfully executed
// *********************************************************************************
bool Rn487xBle::enableBeaconAndAdv(void)
{
  debugPrintLn("[enableBeaconAndAdv]");
  return(setBeaconFeatures(BEACON_ADV_ON));
}

// *********************************************************************************
// Factory Reset
// *********************************************************************************
// Resets the configurations into factory default.
// This command causes an immediate reboot after invoking it.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::factoryReset(void)
{
  debugPrintLn("[factoryReset]");

  sendCommand(FACTORY_RESET);
  if (expectResponse(FACTORY_RESET_RESP, RESET_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Adjust the output power under advertisement
// *********************************************************************************
// Range from (0-5)
// 0 represents highest output power and 5 lowest output power
// Input : uint8_t value (0-5)
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setAdvPower(uint8_t value)
{
  debugPrint("[setAdvPower] ");
  debugPrintLn(value);

#if (MIN_POWER_OUTPUT != 0)
  if (value < MIN_POWER_OUTPUT)
  {
    value = MIN_POWER_OUTPUT;
  }
#endif
  if (value > MAX_POWER_OUTPUT)
  {
    value = MAX_POWER_OUTPUT;
  }
  
  uint8_t len = strlen(SET_ADV_POWER);
  char c[1];
  sprintf(c, "%d", value);
  this->flush();
  memcpy(uartBuffer, SET_ADV_POWER, len);
  memcpy(&uartBuffer[len], c, 1);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Adjust the output power under connected state
// *********************************************************************************
// Range from (0-5)
// 0 represents highest output power and 5 lowest output power
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setConPower(uint8_t value)
{
  debugPrint("[setConnPower] ");
  debugPrintLn(value);

#if (MIN_POWER_OUTPUT != 0)
  if (value < MIN_POWER_OUTPUT) 
  {
    value = MIN_POWER_OUTPUT;
  }
#endif
  if (value > MAX_POWER_OUTPUT)
  {
    value = MAX_POWER_OUTPUT;
  }
   
  uint8_t len = strlen(SET_CONN_POWER);
  char c[1];
  sprintf(c, "%d", value);
  this->flush();
  memcpy(uartBuffer, SET_CONN_POWER, len);
  memcpy(&uartBuffer[len], c, 1);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Set Serialized Device Name
// *********************************************************************************
// Sets a serialized Bluetooth name for the device where the name is up to 
// MAX_SERIALIZED_NAME_LEN alphanumeric characters. This method automatically 
// appends the last two bytes of the Bluetooth MAC address to the name which is 
// useful for generating a custom name with unique numbering.
// Input : *newName
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setSerializedName(const char *newName)
{
  debugPrint("[setSerializedName] ");
  debugPrintLn(newName);

  uint8_t len = strlen(SET_SERIALIZED_NAME);
  uint8_t newLen = strlen(newName);
  if (newLen > MAX_SERIALIZED_NAME_LEN)
  {
    newLen = MAX_SERIALIZED_NAME_LEN;
    debugPrintLn(" Too many characters");
  }

  // Fill the device name without the last two bytes of the Bluetooth MAC address
  memset(deviceName, 0, newLen);
  memcpy(deviceName, newName, newLen);
  // Fill the buffer
  this->flush();
  memcpy(uartBuffer, SET_SERIALIZED_NAME, len);
  memcpy(&uartBuffer[len], newName, newLen);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Set Device Name
// *********************************************************************************
// Sets the device name where the name is up to MAX_DEVICE_NAME_LEN alphanumeric characters
// Input : *newName
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setDevName(const char *newName)
{
  debugPrint("[setDeviceName] ");
  debugPrintLn(newName);

  uint8_t len = strlen(SET_DEVICE_NAME);
  uint8_t newLen = strlen(newName);
  if (newLen > MAX_DEVICE_NAME_LEN)
  {
    newLen = MAX_DEVICE_NAME_LEN;
    debugPrintLn(" Too many characters");
  }

  // Fill the device name
  memset(deviceName, 0, newLen);
  memcpy(deviceName, newName, newLen);
  // Fill the buffer
  this->flush();
  memcpy(uartBuffer, SET_DEVICE_NAME, len);
  memcpy(&uartBuffer[len], newName, newLen);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Enables Low-Power operation mode
// *********************************************************************************
// Module enables low-power mode by running 32kHz clock with much lower power
// consumption. When module runs on 32kHz clock, UART is not operational
// Module restarts 16MHz clock by using the hwWakeUp() method
// When module runs on 32kHz clock, a BLE connection can still be maintained but
// UART cannot receive data
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::enableLowPower(void)
{
  debugPrintLn("[enableLowPower]");
  
  sendCommand(SET_LOW_POWER_ON);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Disables Low-Power operation mode
// *********************************************************************************
// Module runs 16MHz clock all the time, therefore can operate UART all the time.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::disableLowPower(void)
{
  debugPrintLn("[disableLowPower]");
  
  sendCommand(SET_LOW_POWER_OFF);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Sets the supported feature of the module
// *********************************************************************************
// After changing the features a reboot() is necessary to make the changes effective
// Input : uint16_t bitmap that indicates features to be supported
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setSupportedFeatures(uint16_t bitmap)
{
  debugPrint("[setSupportedFeatures]");

  uint8_t len = strlen(SET_SUPPORTED_FEATURES);
  char c[4];
  sprintf(c, "%04X", bitmap);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, SET_SUPPORTED_FEATURES, len);
  memcpy(&uartBuffer[len], c, newLen);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Sets the default services to be supported by the module in GAP server role
// *********************************************************************************
// Supporting service in server role means that the host MCU must supply the values 
// of all characteristics in supported services and to provide client access to
// those values upon request. Once the service bitmap is modified, the device must
// reboot() to make the new services effective.
// Input : uint8_t bitmap that indicates the services to be supported as a server
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setDefaultServices(uint8_t bitmap)
{
  debugPrint("[setDefaultServices]");

  uint8_t len = strlen(SET_DEFAULT_SERVICES);
  char c[2];
  sprintf(c, "%02X", bitmap);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, SET_DEFAULT_SERVICES, len);
  memcpy(&uartBuffer[len], c, newLen);
  
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Clear all services
// *********************************************************************************
// Clears all settings of services and characteristics.
// A power cycle is required afterwards to make the changes effective.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearAllServices(void)
{
  debugPrintLn("[cleanAllServices]");
  
  sendCommand(CLEAR_ALL_SERVICES);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Advertisement
// *********************************************************************************
// The advertisement is undirect connectable.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startAdvertising(void)
{
  debugPrintLn("[startAdvertising]");
  
  sendCommand(START_DEFAULT_ADV);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Custom Advertisement
// *********************************************************************************
// The advertisement is undirect connectable. (note, with no second parameter, this
//   function will use the form of the START_CUSTOM_ADV command where no second 
//   parameter is sent, thus making advertisements last forever)
// Input : interval - the number of milliseconds between advertisements
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startCustomAdvertising(uint16_t interval)
{
  debugPrintLn("[startCustomAdvertising]");

  uint8_t len = strlen(START_CUSTOM_ADV);
  char parameters[5];
  sprintf(parameters, "%04X", interval);
  uint8_t newLen = strlen(parameters);

  this->flush();
  memcpy(uartBuffer, START_CUSTOM_ADV, len);
  memcpy(&uartBuffer[len], parameters, newLen);
  
  sendCommand(uartBuffer);

  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Custom Advertisement
// *********************************************************************************
// The advertisement is undirect connectable.
// Input : interval - the number of milliseconds between advertisements
// Input : totalAdTime - the number of 640ms ticks until advertisements stop
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startCustomAdvertising(uint16_t interval, uint16_t totalAdTime)
{
  debugPrintLn("[startCustomAdvertising]");

  uint8_t len = strlen(START_CUSTOM_ADV);
  char parameters[10];
  sprintf(parameters, "%04X,%04X", interval, totalAdTime);
  uint8_t newLen = strlen(parameters);

  this->flush();
  memcpy(uartBuffer, START_CUSTOM_ADV, len);
  memcpy(&uartBuffer[len], parameters, newLen);
  
  sendCommand(uartBuffer);

  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Stops Advertisement
// *********************************************************************************
// Stops advertisement started by the startAdvertising method.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::stopAdvertising(void)
{
  debugPrintLn("[stopAdvertising]");
  
  sendCommand(STOP_ADV);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Clear the advertising structure Immediately
// *********************************************************************************
// Make the changes immediately effective without a reboot.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearImmediateAdvertising(void)
{
  debugPrintLn("[clearImmediateAdvertising]");
  
  sendCommand(CLEAR_IMMEDIATE_ADV);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Clear the advertising structure in a permanent way
// *********************************************************************************
// The changes are saved into NVM only if other procedures require permanent
// configuration changes. A reboot is requested after executing this method.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearPermanentAdvertising(void)
{
  debugPrintLn("[clearPermanentAdvertising]");
  
  sendCommand(CLEAR_PERMANENT_ADV);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Clear the Beacon structure Immediately
// *********************************************************************************
// Make the changes immediately effective without a reboot.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearImmediateBeacon(void)
{
  debugPrintLn("[clearImmediateBeacon]");
  
  sendCommand(CLEAR_IMMEDIATE_BEACON);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}


// *********************************************************************************
// Clear the Beacon structure in a permanent way
// *********************************************************************************
// The changes are saved into NVM only if other procedures require permanent
// configuration changes. A reboot is requested after executing this method.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearPermanentBeacon(void)
{
  debugPrintLn("[clearPermanentBeacon]");
  
  sendCommand(CLEAR_PERMANENT_BEACON);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Advertising immediately
// *********************************************************************************
// Input : uint8_t adType  Bluetooth SIG defines AD types in the assigned number list
//         in the Core Specification
//         const char adData[] has various lengths and follows the format defined
//         by Bluetooth SIG Supplement to the Bluetooth Core specification
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startImmediateAdvertising(uint8_t adType, const char adData[])
{
  debugPrintLn("[startImmediateAdvertising]");

  uint8_t len = strlen(START_IMMEDIATE_ADV);
  char c[2];
  sprintf(c, "%02X", adType);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, START_IMMEDIATE_ADV, len);
  memcpy(&uartBuffer[len], c, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], adData, strlen(adData));
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Advertising permanently
// *********************************************************************************
// A reboot is needed after issuing this method
// Input : uint8_t adType  Bluetooth SIG defines AD types in the assigned number list
//         in the Core Specification
//         const char adData[] has various lengths and follows the format defined
//         by Bluetooth SIG Supplement to the Bluetooth Core specification
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startPermanentAdvertising(uint8_t adType, const char adData[])
{
  debugPrintLn("[startPermanentAdvertising]");

  uint8_t len = strlen(START_PERMANENT_ADV);
  char c[2];
  sprintf(c, "%02X", adType);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, START_PERMANENT_ADV, len);
  memcpy(&uartBuffer[len], c, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], adData, strlen(adData));
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Beacon adv immediatly
// *********************************************************************************
// Input : uint8_t adType  Bluetooth SIG defines AD types in the assigned number list
//         in the Core Specification
//         const char adData[] has various lengths and follows the format defined
//         by Bluetooth SIG Supplement to the Bluetooth Core specification
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startImmediateBeacon(uint8_t adType, const char adData[])
{
  debugPrintLn("[startImmediateBeacon]");

  uint8_t len = strlen(START_IMMEDIATE_BEACON);
  char c[2];
  sprintf(c, "%02X", adType);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, START_IMMEDIATE_BEACON, len);
  memcpy(&uartBuffer[len], c, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], adData, strlen(adData));
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Beacon adv permanently
// *********************************************************************************
// A reboot is needed after issuing this method
// Input : uint8_t adType  Bluetooth SIG defines AD types in the assigned number list
//         in the Core Specification
//         const char adData[] has various lengths and follows the format defined
//         by Bluetooth SIG Supplement to the Bluetooth Core specification
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startPermanentBeacon(uint8_t adType, const char adData[])
{
  debugPrintLn("[startPermanentBeacon]");

  uint8_t len = strlen(START_PERMANENT_BEACON);
  char c[2];
  sprintf(c, "%02X", adType);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, START_PERMANENT_BEACON, len);
  memcpy(&uartBuffer[len], c, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], adData, strlen(adData));
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Scanning
// *********************************************************************************
// Method available only when the module is set as a Central (GAP) device and is
// ready for scan before establishing connection.
// By default, scan interval of 375 milliseconds and scan window of 250 milliseconds
// Use stopScanning() method to stop an active scan
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startScanning(void)
{
  debugPrintLn("[startScanning]");
  
  sendCommand(START_DEFAULT_SCAN);
  if (expectResponse(SCANNING_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Start Scanning
// *********************************************************************************
// Method available only when the module is set as a Central (GAP) device and is
// ready for scan before establishing connection.
// The user has the option to specify the scan interval and scan window as first 
// and second parameter, respectively. Each unit is 0.625 millisecond. Scan interval
// must be larger or equal to scan window. The scan interval and the scan window
// values can range from 2.5 milliseconds to 10.24 seconds.
// Use stopScanning() method to stop an active scan
// Input : uint16_t scan interval value (must be >= scan window)
//         uint16_t scan window value
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::startScanning(uint16_t scanInterval, uint16_t scanWindow)
{
  debugPrint("[startScanning2]"); //debugPrint(" Interval = " + scanInterval) ; debugPrint(" Window = " + scanWindow) ;

  uint8_t len = strlen(START_CUSTOM_SCAN);
  char c1[4];
  sprintf(c1, "%04X", scanInterval);
  uint8_t newLen = strlen(c1);
  char c2[4];
  sprintf(c2, "%04X", scanWindow);
  
  this->flush();
  memcpy(uartBuffer, START_CUSTOM_SCAN, len);
  memcpy(&uartBuffer[len], c1, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], c2, strlen(c2));
  sendCommand(uartBuffer);
  if (expectResponse(SCANNING_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Stop Scanning
// *********************************************************************************
// Stops scan process started by startScanning() method
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::stopScanning(void)
{
  debugPrintLn("[stopScanning]");
  
  sendCommand(STOP_SCAN);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Add a MAC address to the white list
// *********************************************************************************
// Once one device is added to the white list, the white list feature is enabled.
// With the white list feature enabled, when performing a scan, any device not
// included in the white list does not appear in the scan results.
// As a peripheral, any device not listed in the white list cannot be connected
// with a local device. RN4870/71 supports up to 16 addresses in the white list.
// A random address stored in the white list cannot be resolved. If the peer 
// device does not change the random address, it is valid in the white list. 
// If the random address is changed, this device is no longer considered to be on 
// the white list.
// Input : bool addrType = 0 if following address is public (=1 for private)
//         const char *addr 6-byte address in hex format
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::addMacAddrWhiteList(bool addrType, const char *addr)
{
  debugPrint("[addMacAddrWhiteList] ");

  uint8_t len = strlen(ADD_WHITE_LIST);
  uint8_t addrLen = strlen(addr);
  if (addrLen != MAC_ADDRESS_LEN)
  {
    debugPrintLn("MAC Address is not valid !");
    return false;
  }

  debugPrintLn(addrType);

  whiteListCnt++;
  if (whiteListCnt >= MAX_WHITE_LIST_SIZE)
  {
    whiteListCnt = MAX_WHITE_LIST_SIZE;
    debugPrintLn("White list is full, not possible to add new MAC address.");
    return false;
  }

  // Fill the buffer
  this->flush();
  memcpy(uartBuffer, ADD_WHITE_LIST, len);
  if (addrType)
  {
    memcpy(&uartBuffer[len], PRIVATE_ADDRESS_TYPE, 1);
  }
  else
  {
    memcpy(&uartBuffer[len], PUBLIC_ADDRESS_TYPE, 1);
  }
  len += 1;
  memcpy(&uartBuffer[len], ",", 1);
  len += 1;
  memcpy(&uartBuffer[len], addr, addrLen);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Add all currently bonded devices to the white list
// *********************************************************************************
// The random address in the white list can be resolved with this method for 
// connection purpose. If the peer device changes its resolvable random address, 
// the RN4870/71 is still able to detect that the different random addresses are 
// from the same physical device, therefore, allows connection from such peer 
// device. This feature is particularly useful if the peer device is a iOS or 
// Android device which uses resolvable random.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::addBondedWhiteList(void)
{
  debugPrintLn("[addBondedWhiteList]");
  
  sendCommand(ADD_BONDED_WHITE_LIST);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Clear the white list
// *********************************************************************************
// Once the white list is cleared, white list feature is disabled.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::clearWhiteList(void)
{
  debugPrintLn("[clearWhiteList]");

  whiteListCnt = 0;
  
  sendCommand(CLEAR_WHITE_LIST);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Kill the active connection
// *********************************************************************************
// Disconnect the active BTLE link. It can be used in central or peripheral role.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::killConnection(void)
{
  debugPrintLn("[killConnection]");
  
  sendCommand(KILL_CONNECTION);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Get the RSSI level
// *********************************************************************************
// Get the signal strength in dBm of the last communication with the peer device. 
// The signal strength is used to estimate the distance between the device and its
// remote peer.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::getRSSI(void)
{
  debugPrint("[getRSSI]");

  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;
  sendCommand(GET_RSSI_LEVEL);
  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        return true;
      }
    }
  }
  return false ;
}

// *********************************************************************************
// Reboot the module
// *********************************************************************************
// Forces a complete device reboot (similar to a power cycle).
// After rebooting RN487x, all prior made setting changes takes effect.
// Input : void
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::reboot(void)
{
  debugPrintLn("[reboot]");
  
  sendCommand(REBOOT);
  if (expectResponse(REBOOTING_RESP, RESET_CMD_TIMEOUT))
  {
    delay(RESET_CMD_TIMEOUT); // delay needed
    return true;
  }
  return false;
}

// *********************************************************************************
// Sets the service UUID
// *********************************************************************************
// Sets the UUID of the public or the private service.
// This method must be called before the setCharactUUID() method.
// 
// Input : const char *uuid 
//         can be either a 16-bit UUID for public service
//         or a 128-bit UUID for private service
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setServiceUUID(const char *uuid)
{
  debugPrint("[serServiceUUID]");
  debugPrintLn(uuid);

  uint8_t len = strlen(DEFINE_SERVICE_UUID);
  uint8_t newLen = strlen(uuid);

  if (newLen == 32)
  {
    // 128-bit => private service
    debugPrintLn("128-bit length => private service");
  }
  else if (newLen == 4)
  {
    // 16-bit => public service
    debugPrintLn("16-bit length => public service");
  }
  else
  {
    debugPrintLn("UUID length is not correct");
    return false;
  }

  this->flush();
  memcpy(uartBuffer, DEFINE_SERVICE_UUID, len);
  memcpy(&uartBuffer[len], uuid, newLen);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Sets the characteritics UUID
// *********************************************************************************
// Sets private characteriticsthe UUID of the public or the private service.
// This method must be called after the setServiceUUID() method.
// 
// Input : const char *uuid 
//         can be either a 16-bit UUID for public service
//         or a 128-bit UUID for private service
//         uint8_t property is a 8-bit property bitmap of the characteristics
//         uint8_t octetLen is an 8-bit value that indicates the maximum data size
//         in octet where the value of the characteristics holds in the range
//         from 1 to 20 (0x01 to 0x14)
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setCharactUUID(const char *uuid, uint8_t property, uint8_t octetLen)
{
  debugPrint("[setCharactUUID]");
  debugPrintLn(uuid);

  if (octetLen < 0x01)
  {
    octetLen = 0x01;
    debugPrintLn("Octet Length is out of range (0x01-0x14)");
  }
  else if (octetLen > 0x14)
  {
    octetLen = 0x14;
    debugPrintLn("Octet Length is out of range (0x01-0x14)");
  }

  //uint8_t len = strlen(DEFINE_CHARACT_UUID);
  uint8_t newLen = strlen(uuid);

  if (newLen == PRIVATE_SERVICE_LEN)
  {
    debugPrintLn("128-bit length => private service");
  }
  else if (newLen == PUBLIC_SERVICE_LEN)
  {
    // 16-bit => public service
    debugPrintLn("16-bit length => public service");
  }
  else
  {
    debugPrintLn("UUID length is not correct");
    return false;
  }

  String str = DEFINE_CHARACT_UUID;
  char c[2];
  str.concat(uuid);
  str.concat(',');
  sprintf(c, "%02X", property);
  str.concat(c);
  str.concat(',');
  sprintf(c, "%02X", octetLen);
  str.concat(c);

  sendCommand(str);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Write local characteristic value as server
// *********************************************************************************
// Writes content of characteristic in Server Service to local device by addressing
// its handle
// Input : uint16_t handle which corresponds to the characteristic of the server service
//         const unsigned char value[] is the content to be written to the characteristic
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::writeLocalCharacteristic(uint16_t handle, const char value[])
{
  debugPrint("[writeLocalCharacteristic]");

  uint8_t len = strlen(WRITE_LOCAL_CHARACT);
  char c[4];
  sprintf(c, "%04X", handle);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, WRITE_LOCAL_CHARACT, len);
  memcpy(&uartBuffer[len], c, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], value, strlen(value));
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Read local characteristic value as server
// *********************************************************************************
// Reads the content of the server service characteristic on the local device
// by addressing its handle. 
// This method is effective with or without an active connection.
// Input : uint16_t handle which corresponds to the characteristic of the server service
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::readLocalCharacteristic(uint16_t handle)
{
  debugPrint("[readLocalCharacteristic]");

  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;

  uint8_t len = strlen(READ_LOCAL_CHARACT);
  char c[4];
  sprintf(c, "%04X", handle);
  uint8_t newLen = strlen(c);

  this->flush();
  memcpy(uartBuffer, READ_LOCAL_CHARACT, len);
  memcpy(&uartBuffer[len], c, newLen);
  sendCommand(uartBuffer);
  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        return true;
      }
    }
  }
  return false ;
}

// *********************************************************************************
// Get the current connection status
// *********************************************************************************
// If the RN4870/71 is not connected, the output is none.
// If the RN4870/71 is connected, the buffer must contains the information:
// <Peer BT Address>,<Address Type>,<Connection Type>
// where <Peer BT Address> is the 6-byte hex address of the peer device; 
//       <Address Type> is either 0 for public address or 1 for random address; 
//       <Connection Type> specifies if the connection enables UART Transparent 
// feature, where 1 indicates UART Transparent is enabled and 0 indicates 
// UART Transparent is disabled
// Input : void
// Output: int true if connected, (-1) if timeout occurred, false if not connected
// *********************************************************************************
int Rn487xBle::getConnectionStatus(void)
{
  debugPrint("[getConnectionStatus]");

  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;

  sendCommand(GET_CONNECTION_STATUS);

  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        // Check for the connection
        if (strstr(this->uartBuffer, NONE_RESP) != NULL)
        {
          // Not connected
          return false;
        }
        // Connected
        return true;
      }
    }
  }
  debugPrint("[getConnectionStatus] Timeout without a response !");
  return -1;
}

// ---------------------------------------- Private section ----------------------------------------

// *********************************************************************************
// Configures the Beacon Feature
// *********************************************************************************
// Input : const char *value
// Output: return true if successfully executed
// *********************************************************************************
bool Rn487xBle::setBeaconFeatures(const char *value)
{
  debugPrintLn("[setBeaconFeatures]");

  uint8_t len = strlen(SET_BEACON_FEATURES);
  this->flush();
  memcpy(uartBuffer, SET_BEACON_FEATURES, len);
  memcpy(&uartBuffer[len], value, 1);
  sendCommand(uartBuffer);
  if (expectResponse(AOK_RESP, DEFAULT_CMD_TIMEOUT))
  {
    return true;
  }
  return false;
}

// *********************************************************************************
// Get any settings in module's configuration memory
// *********************************************************************************
// Input : uint16_t memory shift address in non-volatile memory for data
//         uint8_t size of the data to be read, up to 32 (0x20) bytes
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::getSettings(uint16_t addr, uint8_t sizeToRead)
{
  debugPrintLn("[getSettings]");

  if (sizeToRead > MAX_SETTINGS_LEN)
  {
    sizeToRead = MAX_SETTINGS_LEN;
  }

  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;

  uint8_t len = strlen(GET_SETTINGS);
  char c1[4];
  sprintf(c1, "%04X", addr);
  uint8_t newLen = strlen(c1);
  char c2[2];
  sprintf(c2, "%02X", sizeToRead);

  this->flush();
  memcpy(uartBuffer, GET_SETTINGS, len);
  memcpy(&uartBuffer[len], c1, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], c2, strlen(c2));
  sendCommand(uartBuffer);
  
  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        return true;
      }
    }
  }
  return false;
}

// *********************************************************************************
// Change any settings in configuration NVM
// *********************************************************************************
// Input : uint16_t 16-bit hex value that represents memory address of eFlash of 
//          the first byte of data. If more than one byte of data is provided, the
//          memory address automatically increase
//         const char* data value in hex format, up to 32-bytes
// Output: bool true if successfully executed
// *********************************************************************************
bool Rn487xBle::setSettings(uint16_t addr, const char *data)
{
  debugPrintLn("[setSettings]");

  uint8_t dataLen = strlen(data);
  debugPrintLn(data);
  debugPrint("Len = ");
  debugPrintLn(dataLen);
  
  uint16_t timeout = DEFAULT_CMD_TIMEOUT;
  unsigned long previous;

  uint8_t len = strlen(SET_SETTINGS);
  char c1[4];
  sprintf(c1, "%04X", addr);
  uint8_t newLen = strlen(c1);

  this->flush();
  memcpy(uartBuffer, SET_SETTINGS, len);
  memcpy(&uartBuffer[len], c1, newLen);
  memcpy(&uartBuffer[len+newLen], ",", 1);
  memcpy(&uartBuffer[len+newLen+1], data, dataLen);
  sendCommand(uartBuffer);
  
  previous = millis();
  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (this->bleSerial->available() > 0)
    {
      if (readUntilCR() > 0)
      {
        return true;
      }
    }
  }
  return false;
}

// *********************************************************************************
// Flush the uart buffer
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::flush(void)
{
  uartBufferLen = DEFAULT_INPUT_BUFFER_SIZE;
  // FOR DEBUGGING, PRINT OUT ANYTHING THAT'S IN THE BUFFER
//  this->diagStream->print(" * Flushing :");
//  this->diagStream->print(uartBuffer);
//  this->diagStream->println(":");
  
  memset(uartBuffer, 0, uartBufferLen);
}

// *********************************************************************************
// Clean the serial input buffer
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
void Rn487xBle::cleanInputBuffer(void)
{
  while (bleSerial->available() > 0)
  {
    bleSerial->read();
  }
}

// *********************************************************************************
// Set the current operation mode
// *********************************************************************************
// Input : operationMode_t new mode
// Output: void
// *********************************************************************************
void Rn487xBle::setOperationMode(operationMode_t newMode)
{
  operationMode = newMode;
}

// *********************************************************************************
// Get the current operation mode
// *********************************************************************************
// Input : void
// Output: operationMode_t
// *********************************************************************************
operationMode_t Rn487xBle::getOperationMode(void)
{
  return(operationMode);
}


// *********************************************************************************
// Reads a buffer until the carriage return
// *********************************************************************************
// Reads a buffer until the carriage return from the device stream into the "buffer" 
// starting at the "start" position of the buffer.
// Input : char c
//         char *buffer
//         uint16_t size
//         uint16_t start
// Output: uint16_t the number of bytes read
// *********************************************************************************
uint16_t Rn487xBle::readUntilCR(char* buffer, uint16_t size, uint16_t start)
{
  int len = this->bleSerial->readBytesUntil(LF, buffer + start, size);
  return len;
}


// *********************************************************************************
// Reads UART stream, parse the reception buffer
// *********************************************************************************
// Read UART stream - Parse the reception buffer - 
// Input : const char* expectedResponse
//       : uint16_t timeout (ms)
// Output: bool true if expected response match into the received stream
// *********************************************************************************
bool Rn487xBle::expectResponse(const char* expectedResponse, uint16_t timeout)
{
  debugPrint("[expectResponse] expecting ");
  debugPrintLn(expectedResponse);

  unsigned long previous;
  
  memset(uartBuffer, 0, uartBufferLen); // clear the buffer
  cleanInputBuffer();
  previous = millis();

  while (millis() - previous < timeout)
  {
    debugPrint(".");
    if (readUntilCR() > 0)
    {
      debugPrint("(");
      debugPrint(this->uartBuffer);
      debugPrint(")");
      if (strstr(this->uartBuffer, expectedResponse) != NULL)
      {
        debugPrintLn(" found a match !");
        return true;
      }
      return false;
    }
  }
  return false;
}
