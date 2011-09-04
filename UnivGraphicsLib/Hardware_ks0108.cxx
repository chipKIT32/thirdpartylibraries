//*******************************************************************************
//*	
//*	ks0108 Arduino drivers
//*	
//*	(C) by Mark Sproul
//*	This code is derived from other works. Credit is given below
//*	This is open source like the rest of the arduino source code
//*
//*	this code is derived from the ks0108.cpp Arduino library
//*
//*******************************************************************************
//*		ks0108.cpp - Arduino library support for ks0108 and compatable graphic LCDs
//*		Copyright (c)2008 Michael Margolis All right reserved
//*		mailto:memargolis@hotmail.com?subject=KS0108_Library 
//*	
//*		The high level functions of this library are based on version 1.1 of ks0108 graphics routines
//*		written and copyright by Fabian Maximilian Thiele. His sitelink is dead but
//*		you can obtain a copy of his original work here:
//*		http://www.scienceprog.com/wp-content/uploads/2007/07/glcd_ks0108.zip
//*	
//*		Code changes include conversion to an Arduino C++ library, rewriting the low level routines 
//*		to read busy status flag and support a wider range of displays, adding more flexibility
//*		in port addressing and improvements in I/O speed. The interface has been made more Arduino friendly
//*		and some convenience functions added. 
//*	
//*		This library is distributed in the hope that it will be useful,
//*		but WITHOUT ANY WARRANTY; without even the implied warranty of
//*		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//*	
//*		Version:	1.0 - May 8	2008	- first release
//*		Version:	1.1 - Nov 7	2008	- restructured low level code to adapt to panel speed
//*									- moved chip and panel configuration into seperate header files
//*									- added fixed width system font
//*		Version:	2	- May 26 2009	- second release
//*									- added support for Mega and Sanguino, improved panel speed tolerance, added bitmap support
//*	
//*******************************************************************************
//*	<MLS> is Mark Sproul msproul -at- jove.rutgers.edu
//*			http://coewww.rutgers.edu/~msproul/
//*******************************************************************************
//*	Detailed edit history
//*******************************************************************************
//*	Jan 28,	2010	<MLS> Started on UnivGraphicsLib
//*	Jun 20,	2011	<MLS> UGL working on KS0108 board on mega
//*******************************************************************************

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>

#if defined(__AVR__)
	#include	<avr/pgmspace.h>
#endif


#include	"HardwareSerial.h"

#include	"UnivGraphicsHW_Defs.h"
#include	"UnivGraphicsLib.h"
#include	"UnivGraphicsHardware.h"	//*	this is the routines that the implementer provides

#ifdef _GRAPHICS_HW_KS0108_


//*	this slows down the drawing so you can debug things.
//#define	GLCD_DEBUG

#define		ksSOURCE
#include	"ks0108.h"

//*******************************************************************************
//*	private globals for ks0108
static	lcdCoord		gCoord;
static	boolean			gInverted; 


#pragma mark -
#pragma mark ks0108 private routines

#ifndef __MWERKS__
	#define EN_DELAY() asm volatile( "ldi r24, %0 \n\t" "subi r24, 0x1 \n\t" "and r24, r24 \n\t" "brne .-6 \n\t" ::"M" (EN_DELAY_VALUE) : "r24" )
#endif

static void KS0108_WriteCommand(uint8_t cmd, uint8_t chip);
void KS0108_WriteData(uint8_t data);



//***********************************************************************************
__inline__ void KS0108_SelectChip(uint8_t chip)
{
//static uint8_t prevchip; 

	if (gChipSelect[chip] & 1)
	{
		fastWriteHigh(CSEL1);
	}
	else
	{
		fastWriteLow(CSEL1);
	}
	if (gChipSelect[chip] & 2)
	{
		fastWriteHigh(CSEL2);
	}
	else
	{
		fastWriteLow(CSEL2);
	}
}



//***********************************************************************************
void KS0108_WaitReady( uint8_t chip)
{
	// wait until LCD busy bit goes to zero

	KS0108_SelectChip(chip);
	lcdDataDir(0x00);
	fastWriteLow(D_I);	
	fastWriteHigh(R_W);	
	fastWriteHigh(EN); 	
	EN_DELAY();
	while(LCD_DATA_IN_HIGH & LCD_BUSY_FLAG)
	{
		;	//do nothin
	}
	fastWriteLow(EN); 	

}

//***********************************************************************************
 __inline__ void KS0108_Enable(void)
 {
	EN_DELAY();
	fastWriteHigh(EN);					// EN high level width min 450 ns
	EN_DELAY();
	fastWriteLow(EN);
	//EN_DELAY(); // some displays may need this delay at the end of the enable pulse
}

