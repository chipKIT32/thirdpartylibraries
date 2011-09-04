//************************************************************************
//*	adapted for Arduino and Mega by Mark Sproul
//*	Pin definitions
//*		RES	Reset			Digital Pin 8
//*		CS	Chip Select		Digital Pin 9
//*		DIO	Data I/O		Digital Pin 11
//*		SCK	Serial Clock	Digital Pin 13
//************************************************************************
//*		At first I tried to port it to the Arduino pin model, i.e. digitalWrite.
//*		As I feared, it introduced a noticeable decrease in speed.  For the LCD
//*		interface it needs to stay as it was. I now have it fully working on the
//*		mega utilizing the original code. This was done by modifying the pin
//*		definitions in the .h file.  Very straight forward and very clean. In fact
//*		it worked first shot on the Mega.
//************************************************************************
//#define	PHILLIPS
#define	EPSON
//*	these define the PORT and BIT number for each control pin

#ifndef __AVR__
	#define _USE_ARDUINO_FOR_NOKIEA_
	//#define	_USE_SPI_LIB_
#endif
//	#define _USE_ARDUINO_FOR_NOKIEA_

//*	Arduino pin number defs
#define	LCD_RES_PIN	8
#define	CS_PIN		9
#define	DIO_PIN		11
#define	SCK_PIN		13

#define	kSwitch1_PIN	3
#define	kSwitch2_PIN	4
#define	kSwitch3_PIN	5


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  || defined(_BOARD_MEGA_) 
	//*	Arduino Mega bit numbers
	#define LCD_RES		5		// D8
	#define CS			6		// D9
	#define DIO			5		// D11
	#define SCK			7		// D13

	//*	Arduino Mega ports
	#define	LCD_PORT_CS		PORTH
	#define	LCD_PORT_SCK	PORTB
	#define	LCD_PORT_RES	PORTH
	#define	LCD_PORT_DIO	PORTB
#else
	//*	Arduino Duemilanove bit numbers
	#define LCD_RES		0		// D8
	#define CS			1		// D9
	#define DIO			3		// D11
	#define SCK			5		// D13
	//#define LCD_PORT	PORTB

	//*	Arduino Duemilanove ports
	#define	LCD_PORT_CS		PORTB
	#define	LCD_PORT_SCK	PORTB
	#define	LCD_PORT_RES	PORTB
	#define	LCD_PORT_DIO	PORTB
#endif


/* Old
#define CS			2		// D10
#define SCK			3		// D11
#define LCD_RES		1		// D9
#define DIO			4		// D12
#define LCD_PORT	PORTB
*/

//*******************************************************
//						Macros
//*******************************************************
#ifndef _USE_ARDUINO_FOR_NOKIEA_
	#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
	#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))
#endif

//*******************************************************
//					General Definitions
//*******************************************************
#define MYUBRR 16	//Used to set the AVR Baud Rate TO 115200 (External 16MHz Oscillator)


#ifdef __cplusplus
	extern "C" {
#endif

//********************************************************************
//
//				General Function Definitions
//
//********************************************************************
//void LCDCommand(unsigned char data);
//void LCDData(unsigned char data);
//void LCDInit(void);
//void LCDClear(int color);
void LCDPrintLogo(void);
//void LCDContrast(char setting);
//void LCDSetPixel(int color, unsigned char x, unsigned char y);

//********************************************************************
//
//					LCD Dimension Definitions
//
//********************************************************************
#define ROW_LENGTH	132
#define COL_HEIGHT	132
#define ENDPAGE     132
#define ENDCOL      130

//********************************************************************
//
//					EPSON Controller Definitions
//
//********************************************************************
#define DISON       0xAF
#define DISOFF      0xAE
#define DISNOR      0xA6
#define DISINV      0xA7
#define SLPIN       0x95
#define SLPOUT      0x94
#define COMSCN      0xBB
#define DISCTL      0xCA
#define PASET       0x75
#define CASET       0x15
#define DATCTL      0xBC
#define RGBSET8     0xCE
#define RAMWR       0x5C
#define RAMRD       0x5D
#define PTLIN       0xA8
#define PTLOUT      0xA9
#define RMWIN       0xE0
#define RMWOUT      0xEE
#define ASCSET      0xAA
#define SCSTART     0xAB
#define OSCON       0xD1
#define OSCOFF      0xD2
#define PWRCTR      0x20
#define VOLCTR      0x81
#define VOLUP       0xD6
#define VOLDOWN     0xD7
#define TMPGRD      0x82
#define EPCTIN      0xCD
#define EPCOUT      0xCC
#define EPMWR       0xFC
#define EPMRD       0xFD
#define EPSRRD1     0x7C
#define EPSRRD2     0x7D
#define NOP         0x25

//********************************************************************
//
//			PHILLIPS Controller Definitions
//
//********************************************************************
//LCD Commands
#define	NOPP		0x00
#define	BSTRON		0x03
#define SLEEPIN     0x10
#define	SLEEPOUT	0x11
#define	NORON		0x13
#define	INVOFF		0x20
#define INVON      	0x21
#define	SETCON		0x25
#define DISPOFF     0x28
#define DISPON      0x29
#define CASETP      0x2A
#define PASETP      0x2B
#define RAMWRP      0x2C
#define RGBSET	    0x2D
#define	MADCTL		0x36
#define	COLMOD		0x3A
#define DISCTR      0xB9
#define	EC			0xC0

//*******************************************************
//				12-Bit Color Definitions
//*******************************************************
#define WHITE	0xFFF
#define BLACK	0x000
#define RED		0xF00
#define	GREEN	0x0F0
#define BLUE	0x00F
#define CYAN	0x0FF
#define MAGENTA	0xF0F
#define YELLOW	0xFF0
#define BROWN	0xB22
#define ORANGE	0xFA0
#define PINK	0xF6A

//typedef struct
//{
//	unsigned char red;
//	unsigned char green;
//	unsigned char blue;
//} RGBColor;


#ifdef __cplusplus
	}
#endif
