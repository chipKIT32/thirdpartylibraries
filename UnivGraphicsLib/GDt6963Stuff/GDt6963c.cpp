//-------------------------------------------------------------------------------------------------
// Graphic LCD with Toshiba T6963 controller
// Copyright (c) Radoslaw Kwiecien, 2007r
// http://en.radzio.dxp.pl/t6963/
// Compiler : avr-gcc
// Modified By -Gil- to work on Arduino easily : http://domoduino.tumblr.com/
//-------------------------------------------------------------------------------------------------
#include "GDT6963C.h"
//-------------------------------------------------------------------------------------------------
//
// Delay function
//	
//-------------------------------------------------------------------------------------------------
void n_delay(void)
{
volatile unsigned char i;

	for(i = 0; i < (F_CPU/1000000); i++)
	{
		asm("nop");
	}
}
//-------------------------------------------------------------------------------------------------
//
// Ports intalization
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitalizeInterface(void)
{
	//GLCD_DATA_DDR	=	0xFF;
	GLCD_DATA_DDR1 |= GLCD_DATA_MASK1;
	GLCD_DATA_DDR2 |= GLCD_DATA_MASK2;
		
	GLCD_CTRL_DDR	=	((1 << GLCD_WR) | (1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD) | (1 << GLCD_RESET) | (1 << GLCD_FS));
	GLCD_CTRL_PORT |= ((1 << GLCD_WR) | (1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD) | (1 << GLCD_RESET) | (1 << GLCD_FS));
}
//-------------------------------------------------------------------------------------------------
//
// Reads dispay status
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ChceckStatus(void)
{
	uint8_t tmp;
	//GLCD_DATA_DDR	=	0x00;
	GLCD_DATA_DDR1 &= ~GLCD_DATA_MASK1;
	GLCD_DATA_DDR2 &= ~GLCD_DATA_MASK2;

	GLCD_CTRL_PORT &= ~((1 << GLCD_RD) | (1 << GLCD_CE));
	n_delay();
	//tmp	=	GLCD_DATA_PIN;
	tmp	=	(GLCD_DATA_PIN1 GLCD_DATA_RSHIFT1) | (GLCD_DATA_PIN2 GLCD_DATA_RSHIFT2);
	//GLCD_DATA_DDR	=	0xFF;
	GLCD_DATA_DDR1 |= GLCD_DATA_MASK1;
	GLCD_DATA_DDR2 |= GLCD_DATA_MASK2;
	GLCD_CTRL_PORT |= ((1 << GLCD_RD) | (1 << GLCD_CE));
	return tmp;
}
//-------------------------------------------------------------------------------------------------
//
// Writes instruction 
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char command)
{
	while(!(GLCD_ChceckStatus()&0x03));
		
	// GLCD_DATA_PORT	=	command;
	GLCD_DATA_PORT1 &= ~GLCD_DATA_MASK1;
	GLCD_DATA_PORT1 |= (command GLCD_DATA_SHIFT1);
	GLCD_DATA_PORT2 &= ~GLCD_DATA_MASK2;
	GLCD_DATA_PORT2 |= (command GLCD_DATA_SHIFT2);


	GLCD_CTRL_PORT &= ~((1 << GLCD_WR) | (1 << GLCD_CE));
	n_delay();
	GLCD_CTRL_PORT |= ((1 << GLCD_WR) | (1 << GLCD_CE));
}

