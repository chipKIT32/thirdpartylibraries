/*
  EEPROM.h - EEPROM library
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

#ifndef IO_SHIELD_TEMP_h
#define IO_SHIELD_TEMP_h

#include "WProgram.h"
#include "Wire.h"
#include <inttypes.h>

//Device Constants
#define IOShieldTempAddr	0x48

//Configuration settings
#define IOSHIELDTEMP_ONESHOT	0x80 //One Shot mode
#define IOSHIELDTEMP_RES9		0x00 //9-bit resultion
#define IOSHIELDTEMP_RES10		0x20 //10-bit resolution
#define IOSHIELDTEMP_RES11		0x40 //11-bit resolution
#define IOSHIELDTEMP_RES12		0x60 //12-bit resolution
#define IOSHIELDTEMP_FAULT1		0x00 //1 fault queue bits
#define IOSHIELDTEMP_FAULT2		0x08 //2 fault queue bits
#define IOSHIELDTEMP_FAULT4		0x10 //4 fault queue bits
#define IOSHIELDTEMP_FAULT6		0x18 //6 fault queue bits
#define IOSHIELDTEMP_ALERTLOW	0x00 //Alert bit active-low
#define IOSHIELDTEMP_ALERTHIGH	0x04 //Alert bit active-high
#define IOSHIELDTEMP_CMPMODE	0x00 //comparator mode
#define IOSHIELDTEMP_INTMODE	0x02 //interrupt mode
#define IOSHIELDTEMP_SHUTDOWN	0x01 //Shutdown enabled
#define	IOSHIELDTEMP_STARTUP	0x00 //Shutdown Disabled
						 //Default Startup Configuration Used, this is just so the device can be
						 //reset to startup configurations at a later time, it doesn't need to be
						 //called anywhere.
#define IOSHIELDTEMP_CONF_DEFAULT	(IOSHIELDTEMP_RES9 | IOSHIELDTEMP_FAULT1 | IOSHIELDTEMP_ALERTLOW | IOSHIELDTEMP_CMPMODE)
#define	IOSHIELDTEMP_MIN		-128 //Minimum input tempreture for the Hyst/Limit registers
#define	IOSHIELDTEMP_MAX		127.5 //Maximum input tempreture for the Hyst/Limit registers

class IOShieldTempClass
{
  private:
	static uint8_t	lastConfig;
	static void setTempHystPriv(uint16_t);
	static void setTempLimitPriv(uint16_t);
  public:
	IOShieldTempClass();
	void config(uint8_t);
    float getTemp();
	void setTempHyst(float);
	void setTempLimit(float);
	float convFtoC(float);
	float convCtoF(float);
};

extern IOShieldTempClass IOShieldTemp;

#endif