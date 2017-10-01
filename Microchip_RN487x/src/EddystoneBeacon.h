// EddystoneBeacon.h
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

#ifndef __EDDYSTONEBEACON
#define __EDDYSTONEBEACON

#include "RN487x_BLE.h"

#define EDDYSTONE_FRAME_SIZE  50

class EddystoneBeacon : public Rn487xBle
{
// ---------------------------------------- Public section ----------------------------------------
public:
  EddystoneBeacon() ;
  virtual ~EddystoneBeacon() ;
  void beginUUID(uint8_t txPower, const char* uuid) ;
  void beginURI(uint8_t txPower, const char* uri) ;
  
// --------------------------------------- Protected section ---------------------------------------
protected:


// ---------------------------------------- Private section ----------------------------------------
private:
  unsigned char compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize) ;

  char data[EDDYSTONE_FRAME_SIZE] ;
  const char* eddystoneUUID = "AAFE" ;
  const char* uuidFrame = "00" ;
  const char* uriFrame = "10" ;
} ;

// external variable used by the sketch
extern EddystoneBeacon eddystoneBeacon ;



#endif
