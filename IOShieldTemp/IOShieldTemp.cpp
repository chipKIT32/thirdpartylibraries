/*
  IOShieldTemp.cpp - IO Shield Library : Temp Module
  Copyright (c) 2011 Digilent.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include <Arduino.h>
#include "IOShieldTemp.h"

uint8_t IOShieldTempClass::lastConfig = IOSHIELDTEMP_CONF_DEFAULT;

IOShieldTempClass::IOShieldTempClass()
{
	#if defined (_BOARD_MEGA_)
		PORTSetPinsDigitalIn(IOPORT_B, BIT_4 | BIT_5);
	#endif
	Wire.begin();
}

/* ------------------------------------------------------------ */
/***	void IOShieldTempClass::config(uint8_t configuration)
**		void IOShieldTempClass::config(int configuration)
**
**	Parameters:
**		configuration	- One byte predefined configuration setting
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Configures temperature sensor, some predefined register
**		settings are defined below and in "IOShieldTemp.h"
**
**		//Configuration settings
**		IOSHIELDTEMP_ONESHOT	0x80 //One Shot mode
**		IOSHIELDTEMP_RES9		0x00 //9-bit resultion
**		IOSHIELDTEMP_RES10		0x20 //10-bit resolution
**		IOSHIELDTEMP_RES11		0x40 //11-bit resolution
**		IOSHIELDTEMP_RES12		0x60 //12-bit resolution
**		IOSHIELDTEMP_FAULT1		0x00 //1 fault queue bits
**		IOSHIELDTEMP_FAULT2		0x08 //2 fault queue bits
**		IOSHIELDTEMP_FAULT4		0x10 //4 fault queue bits
**		IOSHIELDTEMP_FAULT6		0x18 //6 fault queue bits
**		IOSHIELDTEMP_ALERTLOW	0x00 //Alert bit active-low
**		IOSHIELDTEMP_ALERTHIGH	0x04 //Alert bit active-high
**		IOSHIELDTEMP_CMPMODE	0x00 //comparator mode
**		IOSHIELDTEMP_INTMODE	0x02 //interrupt mode
**		IOSHIELDTEMP_SHUTDOWN	0x01 //Shutdown enabled
**		IOSHIELDTEMP_STARTUP	0x00 //Shutdown Disabled
**									 //Default Startup Configuration Used
**		IOSHIELDTEMP_CONF_DEFAULT	(RES9 | FAULT1 | ALERTLOW | CMPMODE)
**
*/
void IOShieldTempClass::config(uint8_t configuration)
{
	uint8_t temp[2] = {0};
	temp[0] = 0x01;
	temp[1] = configuration;
	Wire.beginTransmission(IOShieldTempAddr);
	Wire.write(temp, 2);
	Wire.endTransmission();
}

/* ------------------------------------------------------------ */
/***	float IOSheidlTempClass::getTemp()
**
**	Parameters:
**		useC	- Convert the result to C or F. If undefined default to C.
**
**	Return Value:
**		float	- The resulted scaled tempreture data from the
**					device.
**
**	Errors:
**		none
**
**	Description:
**		Retrieves data from temp sensor and returns the value
**		to the user as a floating point value. If the user does
**		not specify then we will return as Celcius, otherwise
**		we may convert it to Fahrenheit if needed.
**
*/
float IOShieldTempClass::getTemp()
{
	float	tempResult;
	int16_t	wResult = 0;
	uint8_t temp[2] = {0};
	int index = 0;
	
	Wire.beginTransmission(IOShieldTempAddr);
	Wire.write(temp, 1);
	Wire.endTransmission();
	
	Wire.requestFrom(IOShieldTempAddr, 2);
	while(Wire.available())
	{
		temp[index++] = Wire.read();
	}
	
	// Convert the result
	wResult = temp[1] | (temp[0] << 8);
	tempResult = wResult/256.0;
	
	return tempResult;
}

/* ------------------------------------------------------------ */
/***	void IOShieldTempClass::setTempHystPriv(uint16_t tMin)
**
**	Parameters:
**		tMin		- 16 bit value representing the hysterises tempreture
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets the mimimum temperature limit such that if the ambient 
**		temp drops below this level the alert output will be
**		asserted (if interrupt mode is on)
**
**		The data is read in two's compliment format while only
**		using the 9 most significant bits the rest being dont
**		cares
**		Upper Byte
**		  15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 |
**		 Sign|2^6 |2^5 |2^4 |2^3 |2^2 |2^1|2^0|
**		Lower Byte
**		  7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
**		 2^-1| 0 | 0 | 0 | 0 | 0 | 0 | 0 |	
**
*/
void IOShieldTempClass::setTempHystPriv(uint16_t tMin)
{
	uint8_t temp[3] = {0};
	temp[0] = 0x02;
	temp[1] = (tMin >> 8);
	temp[2] = (tMin & 0xFF);
	Wire.beginTransmission(IOShieldTempAddr);
	Wire.write(temp, 3);
	Wire.endTransmission();
	//twi_writeTo(IOShieldTempAddr, temp, 3, 1);
}