//***********************************************************************************
void KS0108_GotoXY(uint8_t x, uint8_t y)
{
uint8_t	chip, cmd;
	
	if ( (x > DISPLAY_WIDTH-1) || (y > DISPLAY_HEIGHT-1) )	// exit if coordinates are not legal
		return;
	gCoord.x	=	x;								//	save new coordinates
	gCoord.y	=	y;
	
	if (y/8 != gCoord.page)
	{
		gCoord.page	=	y/8;
		cmd			=	LCD_SET_PAGE | gCoord.page;			// set y address on all chips	
		for (chip=0; chip < DISPLAY_WIDTH/CHIP_WIDTH; chip++)
		{
			KS0108_WriteCommand(cmd, chip);	
		}
	}
	chip	=	gCoord.x/CHIP_WIDTH;
	x		=	x % CHIP_WIDTH;
	cmd		=	LCD_SET_ADD | x;
	KS0108_WriteCommand(cmd, chip);					// set x address on active chip	
}


//***********************************************************************************
uint8_t KS0108_DoReadData(uint8_t first)
{
	uint8_t data, chip;

	chip	=	gCoord.x / CHIP_WIDTH;
	KS0108_WaitReady(chip);
	if (first)
	{
		if (gCoord.x % CHIP_WIDTH == 0 && chip > 0)
		{ 		// todo , remove this test and call GotoXY always?
			KS0108_GotoXY(gCoord.x, gCoord.y);
			KS0108_WaitReady(chip);
		}
	}	
	fastWriteHigh(D_I);					// D/I = 1
	fastWriteHigh(R_W);					// R/W = 1
	
	fastWriteHigh(EN); 					// EN high level width: min. 450ns
 	EN_DELAY();

#ifdef LCD_DATA_NIBBLES
	data	=	(LCD_DATA_IN_LOW & 0x0F) | (LCD_DATA_IN_HIGH & 0xF0);
#else
	data	=	LCD_DATA_IN_LOW;				// low and high nibbles on same port so read all 8 bits at once
#endif 
	fastWriteLow(EN); 
	if (first == 0) 
	{
		KS0108_GotoXY(gCoord.x, gCoord.y);	
	}
	if (gInverted)
	{
		data	=	~data;
	}
	return data;
}

//***********************************************************************************
inline uint8_t KS0108_ReadData(void)
{
	KS0108_DoReadData(1);				// dummy read

	return KS0108_DoReadData(0);		// "real" read

	gCoord.x++;
}

//***********************************************************************************
static void KS0108_WriteCommand(uint8_t cmd, uint8_t chip)
{
	if (gCoord.x % CHIP_WIDTH == 0 && chip > 0)
	{ 		// todo , ignore address 0???
 		EN_DELAY();
	}
	KS0108_WaitReady(chip);
	fastWriteLow(D_I);					// D/I = 0
	fastWriteLow(R_W);					// R/W = 0	
	lcdDataDir(0xFF);

	EN_DELAY();
	lcdDataOut(cmd);
	KS0108_Enable();					// enable
	EN_DELAY();
	EN_DELAY();
	lcdDataOut(0x00);
}

//***********************************************************************************
void KS0108_WriteData(uint8_t data)
{
	uint8_t displayData, yOffset, chip;
	//showHex("wrData",data);
	//showXY("wr", this->Coord.x,this->Coord.y);

#ifdef LCD_CMD_PORT	
	uint8_t cmdPort;	
#endif

#ifdef GLCD_DEBUG
	volatile uint16_t i;
	for (i=0; i<5000; i++);
#endif

	if (gCoord.x >= DISPLAY_WIDTH)
		return;
 	chip	=	gCoord.x/CHIP_WIDTH; 
 	KS0108_WaitReady(chip);


	 if (gCoord.x % CHIP_WIDTH == 0 && chip > 0)
	 { 		// todo , ignore address 0???
		KS0108_GotoXY(gCoord.x, gCoord.y);
	 }

	fastWriteHigh(D_I);					// D/I = 1
	fastWriteLow(R_W);					// R/W = 0	
	lcdDataDir(0xFF);					// data port is output
	
	yOffset	=	gCoord.y%8;

	if (yOffset != 0)
	{
		// first page
	#ifdef LCD_CMD_PORT 
		cmdPort	=	LCD_CMD_PORT;				// save command port
	#endif
		displayData	=	KS0108_ReadData();

	#ifdef LCD_CMD_PORT 		
		LCD_CMD_PORT	=	cmdPort;			// restore command port
	#else
		fastWriteHigh(D_I);						// D/I = 1
		fastWriteLow(R_W);						// R/W = 0
		SelectChip(chip);
	#endif
		lcdDataDir(0xFF);						// data port is output
		
		displayData |= data << yOffset;
		if (gInverted)
			displayData	=	~displayData;
		lcdDataOut( displayData);				// write data
		KS0108_Enable();							// enable
		
		// second page
		KS0108_GotoXY(gCoord.x, gCoord.y+8);
		
		displayData	=	KS0108_ReadData();

	#ifdef LCD_CMD_PORT 		
		LCD_CMD_PORT	=	cmdPort;			// restore command port
	#else		
		fastWriteHigh(D_I);						// D/I = 1
		fastWriteLow(R_W); 						// R/W = 0	
		SelectChip(chip);
	#endif
		lcdDataDir(0xFF);						// data port is output
		
		displayData |= data >> (8-yOffset);
		if (gInverted)
			displayData	=	~displayData;
		lcdDataOut(displayData);		// write data
		KS0108_Enable();					// enable
		
		KS0108_GotoXY(gCoord.x + 1, gCoord.y - 8);
	}
	else 
	{
		// just this code gets executed if the write is on a single page
		if (gInverted)
		{
			data	=	~data;	
		}
		EN_DELAY();
		lcdDataOut(data);				// write data
		KS0108_Enable();					// enable
		gCoord.x++;
		//showXY("WrData",gCoord.x, gCoord.y); 
	}
}


