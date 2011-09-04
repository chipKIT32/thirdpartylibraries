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
//*	Jun 20,	2011	<MLS> Started Wide TFT (www.wide.hk) Arduino TFT for ATMEGA168/328 with Touch Panel
//*******************************************************************************

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>



#include	"HardwareSerial.h"

#include	"UnivGraphicsHW_Defs.h"
#include	"UnivGraphicsLib.h"
#include	"UnivGraphicsHardware.h"	//*	this is the routines that the implementer provides



//*******************************************************************************
#pragma mark -
#pragma mark WideTFT private routines


#define	kWideTFT_DB10	2
#define	kWideTFT_DB11	3
#define	kWideTFT_DB12	4
#define	kWideTFT_DB13	5
#define	kWideTFT_DB14	6
#define	kWideTFT_DB15	7
#define	kWideTFT_DB16	8
#define	kWideTFT_DB17	9
#define	kWideTFT_RESET	10
#define	kWideTFT_CS		11
#define	kWideTFT_WR		12
#define	kWideTFT_RS		13
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(_BOARD_MEGA_)
	#define	kWideTFT_ReadPin		54				// analog input pin 0
	#define	kWideTFT_groundpin	58				// analog input pin 4 -- ground
	#define	kWideTFT_powerpin	59				// analog input pin 5 -- voltage
#else
	#define	kWideTFT_ReadPin		14				// analog input pin 0
	#define	kWideTFT_groundpin	18				// analog input pin 4 -- ground
	#define	kWideTFT_powerpin	19				// analog input pin 5 -- voltage
#endif


#define red			0xf800
#define green		0x07e0
#define blue		0x001f
#define black		0x0000
#define white		0xffff
#define gray		0x8c51
#define yellow		0xFFE0
#define cyan		0x07FF
#define purple		0xF81F

#define	SET_LOW(x)	digitalWrite(x,LOW)
#define	SET_HIGH(x)	digitalWrite(x,HIGH)

#ifdef __arm__
	#define lowByte(w) ((uint8_t) ((w) & 0xff))
	#define highByte(w) ((uint8_t) ((w) >> 8))
	#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif

static void Write_Data(unsigned char data);
static void Wide_LCD_ClearWindow(void);
static void Wide_LCD_SetX(unsigned int x);
static void Wide_LCD_SetY(unsigned int y);
static void Wide_LCD_SetWindow(unsigned int HSA,unsigned int HEA,unsigned int VSA,unsigned int VEA);


//************************************************************************
static void WideTFT_init(void)
{
	digitalWrite(kWideTFT_RESET,	HIGH);

//	pinMode(0,					OUTPUT);
//	pinMode(1,					OUTPUT);
	pinMode(kWideTFT_DB10,		OUTPUT);
	pinMode(kWideTFT_DB11,		OUTPUT);
	pinMode(kWideTFT_DB12,		OUTPUT);
	pinMode(kWideTFT_DB13,		OUTPUT);
	pinMode(kWideTFT_DB14,		OUTPUT);
	pinMode(kWideTFT_DB15,		OUTPUT);
	pinMode(kWideTFT_DB16,		OUTPUT);
	pinMode(kWideTFT_DB17,		OUTPUT);
	pinMode(kWideTFT_RESET,		OUTPUT);
	pinMode(kWideTFT_CS,		OUTPUT);
	pinMode(kWideTFT_WR,		OUTPUT);
	pinMode(kWideTFT_RS,		OUTPUT);
	pinMode(kWideTFT_ReadPin,	OUTPUT);


	digitalWrite(kWideTFT_RESET,		HIGH);


	pinMode(kWideTFT_groundpin,			OUTPUT);
	pinMode(kWideTFT_powerpin,			OUTPUT);
	digitalWrite(kWideTFT_groundpin,	LOW);
	digitalWrite(kWideTFT_powerpin,		HIGH);
}

