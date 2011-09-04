//*******************************************************************************
//*	
//*	MikroElektronikadrivers
//*	
//*	(C) by Mark Sproul
//*	This code is derived from other works. Credit is given below
//*	This is open source like the rest of the arduino source code
//*
//*	this code is derived from the ks0108.cpp Arduino library
//*
//*******************************************************************************
//*******************************************************************************
//*	Detailed edit history
//*******************************************************************************
//*	Jan 28,	2010	<MLS> Started on UnivGraphicsLib
//*	Jun 18,	2011	<MLS> Started MikroElektronika support
//*******************************************************************************

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>



#include	"HardwareSerial.h"

#include	"UnivGraphicsHW_Defs.h"
#include	"UnivGraphicsLib.h"
#include	"UnivGraphicsHardware.h"	//*	this is the routines that the implementer provides


#include	"Hardware_MikroElektronika.h"
//#include	"drvTFT002.h"
#include	"HX8347.h"

//*******************************************************************************
#pragma mark -
#pragma mark MikroMedia private routines

//*******************************************************************************
void  DelayMs(WORD time)
{
	delay(time);
}

/*********************************************************************
* Function:  void  SetReg(BYTE index, BYTE value)
*
* PreCondition: none
*
* Input: index - register number
*		value - value to be set
*
* Output: none
*
* Side Effects: none
*
* Overview: sets graphics controller register
*
* Note: none
*
********************************************************************/
static void  SetReg(unsigned char index, unsigned char value)
{

	CS_LAT_BIT	=	0;

	SetIndex(index);
	WriteCommand(value);

	CS_LAT_BIT	=	1;
}

//*******************************************************************************
static int GetReg(unsigned char index)
{
	int x;
	
	CS_LAT_BIT	=	0;
	SetIndex(index);
	DelayMs(1);
	
	RS_LAT_BIT=1;	
	x	=	PMDIN;		  // start a read sequence
	PMPWaitBusy();	  // wait for read completion
	x	=	PMDIN;
	
	CS_LAT_BIT	=	1;
	return x;
}


/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets LCD, initializes PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void)
{
	int i;

	// Hold in reset
	RST_LAT_BIT			=	0;
	// Set reset pin as output
	RST_TRIS_BIT		=	0;

	// Enable data access
	RS_LAT_BIT			=	1;
	// Set RS pin as output
	RS_TRIS_BIT			=	0;

	// Disable LCD 
	CS_LAT_BIT			=	1;
	// Set LCD CS pin as output
	CS_TRIS_BIT			=	0;

	// PMP setup 
	PMMODE	=	0;
	PMAEN	=	0;
	PMCON	=	0;
	PMMODEbits.MODE 	=	2;  // Master 2
	PMMODEbits.WAITB 	=	0;
#ifdef __PIC32MX
	PMMODEbits.WAITM 	=	3;
#else
	PMMODEbits.WAITM 	=	2;
#endif
	PMMODEbits.WAITE 	=	0;

#ifdef USE_16BIT_PMP
	PMMODEbits.MODE16	=	1;  // 16 bit mode
#else
	PMMODEbits.MODE16	=	0;  // 8 bit mode
#endif

	PMCONbits.CSF		=	0;
	PMCONbits.PTRDEN	=	1;
	PMCONbits.PTWREN	=	1;
	PMCONbits.PMPEN 	=	1; 

	// Release from reset
	DelayMs(100);
	RST_LAT_BIT	=	1;
	DelayMs(150);

	// Driving ability setting
	SetReg(0xEA,0x00);	// PTBA[15:8]
	SetReg(0xEB,0x20);	// PTBA[7:0]
	SetReg(0xEC,0x0C);	// STBA[15:8]
	SetReg(0xED,0xC4);	// STBA[7:0]
	SetReg(0xE8,0x40);	// OPON[7:0]
	SetReg(0xE9,0x38);	// OPON1[7:0]
	SetReg(0xF1,0x01);	// OTPS1B
	SetReg(0xF2,0x10);	// GEN
	SetReg(0x27,0xA3);	// 

	// Gamma 2.8 setting 
	SetReg(0x40,0x00);	// 
	SetReg(0x41,0x00);	// 
	SetReg(0x42,0x01);	// 
	SetReg(0x43,0x13);	// 
	SetReg(0x44,0x10);	// 
	SetReg(0x45,0x26);	// 
	SetReg(0x46,0x08);	// 
	SetReg(0x47,0x51);	// 
	SetReg(0x48,0x02);	// 
	SetReg(0x49,0x12);	// 
	SetReg(0x4A,0x18);	// 
	SetReg(0x4B,0x19);	// 
	SetReg(0x4C,0x14);	// 

	SetReg(0x50,0x19);	// 
	SetReg(0x51,0x2F);	// 
	SetReg(0x52,0x2C);	// 
	SetReg(0x53,0x3E);	// 
	SetReg(0x54,0x3F);	//		  
	SetReg(0x55,0x3F);	// 
	SetReg(0x56,0x2E);	// 
	SetReg(0x57,0x77);	// 
	SetReg(0x58,0x0B);	// 
	SetReg(0x59,0x06);	//	  
	SetReg(0x5A,0x07);	// 
	SetReg(0x5B,0x0D);	// 
	SetReg(0x5C,0x1D);	// 
	SetReg(0x5D,0xCC);	// 

	// Window setting
#if (DISP_ORIENTATION == 0)
	SetReg(0x04,0x00);
	SetReg(0x05,0xEF);
	SetReg(0x08,0x01);
	SetReg(0x09,0x3F);
#else
	SetReg(0x04,0x01);
	SetReg(0x05,0x3F);
	SetReg(0x08,0x00);
	SetReg(0x09,0xEF);
#endif

//	// Display Setting
//	SetReg(0x01,0x06);	// IDMON=0, INVON=1, NORON=1, PTLON=0 

#if (DISP_ORIENTATION == 0)
	SetReg(0x16,0x08);	// MY=0, MX=0, MV=0, BGR=1
#else
	SetReg(0x16,0x68);	// MY=0, MX=1, MV=1, BGR=1
#endif


	// Power Voltage Setting
	SetReg(0x1B,0x1B);	// VRH = 4.65
	SetReg(0x1A,0x01);	// BT
	SetReg(0x24,0x2F);	// VMH
	SetReg(0x25,0x57);	// VML

	// Vcom offset
	SetReg(0x23,0x8D);	// FLICKER ADJUST

	// Power ON Setting
	SetReg(0x18,0x36);	// 
	SetReg(0x19,0x01);	//  
	SetReg(0x01,0x00);	// 
	SetReg(0x1F,0x88);	//
	DelayMs(5); 
	SetReg(0x1F,0x80);	//
	DelayMs(5); 
	SetReg(0x1F,0x90);	//
	DelayMs(5); 
	SetReg(0x1F,0xD0);	//
	DelayMs(5); 

	// 65K Color Selection
	SetReg(0x17,0x05);	//

	// Set Panel
  	SetReg(0x36,0x00);	//
  	
	// Display ON Setting
	SetReg(0x28,0x38);	//
	DelayMs( 40); 
	SetReg(0x28,0x3C);	//
  
}

