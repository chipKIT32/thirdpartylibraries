/*
  IOShieldEEPROM.cpp - IO Shield Library : EEPROM Module
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
#include "IOShieldEEPROM.h"

IOShieldEEPROMClass::IOShieldEEPROMClass()
{
	#if defined(_BOARD_MEGA_)
		PORTSetPinsDigitalIn(IOPORT_B, BIT_4 | BIT_5);
	#endif
	Wire.begin();
}

/* ------------------------------------------------------------ */
/***	uint8_t IOShieldEEPROMClass::read(uint16_t address)
**
**	Parameters:
**		address		- 16 Bit address designating were to read
**
**	Return Value:
**		byte
**
**	Errors:
**		none
**
**	Description:
**		Splits the 16 bit address into two bytes and sends them
**		to the EEPROM device. Next it reads from the EEPROM device
**		to get the data at that location and returns a byte
**
*/
uint8_t IOShieldEEPROMClass::read(uint16_t address)
{
	uint8_t temp;
	readString(address, &temp, 1);
	return temp;
}

/* ------------------------------------------------------------ */
/***	void IOShieldEEPROMClass::readString(uint16_t address, uint8_t *sz, int size)
**
**	Parameters:
**		address		- 16 Bit address designating were to read
**		sz			- pointer to string of Bytes
**		size		- amount of data to be read
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Splits the 16 bit address into two bytes and sends them
**		to the EEPROM device. Next it reads consecutive bytes starting
**		from the given address and stores them in the location provided
**
*/
void IOShieldEEPROMClass::readString(uint16_t address, char *sz, int size)
{
	readString(address, (uint8_t*)sz, size);
}

void IOShieldEEPROMClass::readString(uint16_t address, uint8_t *sz, int size)
{
	uint8_t temp[2] = {0,0};
	int index = 0;
	temp[0] = (address >> 8);
	temp[1] = (address & 0xFF);
	
	Wire.beginTransmission(IOShieldEEPROMAddr);
	Wire.write(temp, 2);
	Wire.endTransmission();
	
	Wire.requestFrom(IOShieldEEPROMAddr, size);
	while(Wire.available())
	{
		sz[index++] = Wire.receive();
	}
	//twi_writeTo(IOShieldEEPROMAddr, temp, 2, 1);
	//twi_readFrom(IOShieldEEPROMAddr, sz, size);
}

/* ------------------------------------------------------------ */
/***	void IOShieldEEPROMClass::write(uint16_t address, uint8_t data)
**
**	Parameters:
**		address		- 16 Bit address designating were to write
**		data		- Data to be written
**
**	Return Value:
**
**	Errors:
**
**	Description:
**		Splits the 16 bit address into two bytes and sends them
**		to the EEPROM device. Then writes a byte to specified address
**
*/
void IOShieldEEPROMClass::write(uint16_t address, uint8_t data)
{
	writeString(address, &data, 1);
}

/* ------------------------------------------------------------ */
/***	void IOShieldEEPROMClass::writeString(uint16_t address, uint8_t *data, int size)
**
**	Parameters:
**		address		- 16 Bit address designating were to write
**		data		- Data to be written
**		size		- Bytes to be written
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Splits the 16 bit address into two bytes and sends them
**		to the EEPROM device. Then writes a string of bytes to 
**		specified address if amount of bytes is above 64 the data
**		will automaticly be truncated.
**
*/
void IOShieldEEPROMClass::writeString(uint16_t address, char *data, int size)
{
	writeString(address, (uint8_t*)data, size);
}

void IOShieldEEPROMClass::writeString(uint16_t address, char *data)
{
	writeString(address, data, strlen(data));
}

void IOShieldEEPROMClass::writeString(uint16_t address, uint8_t *data, int size)
{
	int i;
	uint8_t temp[67] = {0,0,0};
	temp[0] = (address >> 8);
	temp[1] = (address & 0xFF);

	if(size > 64)
	{
		size = 64;
	}

	for(i=0; i<size; i++)
	{
		temp[i+2] = *data;
		data++;
	}

	Wire.beginTransmission(IOShieldEEPROMAddr);
	Wire.write(temp, (size + 2));
	Wire.endTransmission();
	
	//twi_writeTo(IOShieldEEPROMAddr, temp, size+2, 1);
	delay(10);
}

IOShieldEEPROMClass IOShieldEEPROM = IOShieldEEPROMClass();