//************************************************************************
static void Trans_Dat(unsigned int index, unsigned int data)
{
	unsigned char low8;
	unsigned char high8;


	low8	=	lowByte(index);
	high8	=	highByte(index);

	SET_LOW(kWideTFT_CS);
	SET_LOW(kWideTFT_RS);
	SET_HIGH(kWideTFT_WR);

	Write_Data(high8);

	SET_LOW(kWideTFT_WR);

	SET_HIGH(kWideTFT_WR);
	Write_Data(low8);
	SET_LOW(kWideTFT_WR);

	SET_HIGH(kWideTFT_WR);

	SET_HIGH(kWideTFT_RS);

	low8	=	lowByte(data);
	high8	=	highByte(data);

	Write_Data(high8);
	//PORTD	=	high8;
	SET_LOW(kWideTFT_WR);
		
	SET_HIGH(kWideTFT_WR);
	Write_Data(low8);
	SET_LOW(kWideTFT_WR);
				
	SET_HIGH(kWideTFT_WR);

	SET_HIGH(kWideTFT_CS);
}

//************************************************************************
static void Write_Data(unsigned char data)
{
#ifdef __AVR_ATmega328P__
	PORTD	=	data << 2;
#else
	digitalWrite(kWideTFT_DB10,	bitRead(data,0));
	digitalWrite(kWideTFT_DB11,	bitRead(data,1));
	digitalWrite(kWideTFT_DB12,	bitRead(data,2));
	digitalWrite(kWideTFT_DB13,	bitRead(data,3));
	digitalWrite(kWideTFT_DB14,	bitRead(data,4));
	digitalWrite(kWideTFT_DB15,	bitRead(data,5));
#endif
	digitalWrite(kWideTFT_DB16,	bitRead(data,6));
	digitalWrite(kWideTFT_DB17,	bitRead(data,7));
}

