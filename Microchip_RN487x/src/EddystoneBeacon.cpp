// EddystoneBeacon.cpp
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

#include "EddystoneBeacon.h"
#include <stdio.h>
#include <stdlib.h>

// Explanation of the Eddystone encoding:
// https://github.com/google/eddystone/tree/master/eddystone-url

// Tx Power level is a signed 8-bit integer
// examples: the value 0x12 is interpreted as +18dBm
//           the value 0xEE is interpreted as -18dBm

// To encoded URL (size from 1-17): use Google URL shortener
// https://goo.gl/
// example: http://www.microchip.com becomes https://goo.gl/YFeJ8


// ---------------------------------------- Private section ----------------------------------------
EddystoneBeacon eddystoneBeacon ;

#define MAX_URI_SIZE 17

static const char* URL_PREFIX[] = {
  "http://www.",  // 0x00
  "https://www.",
  "http://",
  "https://"      // 0x03
} ;

static const char* URL_SUFFIX[] = {
  ".com/",        // 0x00
  ".org/",
  ".edu/",
  ".net/",
  ".info/",
  ".biz/",
  ".gov/",
  ".com",
  ".org",
  ".edu",
  ".net",
  ".info",
  ".biz",
  ".gov"          // 0x0D
} ;

// *********************************************************************************
// Constructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
EddystoneBeacon::EddystoneBeacon()
{

}

// *********************************************************************************
// Destructor
// *********************************************************************************
// Input : void
// Output: void
// *********************************************************************************
EddystoneBeacon::~EddystoneBeacon()
{

}

// *********************************************************************************
// Start Eddystone Beacon with UUID packet type
// *********************************************************************************
// Input :  uint8_t txPower [1] level in 2's complement, indicating the signal
//          strength one meter from the device. This value must be calibrated for
//          each device by the user or manufacturer
//          const char* uuid [16Bytes] contains namespace ID (10) + instance ID (6)
// Output: void
// *********************************************************************************
void EddystoneBeacon::beginUUID(uint8_t txPower, const char* uuid)
{
  uint8_t len = strlen(eddystoneUUID) ;
  char c[2] ;

  memset(data, 0, strlen(data)) ;
  // Add Prefix
  memcpy(data, eddystoneUUID, len) ;
  // Add UUID type of frame
  memcpy(&data[len], uuidFrame, strlen(uuidFrame)) ;
  len += strlen(uuidFrame) ;
  // Add Tx Power Measured
  sprintf(c, "%02X", txPower) ;
  memcpy(&data[len], c, strlen(c)) ;
  len += strlen(c) ;
  // Add UUID (16B) = Namespace ID (10) + Instance ID (6)
  memcpy(&data[len], uuid, strlen(uuid)) ;
  len += strlen(uuid) ;
  // Last 2x 2-bytes are reserved for future use and must be 0
  sprintf(c, "%02X", '0') ;
  memcpy(&data[len], c, strlen(c)) ;
  len += strlen(c) ;
  memcpy(&data[len], c, strlen(c)) ;

  // Start to transmit the beacon packet
  // Add Eddstyone service to a new frame
  rn487xBle.startImmediateBeacon(AD_TYPE_COMPLETE_16_UUID, eddystoneUUID) ;
  // Add the rest of the frame
  rn487xBle.startImmediateBeacon(AD_TYPE_SERVICE_DATA, data) ;
}

// *********************************************************************************
// Start Eddystone Beacon with URI packet type
// *********************************************************************************
// Input :  uint8_t txPower [1] level in 2's complement, indicating the signal
//          strength one meter from the device. This value must be calibrated for
//          each device by the user or manufacturer
//          const char* uri [17Bytes) contains encoded URI
// Output: void
// *********************************************************************************
void EddystoneBeacon::beginURI(uint8_t txPower, const char* uri)
{
  uint8_t len = strlen(eddystoneUUID) ;
  char c[2] ;

  memset(data, 0, strlen(data)) ;
  // Add Prefix
  memcpy(data, eddystoneUUID, len) ;
  // Add URI type of frame
  memcpy(&data[len], uriFrame, strlen(uuidFrame)) ;
  len += strlen(uuidFrame) ;
  // Add Tx Power Measured
  sprintf(c, "%02X", txPower) ;
  memcpy(&data[len], c, strlen(c)) ;
  len += strlen(c) ;

  // URI Scheme
  String uriString = uri ;
  uint8_t prefixType = 0 ;
  uint8_t suffixType = 0 ;
  // Find and remove prefix
  for (uint8_t i = 0; i < (sizeof(URL_PREFIX) / sizeof(char *)); i++) 
  {
    if (uriString.indexOf(URL_PREFIX[i]) >= 0)
    {
        uriString.replace(URL_PREFIX[i], "") ;
        prefixType = i ;
    }
  }
  // Add URL scheme prefix encoded
  sprintf(c, "%02X", prefixType) ;
  memcpy(&data[len], c, strlen(c)) ;
  len += strlen(c) ;

  // Find an remove suffix if any (URL encoded with Google Shortener has no suffix: https://goo.gl/YFeJ8)
  bool suffixFlag = false ;
  for (uint8_t i = 0; i < (sizeof(URL_SUFFIX) / sizeof(char *)); i++) 
  {
    if (uriString.indexOf(URL_SUFFIX[i]) >= 0 && uriString.endsWith(URL_SUFFIX[i]))
    {
      suffixFlag = true ; // suffix here
      uriString.replace(URL_SUFFIX[i], "") ;
      suffixType = i ;
    }
  }
  // Add suffix at the end if suffixFlag is true

  // Parse the compressed URI and convert ASCII to HEX
  uint8_t uriLen = uriString.length() ;
  if (uriLen > MAX_URI_SIZE)
  {
    // Limit the len
    uriLen = MAX_URI_SIZE ;
    // Limit the string as well
    uriString = uriString.substring(0, uriLen) ;
  }

  // Encoded buffer size must be 2x times bigger than the URI buffer
  uriLen *= 2 ; 
  char uriEncoded[uriLen] ;
  for (uint8_t i = 0; i < uriString.length(); i++)
  {
    sprintf(uriEncoded+i*2, "%02X", uriString[i]) ;
    //if (uriString[i] == '\0') break ;
  }

  // Add encoded URL
  memcpy(&data[len], uriEncoded, uriLen) ;
  len += uriLen ;

  // (if any) Add encoded termination char corresponding to the end of the URI like .com, etc.
  if (suffixFlag)
  {
    sprintf(c, "%02X", suffixType) ;
    memcpy(&data[len], c, strlen(c)) ;
    len += strlen(c) ;
  }
 
  // Add Eddstyone service to a new frame
  rn487xBle.startImmediateBeacon(AD_TYPE_COMPLETE_16_UUID, eddystoneUUID) ;
  // Add the rest of the frame
  rn487xBle.startImmediateBeacon(AD_TYPE_SERVICE_DATA, data) ;
}