/*********************************************************************
* Macros:  SetAddress(addr2,addr1,addr0)
* routine:  SetAddressRoutine(x,y)
*
* Overview: Writes address pointer.
*
* PreCondition: none
*
* Input: x - horizontal position
*        y - vertical position
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
static void	SetAddressRoutine(unsigned short x, unsigned short y)
{
	
	SetIndex(0x02);
	WriteCommand(x >> 8);

	SetIndex(0x03);
	WriteCommand(x & 0x0ff);

	SetIndex(0x06);
	WriteCommand(y >> 8);

	SetIndex(0x07);
	WriteCommand(y & 0x0ff);

	SetIndex(0x22);

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
	Serial.println("GrahicsHW_Init");
	#if defined(__AVR_ATmega1280__)
		Serial.println("__AVR_ATmega1280__");
	#endif
#endif
	ResetDevice();
	
	gWidth	=	320;
	gHeight	=	240;
	return(true);
}

//*******************************************************************************
void	GrahicsHW_EraseScreen(void)
{
DWORD	counter;

	CS_LAT_BIT = 0;
	SetAddressRoutine(0,0);
	for(counter=0; counter<(DWORD)(GetMaxX()+1)*(GetMaxY()+1); counter++)
	{
	//	WriteData(_color);
		WriteData(0);
	}
	CS_LAT_BIT = 1;
}


//*******************************************************************************
void	GrahicsHW_Reset(void)
{
}

//*******************************************************************************
void	GrahicsHW_SetPixelRGB(short xLoc, short yLoc, RGBColor *theColor)
{
unsigned short	the16bitColor;

//#define RGB565CONVERT(red, green, blue) (WORD) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))
//	Converts true color into 5:6:5 RGB format.
	the16bitColor	=	((short)theColor->red & 0xF8) << 8;
	the16bitColor	+=	((short)theColor->green & 0xFC) << 3;
	the16bitColor	+=	((short)theColor->blue & 0xF8) >> 3;

	CS_LAT_BIT	=	0;
	SetAddressRoutine(xLoc, yLoc);
	WriteData(the16bitColor);   
	CS_LAT_BIT	=	1;
}

//*******************************************************************************
void	GrahicsHW_SetPixelRGB16bit(short xLoc, short yLoc, unsigned short the16bitColor)
{

	CS_LAT_BIT	=	0;
	SetAddressRoutine(xLoc, yLoc);
	WriteData(the16bitColor);   
	CS_LAT_BIT	=	1;
}


//*******************************************************************************
void	GrahicsHW_SetPixel(short xLoc, short yLoc, unsigned char theColorByte)
{
unsigned short	the16bitColor;

	//*	8 bit format is	BBGGGRRR
//	Converts true color into 5:6:5 RGB format.

	the16bitColor	=	(theColorByte & 0x0f) << 15;
	the16bitColor	+=	(theColorByte & 0x38) << 5;
	the16bitColor	+=	(theColorByte & 0xc0) >> 2;


	CS_LAT_BIT	=	0;
	SetAddressRoutine(xLoc, yLoc);
	WriteData(the16bitColor);   
	CS_LAT_BIT	=	1;
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


