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

#ifndef IO_SHIELD_EEPROM_h
#define IO_SHIELD_EEPROM_h

#include "WProgram.h"
#include "Wire.h"
#include <inttypes.h>

#define IOShieldEEPROMAddr 0x50

class IOShieldEEPROMClass
{
  public:
	IOShieldEEPROMClass();
    uint8_t read(uint16_t address);
	void readString(uint16_t address, char *sz, int size);
	void readString(uint16_t address, uint8_t *sz, int size);
    void write(uint16_t address, uint8_t data);
	void writeString(uint16_t address, char *data, int size);
	void writeString(uint16_t address, char *data);
	void writeString(uint16_t address, uint8_t *data, int size);
};

extern IOShieldEEPROMClass IOShieldEEPROM;
#endif