//************************************************************************
static void Wide_LCD_Init(void)
{
	SET_HIGH(kWideTFT_ReadPin);
//	SET_HIGH(kWideTFT_RESET);
//	delay(1);
//	SET_LOW(kWideTFT_RESET);
//	delay(5);
//	SET_HIGH(kWideTFT_RESET);
//	delay(10);

	Trans_Dat(0x00E3, 0x3008);
	Trans_Dat(0x00E7, 0x0012);
	Trans_Dat(0x00EF, 0x1231);
	Trans_Dat(0x0001, 0x0100);
	Trans_Dat(0x0002, 0x0700);				// set line inversion
//	Trans_Dat(0x0003, 0b0001000000011000);	// 横画面
	Trans_Dat(0x0003, 0b0001000000110000);	// 縦画面
	Trans_Dat(0x0004, 0x0000);
	Trans_Dat(0x0008, 0x0207);
	Trans_Dat(0x0009, 0x0000);
	Trans_Dat(0x000A, 0x0000);
	Trans_Dat(0x000C, 0x0000);
	Trans_Dat(0x000D, 0x0000);
	Trans_Dat(0x000F, 0x0000);
	Trans_Dat(0x0010, 0x0000);
	Trans_Dat(0x0011, 0x0007);
	Trans_Dat(0x0012, 0x0000); // VREG1OUT voltage
	Trans_Dat(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	delay(200);
	Trans_Dat(0x0010, 0x1490); // SAP, BT[3:0], AP[2:0], DSTB, SLP, STB
	Trans_Dat(0x0011, 0x0227);
	delay(50); // Delayms 50ms
	Trans_Dat(0x0012, 0x001c); // External reference voltage= Vci;
	delay(50); // Delayms 50ms
	Trans_Dat(0x0013, 0x0A00); // R13=0F00 when R12=009E;VDV[4:0] for VCOM amplitude
	Trans_Dat(0x0029, 0x000F); // R29=0019 when R12=009E;VCM[5:0] for VCOMH//0012//
	Trans_Dat(0x002B, 0x000D); // Frame Rate	=	91Hz
	delay(50);
	Trans_Dat(0x0030, 0x0000);
	Trans_Dat(0x0031, 0x0203);
	Trans_Dat(0x0032, 0x0001);
	Trans_Dat(0x0035, 0x0205);
	Trans_Dat(0x0036, 0x030C);
	Trans_Dat(0x0037, 0x0607);
	Trans_Dat(0x0038, 0x0405);
	Trans_Dat(0x0039, 0x0707);
	Trans_Dat(0x003C, 0x0502);
	Trans_Dat(0x003D, 0x1008);

	Wide_LCD_ClearWindow();
	Trans_Dat(0x0060, 0xA700); // Gate Scan Line
	Trans_Dat(0x0061, 0x0001);
	Trans_Dat(0x006A, 0x0000);
	Trans_Dat(0x0080, 0x0000);
	Trans_Dat(0x0081, 0x0000);
	Trans_Dat(0x0082, 0x0000);
	Trans_Dat(0x0083, 0x0000);
	Trans_Dat(0x0084, 0x0000);
	Trans_Dat(0x0085, 0x0000);
	Trans_Dat(0x0090, 0x0010);
	Trans_Dat(0x0092, 0x0600); //0x0000
	Trans_Dat(0x0093, 0x0003);
	Trans_Dat(0x0095, 0x0110);
	Trans_Dat(0x0097, 0x0000);
	Trans_Dat(0x0098, 0x0000);
	Trans_Dat(0x0007, 0x0133);
}

#define ENDCOL		 240	// X
#define ENDROW		 320	// Y
#define XChar		(int)((ENDCOL) / 6)
#define YLine		(int)((ENDROW) / 12)

//************************************************************************
static void Wide_LCD_Clear(unsigned int Color)
{
	unsigned long i;
	Wide_LCD_SetX(0);
	Wide_LCD_SetY(0);
	Wide_LCD_ClearWindow();
	for (i=0; i < 320UL*240UL; i++)
	{
		Trans_Dat(0x0022,Color);
	}
}

//************************************************************************
static void Wide_LCD_ClearWindow(void)
{
	Wide_LCD_SetWindow(0,239,0,319);
}

//************************************************************************
static void Wide_LCD_SetWindow(unsigned int HSA,unsigned int HEA,unsigned int VSA,unsigned int VEA)
{
	Trans_Dat(0x0050, HSA); // 水平(Horizontal) G
	Trans_Dat(0x0051, HEA); // 水平(Horizontal)
	Trans_Dat(0x0052, VSA); // 垂直(Vertical)
	Trans_Dat(0x0053, VEA); // 垂直(Vertical)
}	

//************************************************************************
static void Wide_LCD_SetX(unsigned int x) //座標X
{
	Trans_Dat(0x0020, x);
}	

//************************************************************************
static void Wide_LCD_SetY(unsigned int y) //座標Y
{
	Trans_Dat(0x0021, y);
}	

//************************************************************************
static void Wide_LCD_Color(unsigned int Color)
{
	Trans_Dat(0x0022,Color);
}

//************************************************************************
static void Wide_LCD_Pixel(unsigned int Xpos, unsigned int Ypos, unsigned int Color)
{

	if ((Xpos < ENDCOL) && (Ypos < ENDROW))
	{
		Trans_Dat(0x0020, Xpos);			// 座標指定
		Trans_Dat(0x0021, Ypos);
		Trans_Dat(0x0022, Color);
	}
//??	delay(1);
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

	
	gWidth	=	320;
	gHeight	=	240;
	return(true);
}

//*******************************************************************************
void	GrahicsHW_EraseScreen(void)
{
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
//	the16bitColor	=	((short)theColor->red & 0xF8) << 8;
//	the16bitColor	+=	((short)theColor->green & 0xFC) << 3;
//	the16bitColor	+=	((short)theColor->blue & 0xF8) >> 3;

}

//*******************************************************************************
void	GrahicsHW_SetPixelRGB16bit(short xLoc, short yLoc, unsigned short the16bitColor)
{

}


//*******************************************************************************
void	GrahicsHW_SetPixel(short xLoc, short yLoc, unsigned char theColorByte)
{
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


