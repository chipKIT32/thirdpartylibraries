// RN487x_BLE.h
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

/* BPS TODO:
 * add support for Scan Response commands 
 * add support for script commands (like stop and clear)
 * Bug: Init can fail if previous use of RN module turned off CMD> prompt. How to fix?
 * Bug: Init sends two "R,1"s. Why? Only one is needed.
 * Add support for Set Connection Parameters ("ST")
 * Add support for Modify Current Connection Parameters ("T")
 * Add support for Start Custom Advertisement (START_CUSTOM_ADV - "A,")
 * BIG ONE: Add support for async notifications - like when the phone updates a local 
    server characteristic, and we get a "%WC," or "%WC," string from BLE module. Maybe 
    have a callback mechanism for this?
 */


#ifndef __RN487x_BLE
#define __RN487x_BLE

#include "RN487x_CONST.h"
#include <string.h>
#include <Stream.h>
#if !defined(__PIC32__)
  #include <stdbool.h>
#endif
#include <Arduino.h>

#define DEFAULT_INPUT_BUFFER_SIZE     100
#define DEFAULT_BAUDRATE              115200

typedef enum {
  dataMode,
  cmdMode
} operationMode_t;

class Rn487xBle
{

// ---------------------------------------- Public section ----------------------------------------
public:
  Rn487xBle();
  virtual ~Rn487xBle();
  uint32_t getDefaultBaudRate(void) { return(DEFAULT_BAUDRATE);};
  void setDiag(Stream& stream) { diagStream = &stream;};
  inline void initBleStream(HardwareSerial *stream) { this->bleSerial = stream ; cleanInputBuffer();};
  bool swInit(void);
  void hwInit(void);
  void hwReset(void);
  void hwWakeUp(void);
  void hwSleep(void);
  bool enterDataMode(void);
  bool enterCommandMode(void);
  void sendCommand(String stream);
  void sendData(char *data, uint16_t dataLen);
  bool disableBeacon(void);
  bool enableBeacon(void);
  bool enableBeaconAndAdv(void);
  bool factoryReset(void);
  bool setAdvPower(uint8_t value);
  bool setConPower(uint8_t value);
  bool setSerializedName(const char *newName);
  bool setDevName(const char *newName);
  bool enableLowPower(void);
  bool disableLowPower(void);
  bool setSupportedFeatures(uint16_t bitmap);
  bool setDefaultServices(uint8_t bitmap);
  bool retrieveBtAddress(void);
  bool getFirmwareVersion(void);
  bool clearAllServices(void);
  bool startAdvertising(void);
  bool startCustomAdvertising(uint16_t interval);
  bool startCustomAdvertising(uint16_t interval, uint16_t totalAdTime);
  bool stopAdvertising(void);
  bool clearImmediateAdvertising(void);
  bool clearPermanentAdvertising(void);
  bool clearImmediateBeacon(void);
  bool clearPermanentBeacon(void);
  bool startImmediateAdvertising(uint8_t adType, const char adData[]);
  bool startPermanentAdvertising(uint8_t adType, const char adData[]);
  bool startImmediateBeacon(uint8_t adType, const char adData[]);
  bool startPermanentBeacon(uint8_t adType, const char adData[]);
  bool startScanning(void);
  bool startScanning(uint16_t scanInterval, uint16_t scanWindow);
  bool stopScanning(void);
  bool addMacAddrWhiteList(bool addrType, const char *addr);
  bool addBondedWhiteList(void);
  bool clearWhiteList(void);
  bool killConnection(void);
  bool getRSSI(void);
  bool reboot(void);
  bool setServiceUUID(const char *uuid);
  bool setCharactUUID(const char *uuid, uint8_t property, uint8_t octetLen);
  bool writeLocalCharacteristic(uint16_t handle, const char value[]);
  bool readLocalCharacteristic(uint16_t handle);
  int getConnectionStatus(void);
  inline const char* getPeerAddress() { return peerAddress;};
  inline const char* getBtAddress()   { return btAddress;};
  inline const char* getDeviceName()  { return deviceName;};
  inline const char* getLastResponse() { return uartBuffer;};
  inline bool isInputBuffer()
  {
    if (bleSerial->available() > 0) return true;
    return false;
  }
  inline char getInputBuffer(){ return bleSerial->read();};
  
// --------------------------------------- Protected section ---------------------------------------
protected:


// ---------------------------------------- Private section ----------------------------------------
private:
  bool setBeaconFeatures(const char *value);
  bool getSettings(uint16_t addr, uint8_t sizeToRead);
  bool setSettings(uint16_t addr, const char *data);
  void flush(void);
  void cleanInputBuffer(void);
  void setOperationMode(operationMode_t newMode);
  operationMode_t getOperationMode(void);
  uint16_t readUntilCR(char* buffer, uint16_t size, uint16_t start = 0);
  uint16_t readUntilCR() { return readUntilCR(this->uartBuffer, this->uartBufferLen);};
  bool expectResponse(const char* expectedResponse, uint16_t timeout);

  HardwareSerial *bleSerial;
  Stream* diagStream;
  char *uartBuffer;
  int uartBufferLen;
  char endStreamChar;
  operationMode_t operationMode;
  bool connectionStatus;
  char btAddress[12];
  char peerAddress[6];
  char deviceName[MAX_DEVICE_NAME_LEN];
  uint8_t whiteListCnt;
  bool commandPromptEnabled;
} ;

// external variable used by the sketch
extern Rn487xBle rn487xBle;

#endif
