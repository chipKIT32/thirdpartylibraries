//-------------------------------------------------------------------------------------------------
// Graphic LCD with Toshiba T6963 controller
// Copyright (c) Rados³aw Kwiecieñ, 2007r
// http://en.radzio.dxp.pl/t6963/
// Compiler : avr-gcc
// Modified By -Gil- to work on Arduino easily : http://domoduino.tumblr.com/
//-------------------------------------------------------------------------------------------------

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// data port
#define GLCD_DATA_PORT1 	PORTD
#define GLCD_DATA_PIN1		PIND
#define GLCD_DATA_DDR1		DDRD
#define GLCD_DATA_SHIFT1	<<2
#define GLCD_DATA_RSHIFT1	>>2
#define GLCD_DATA_MASK1		0xFC

#define GLCD_DATA_PORT2 	PORTB
#define GLCD_DATA_PIN2		PINB
#define GLCD_DATA_DDR2		DDRB
#define GLCD_DATA_SHIFT2		>>6
#define GLCD_DATA_RSHIFT2		<<6
#define GLCD_DATA_MASK2		0x03

// control port
#define GLCD_CTRL_PORT		PORTC
#define GLCD_CTRL_PIN		PINC
#define GLCD_CTRL_DDR		DDRC
// control signals
#define GLCD_WR			0
#define GLCD_RD				1
#define GLCD_CE				2
#define GLCD_CD				3
#define GLCD_RESET			4
#define GLCD_FS				5
// display properties

#if 0
	#define GLCD_NUMBER_OF_LINES				64
	#define GLCD_PIXELS_PER_LINE				240
	#define GLCD_FONT_WIDTH					    6
#else
	#define GLCD_NUMBER_OF_LINES				128
	#define GLCD_PIXELS_PER_LINE				160
	#define GLCD_FONT_WIDTH					    8
#endif
//
#define GLCD_GRAPHIC_AREA				(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_TEXT_AREA					(GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_GRAPHIC_SIZE				(GLCD_GRAPHIC_AREA * GLCD_NUMBER_OF_LINES)
#define GLCD_TEXT_SIZE					(GLCD_TEXT_AREA * (GLCD_NUMBER_OF_LINES/8))


#define GLCD_TEXT_HOME					0
#define GLCD_GRAPHIC_HOME				(GLCD_TEXT_HOME + GLCD_TEXT_SIZE)
#define GLCD_OFFSET_REGISTER			2
#define GLCD_EXTERNAL_CG_HOME			(GLCD_OFFSET_REGISTER << 11)


#define T6963_SET_CURSOR_POINTER		0x21
#define T6963_SET_OFFSET_REGISTER		0x22
#define T6963_SET_ADDRESS_POINTER		0x24

#define T6963_SET_TEXT_HOME_ADDRESS		0x40
#define T6963_SET_TEXT_AREA				0x41
#define T6963_SET_GRAPHIC_HOME_ADDRESS	0x42
#define T6963_SET_GRAPHIC_AREA			0x43

#define T6963_MODE_SET					0x80


#define T6963_DISPLAY_MODE				0x90
	#define T6963_CURSOR_BLINK_ON		0x01
	#define T6963_CURSOR_DISPLAY_ON		0x02
	#define T6963_TEXT_DISPLAY_ON		0x04
	#define T6963_GRAPHIC_DISPLAY_ON	0x08				


#define T6963_CURSOR_PATTERN_SELECT		0xA0
	#define T6963_CURSOR_1_LINE			0x00
	#define T6963_CURSOR_2_LINE			0x01
	#define T6963_CURSOR_3_LINE			0x02
	#define T6963_CURSOR_4_LINE			0x03
	#define T6963_CURSOR_5_LINE			0x04
	#define T6963_CURSOR_6_LINE			0x05
	#define T6963_CURSOR_7_LINE			0x06
	#define T6963_CURSOR_8_LINE			0x07

#define T6963_SET_DATA_AUTO_WRITE			0xB0
#define T6963_SET_DATA_AUTO_READ			0xB1
#define T6963_AUTO_RESET					0xB2

#define T6963_DATA_WRITE_AND_INCREMENT		0xC0
#define T6963_DATA_READ_AND_INCREMENT		0xC1
#define T6963_DATA_WRITE_AND_DECREMENT		0xC2
#define T6963_DATA_READ_AND_DECREMENT		0xC3
#define T6963_DATA_WRITE_AND_NONVARIALBE	0xC4
#define T6963_DATA_READ_AND_NONVARIABLE		0xC5

#define T6963_SCREEN_PEEK					0xE0
#define T6963_SCREEN_COPY				0xE8


void GLCD_InitalizeInterface(void);
unsigned char GLCD_ChceckStatus(void);
void GLCD_WriteCommand(unsigned char);
void GLCD_WriteData(unsigned char);
unsigned char GLCD_ReadData(void);
void GLCD_ClearText(void);
void GLCD_ClearCG(void);
void GLCD_ClearGraphic(void);
void GLCD_WriteChar(char ch);
void GLCD_WriteString(char * str);
void GLCD_WriteStringPgm(prog_char * str);
void GLCD_TextGoTo(unsigned char, unsigned char);
void GLCD_DefineCharacter(unsigned char, unsigned char *);
void GLCD_Initalize(void);
void GLCD_SetPixel(unsigned char, unsigned char, unsigned char);
void GLCD_WriteDisplayData(unsigned char);