//-------------------------------------------------------------------------------------------------
//
// Writes data
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char data)
{
	while(!(GLCD_ChceckStatus()&0x03));
	// GLCD_DATA_PORT	=	data;
	GLCD_DATA_PORT1 &= ~GLCD_DATA_MASK1;
	GLCD_DATA_PORT1 |= (data GLCD_DATA_SHIFT1);
	GLCD_DATA_PORT2 &= ~GLCD_DATA_MASK2;
	GLCD_DATA_PORT2 |= (data GLCD_DATA_SHIFT2);

	GLCD_CTRL_PORT &= ~((1 << GLCD_WR) | (1 << GLCD_CE) | (1 << GLCD_CD));
	n_delay();
	GLCD_CTRL_PORT |= ((1 << GLCD_WR) | (1 << GLCD_CE) | (1 << GLCD_CD));
}
//-------------------------------------------------------------------------------------------------
//
// Reads data
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
	uint8_t tmp;
	while(!(GLCD_ChceckStatus()&0x03));
	//GLCD_DATA_DDR	=	0x00;
	GLCD_DATA_DDR1 &= ~GLCD_DATA_MASK1;
	GLCD_DATA_DDR2 &= ~GLCD_DATA_MASK2;

	GLCD_CTRL_PORT &= ~((1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD));
	n_delay();

	//tmp	=	GLCD_DATA_PIN;
	tmp	=	(GLCD_DATA_PIN1 GLCD_DATA_RSHIFT1) | (GLCD_DATA_PIN2 GLCD_DATA_RSHIFT2);
	GLCD_CTRL_PORT |= ((1 << GLCD_RD) | (1 << GLCD_CE) | (1 << GLCD_CD));
	//GLCD_DATA_DDR	=	0xFF;
	GLCD_DATA_DDR1 |= GLCD_DATA_MASK1;
	GLCD_DATA_DDR2 |= GLCD_DATA_MASK2;

	return tmp;
}
//-------------------------------------------------------------------------------------------------
//
// Sets address pointer for display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_SetAddressPointer(unsigned int address)
{
	GLCD_WriteData(address & 0xFF);
	GLCD_WriteData(address >> 8);
	GLCD_WriteCommand(T6963_SET_ADDRESS_POINTER);
}
//-------------------------------------------------------------------------------------------------
//
// Clears text area of display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_ClearText(void)
{
int i;
	GLCD_SetAddressPointer(GLCD_TEXT_HOME);

	for(i = 0; i < GLCD_TEXT_SIZE; i++)
	{
		GLCD_WriteDisplayData(0);
	}
}
//-------------------------------------------------------------------------------------------------
//
// Clears characters generator area of display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_ClearCG(void)
{
unsigned int i;
	GLCD_SetAddressPointer(GLCD_EXTERNAL_CG_HOME);

	for(i = 0; i < 256 * 8; i++)
	{
		GLCD_WriteDisplayData(0);
	}
}
//-------------------------------------------------------------------------------------------------
//
// Clears graphics area of display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_ClearGraphic(void)
{
int i;
	GLCD_SetAddressPointer(GLCD_GRAPHIC_HOME);
	for(i = 0; i < GLCD_GRAPHIC_SIZE; i++)
	{
		GLCD_WriteDisplayData(0x00);
	}
}
//-------------------------------------------------------------------------------------------------
//
// Writes a single character (ASCII code) to display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charCode)
{
	GLCD_WriteDisplayData(charCode - 32);
}
//-------------------------------------------------------------------------------------------------
//
// Writes null-terminated string to display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * string)
{
	while(*string)
	{
		GLCD_WriteChar(*string++);
	}
}
//-------------------------------------------------------------------------------------------------
//
// Writes null-terminated string from program memory to display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteStringPgm(prog_char * string)
{
char ch;
	while((ch = pgm_read_byte(string++)))
	{
		GLCD_WriteChar(ch);
	}
}
//-------------------------------------------------------------------------------------------------
//
// Sets display coordinates
//
//-------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
unsigned int address;

	address	=	GLCD_TEXT_HOME + x + (GLCD_TEXT_AREA * y);

	GLCD_SetAddressPointer(address);
}
//-------------------------------------------------------------------------------------------------
//
// Writes single char pattern to character generator area of display RAM memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_DefineCharacter(unsigned char charCode, unsigned char * defChar)
{
unsigned int address;
unsigned char i; 

	address	=	GLCD_EXTERNAL_CG_HOME + (8 * charCode);

	GLCD_SetAddressPointer(address);

	for(i = 0; i < 8 ; i++)
	{
		GLCD_WriteDisplayData(*(defChar + i));
	}
}