//***********************************************************************************
void KS0108_ClearPage(uint8_t page, uint8_t color)
{
	for (uint8_t x=0; x < DISPLAY_WIDTH; x++)
	{	
		KS0108_GotoXY(x, page * 8);
		KS0108_WriteData(color);
	}	
}

//***********************************************************************************
void KS0108_ClearScreen(uint8_t color)
{
uint8_t page;

	for ( page = 0; page < 8; page++)
	{
		KS0108_GotoXY(0, page * 8);
		KS0108_ClearPage(page, color);
	}
}

//***********************************************************************************
void KS0108_SetDot(uint8_t xLoc, uint8_t yLoc, uint8_t color)
{
	if ((xLoc >= 0) && (xLoc < gWidth) && (yLoc >= 0) && (yLoc < gHeight))
	{
	uint8_t data;
		
		KS0108_GotoXY(xLoc, yLoc - yLoc % 8);			// read data from display memory
			
		data	=	KS0108_ReadData();
		if (color == BLACK)
		{
			data |= 0x01 << (yLoc % 8);				// set dot
		}
		else
		{
			data &= ~(0x01 << (yLoc % 8));			// clear dot
		}	
		KS0108_WriteData(data);					// write data back to display
	}
}

#pragma mark -
#pragma mark Graphics Hardware Routines


//*******************************************************************************
//*	returns TRUE if succesfull
boolean	GrahicsHW_CheckIfPresent(void)
{
	return(true);
}

//*******************************************************************************
boolean	GrahicsHW_Init(void)
{
#ifdef _DEBUG_VIA_SERIAL_
	Serial.println("KS0108_Init");
	#if defined(__AVR_ATmega1280__)
	Serial.println("__AVR_ATmega1280__");
	#endif
#endif

	pinMode(D_I,	OUTPUT); 	
	pinMode(R_W,	OUTPUT); 	
	pinMode(EN,		OUTPUT); 	
	pinMode(CSEL1,	OUTPUT);
	pinMode(CSEL2,	OUTPUT);

	delay(10);

	fastWriteLow(D_I);
	fastWriteLow(R_W);
	fastWriteLow(EN);

	gCoord.x	=	0;
	gCoord.y	=	0;
	gCoord.page	=	0;
	
	gInverted	=	false;
	
	for (uint8_t chip=0; chip < DISPLAY_WIDTH/CHIP_WIDTH; chip++)
	{
		delay(10);
		KS0108_WriteCommand(LCD_ON, chip);				// power on
		delay(2);
		KS0108_WriteCommand(LCD_DISP_START, chip);		// display start line = 0
	}
//	delay(50); 
	delay(150); 
#ifdef _MONOCHROME_BLACK_ON_WHITE_
	KS0108_ClearScreen(WHITE);
#else
	KS0108_ClearScreen(BLACK);
#endif
	KS0108_GotoXY(0, 0);

	gWidth	=	DISPLAY_WIDTH;
	gHeight	=	DISPLAY_HEIGHT;
	return(true);
}

//*******************************************************************************
void	GrahicsHW_EraseScreen(void)
{
#ifdef _MONOCHROME_BLACK_ON_WHITE_
	KS0108_ClearScreen(WHITE);
#else
	KS0108_ClearScreen(BLACK);
#endif
}


//*******************************************************************************
void	GrahicsHW_Reset(void)
{
}

//*******************************************************************************
void	GrahicsHW_SetPixel(short xLoc, short yLoc, unsigned char theColorByte)
{
uint8_t	pixelColor;

	if ((xLoc > gWidth) || (yLoc > gHeight))
	{
		return;
	}
	if ((xLoc < 0) || (yLoc < 0))
	{
		return;
	}


	pixelColor	=	(theColorByte ? BLACK : WHITE);

	KS0108_SetDot(xLoc, yLoc, pixelColor);
}



//*******************************************************************************
short	GrahicsHW_GetWidth(void)
{
	return(gWidth);
}

//*******************************************************************************
short	GrahicsHW_GetHeight(void)
{
	return(gHeight);
}




#endif	//	_GRAPHICS_HW_KS0108_