/* ------------------------------------------------------------ */
/***	void IOShieldTempClass::setTempHyst(float tMin)
**
**	Parameters:
**		tMin		- The human readable value representing minimum
**						temperature.
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		These two routines provide a pair of wrappers for the less
**		Arduino friendly uint16_t type, masking them by the expected
**		integer and float data types that the Arduino user expects.
**
**		This API also provides a constant values IOSHIELDTEMP_MIN and
**		IOSHIELDTEMP_MAX defining the maximum ranges for this register.
**
*/
void IOShieldTempClass::setTempHyst(float tMin)
{
	// An input of 1 would be strait 0x0001, but to
	// convey 1 in the register it should be 0x0100.
	// so multiply by 0x100;
	float		tMinScaled	= tMin*0x100;
	int16_t		tMinInt		= (int16_t)tMinScaled;
	uint16_t	tMinUint	= 0;
	for(int ib = 0; ib < 16; ib++) {
		tMinUint |= (tMinInt & (1 << ib));
	}
	
	setTempHystPriv(tMinUint);
}
/* ------------------------------------------------------------ */
/***	void IOShieldTempClass::setTempLimitPriv(uint16_t tMax)
**
**	Parameters:
**		tMax		- 16 bit value representing maximum temp
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets the maximum temperature limit such that if the ambient 
**		temp exceeds this level the alert output will be
**		asserted (if interrupt mode is on)
**
**		The data is read in two's compliment format while only
**		using the 9 most significant bits the rest being dont
**		cares
**		Upper Byte
**		  15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 |
**		 Sign|2^6 |2^5 |2^4 |2^3 |2^2 |2^1|2^0|
**		Lower Byte
**		  7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
**		 2^-1| 0 | 0 | 0 | 0 | 0 | 0 | 0 |	
**
*/
void IOShieldTempClass::setTempLimitPriv(uint16_t tMax)
{
	uint8_t temp[3] = {0};
	temp[0] = 0x03;
	temp[1] = (tMax >> 8);
	temp[2] = (tMax & 0xFF);
	Wire.beginTransmission(IOShieldTempAddr);
	Wire.write(temp, 3);
	Wire.endTransmission();
	//twi_writeTo(IOShieldTempAddr, temp, 3, 1);
}

/* ------------------------------------------------------------ */
/***	void IOShieldTempClass::setTempLimit(float tMax)
**
**	Parameters:
**		tMax		- The human readable value representing maximum
**						temperature.
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		These two routines provide a pair of wrappers for the less
**		Arduino friendly uint16_t type, masking them by the expected
**		integer and float data types that the Arduino user expects.
**
**		This API also provides a constant values IOSHIELDTEMP_MIN and
**		IOSHIELDTEMP_MAX defining the maximum ranges for this register.
**
*/
void IOShieldTempClass::setTempLimit(float tMax)
{
	// An input of 1 would be strait 0x0001, but to
	// convey 1 in the register it should be 0x0100.
	// so multiply by 0x100;
	float		tMaxScaled	= tMax*0x100;
	int16_t		tMaxInt		= (int16_t)tMaxScaled;
	uint16_t	tMaxUint	= 0;
	for(int ib = 7; ib < 16; ib++) {
		tMaxUint |= (tMaxInt & (1 << ib));
	}
	
	setTempLimitPriv(tMaxUint);
}

/* ------------------------------------------------------------ */
/***	float IOShieldTempClass::convFtoC(float tempF)
**
**	Parameters:
**		tempF		- A fahrenheit tempreture value to be converted
**
**	Return Value:
**		float		- The converted tempreture to Celcius
**
**	Errors:
**		none
**
**	Description:
**		This is a simple convienence method for converting between
**		Celcius tempreture values and Ferenheight tempreture values.
**
*/
float IOShieldTempClass::convFtoC(float tempF)
{
	return ((tempF-32.0)*5.0/9.0);
}

/* ------------------------------------------------------------ */
/***	float IOShieldTempClass::convCtoF(float tempC)
**
**	Parameters:
**		tempC		- A Celcius tempreture value to be converted
**
**	Return Value:
**		float		- The converted tempreture to Fahrenheit
**
**	Errors:
**		none
**
**	Description:
**		This is a simple convienence method for converting between
**		Celcius tempreture values and Ferenheight tempreture values.
**
*/
float IOShieldTempClass::convCtoF(float tempC)
{
	return (tempC*9.0/5.0+32.0);
}
	
/* Preinstantiate Objects */
IOShieldTempClass IOShieldTemp = IOShieldTempClass();