//-------------------------------------------------------------------------------------------------
//
// Set (if color==1) or clear (if color==0) pixel on screen
//
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
unsigned char tmp;
unsigned int address;

	address	=	GLCD_GRAPHIC_HOME + (x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y);

	GLCD_SetAddressPointer(address);

	GLCD_WriteCommand(T6963_DATA_READ_AND_NONVARIABLE);
	tmp	=	GLCD_ReadData();

	if(color)
	{
		tmp |= (1 << (GLCD_FONT_WIDTH - 1 - (x % GLCD_FONT_WIDTH)));
	}
	else
	{
		tmp &= ~(1 << (GLCD_FONT_WIDTH - 1 - (x % GLCD_FONT_WIDTH)));
	}
	GLCD_WriteDisplayData(tmp);

}
//-------------------------------------------------------------------------------------------------
//
// Writes display data and increment address pointer
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteDisplayData(unsigned char x)
{
	GLCD_WriteData(x);
	GLCD_WriteCommand(T6963_DATA_WRITE_AND_INCREMENT);
}
//-------------------------------------------------------------------------------------------------
//
// Sets graphics coordinates
//
//-------------------------------------------------------------------------------------------------
void GLCD_GraphicGoTo(unsigned char x, unsigned char y)
{
unsigned int address;
	address	=	GLCD_GRAPHIC_HOME + (x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y);
	GLCD_SetAddressPointer(address);
}
//-------------------------------------------------------------------------------------------------
//
// Displays bitmap from program memory
//
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(unsigned char * bitmap, unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{
unsigned char i, j;

	for(j = 0; j < height; j++)
	{
		GLCD_GraphicGoTo(x, y + j);
		for(i = 0; i < width/GLCD_FONT_WIDTH; i++)
		{
			GLCD_WriteDisplayData(pgm_read_byte(bitmap + i + (GLCD_GRAPHIC_AREA * j))); 	
		}
	}
}
//-------------------------------------------------------------------------------------------------
//
// Display initalization
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initalize(void)
{

	GLCD_InitalizeInterface();

	GLCD_CTRL_PORT &= ~(1 << GLCD_RESET);
	_delay_ms(1);
	GLCD_CTRL_PORT |= (1 << GLCD_RESET);

	#if (GLCD_FONT_WIDTH == 8)
	GLCD_CTRL_PORT &= ~(1 << GLCD_FS);
	#endif

	GLCD_WriteData(GLCD_GRAPHIC_HOME & 0xFF);
	GLCD_WriteData(GLCD_GRAPHIC_HOME >> 8);
	GLCD_WriteCommand(T6963_SET_GRAPHIC_HOME_ADDRESS);

	GLCD_WriteData(GLCD_GRAPHIC_AREA);
	GLCD_WriteData(0x00);
	GLCD_WriteCommand(T6963_SET_GRAPHIC_AREA);

	GLCD_WriteData(GLCD_TEXT_HOME);
	GLCD_WriteData(GLCD_TEXT_HOME >> 8);
	GLCD_WriteCommand(T6963_SET_TEXT_HOME_ADDRESS);

	GLCD_WriteData(GLCD_TEXT_AREA);
	GLCD_WriteData(0x00);
	GLCD_WriteCommand(T6963_SET_TEXT_AREA);

	GLCD_WriteData(GLCD_OFFSET_REGISTER);
	GLCD_WriteData(0x00);
	GLCD_WriteCommand(T6963_SET_OFFSET_REGISTER);

	GLCD_WriteCommand(T6963_DISPLAY_MODE | T6963_GRAPHIC_DISPLAY_ON | T6963_TEXT_DISPLAY_ON /*| T6963_CURSOR_DISPLAY_ON*/);

	GLCD_WriteCommand(T6963_MODE_SET | 0);

}
