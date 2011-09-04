/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  HIMAX HX8347 LCD controllers driver
 *****************************************************************************
 * FileName:        HX8347.h
 * Dependencies:    p24Fxxxx.h or plib.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2009 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     05/26/09
 *****************************************************************************/
#ifndef _HX8347_H
	#define _HX8347_H

	#ifdef __PIC32MX
		#include <plib.h>
		#define PMDIN1  PMDIN
	#else
		#ifdef __PIC24F__
			#include <p24Fxxxx.h>
		#else
			#error CONTROLLER IS NOT SUPPORTED
		#endif
	#endif
//	#include "GraphicsConfig.h"
//	#include "GenericTypeDefs.h"



/*********************************************************************
* Overview: Additional hardware-accelerated functions can be implemented
*		   in the driver. These definitions exclude the PutPixel()-based
*		   functions in the primitives layer (Primitive.c file) from compilation.
*********************************************************************/

// Define this to implement Font related functions in the driver.
//#define USE_DRV_FONT
// Define this to implement Line function in the driver.
//#define USE_DRV_LINE
// Define this to implement Circle function in the driver.
//#define USE_DRV_CIRCLE
// Define this to implement FillCircle function in the driver.
//#define USE_DRV_FILLCIRCLE
// Define this to implement Bar function in the driver.
	#define USE_DRV_BAR

// Define this to implement ClearDevice function in the driver.
	#define USE_DRV_CLEARDEVICE

// Define this to implement PutImage function in the driver.
	#define USE_DRV_PUTIMAGE

	#ifndef DISP_HOR_RESOLUTION
		#error DISP_HOR_RESOLUTION must be defined in HardwareProfile.h
	#endif
	#ifndef DISP_VER_RESOLUTION
		#error DISP_VER_RESOLUTION must be defined in HardwareProfile.h
	#endif
	#ifndef COLOR_DEPTH
		#error COLOR_DEPTH must be defined in HardwareProfile.h
	#endif
	#ifndef DISP_ORIENTATION
		#error DISP_ORIENTATION must be defined in HardwareProfile.h
	#endif

/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
	#if (DISP_HOR_RESOLUTION != 240)
		#error This driver doesnt supports this resolution. Horisontal resolution must be 240 pixels.
	#endif
	#if (DISP_VER_RESOLUTION != 320)
		#error This driver doesnt supports this resolution. Vertical resolution must be 320 pixels.
	#endif

/*********************************************************************
* Overview: Display orientation.
*********************************************************************/
	#if (DISP_ORIENTATION != 0) && (DISP_ORIENTATION != 90)
		#error This driver doesnt support this orientation.
    #endif

/*********************************************************************
* Overview: Color depth.
*********************************************************************/
    #if (COLOR_DEPTH != 16)
        #error This driver doesnt support this color depth. It should be 16.
    #endif

/*********************************************************************
* Overview: Clipping region control codes to be used with SetClip(...)
*           function. 
*********************************************************************/
    #define CLIP_DISABLE    0   // Disables clipping.
    #define CLIP_ENABLE     1   // Enables clipping.

/*********************************************************************
* Overview: Some basic colors definitions.
*********************************************************************/
    #define BLACK           RGB565CONVERT(0, 0, 0)
    #define BRIGHTBLUE      RGB565CONVERT(0, 0, 255)
    #define BRIGHTGREEN     RGB565CONVERT(0, 255, 0)
    #define BRIGHTCYAN      RGB565CONVERT(0, 255, 255)
    #define BRIGHTRED       RGB565CONVERT(255, 0, 0)
    #define BRIGHTMAGENTA   RGB565CONVERT(255, 0, 255)
    #define BRIGHTYELLOW    RGB565CONVERT(255, 255, 0)
    #define BLUE            RGB565CONVERT(0, 0, 128)
    #define GREEN           RGB565CONVERT(0, 128, 0)
    #define CYAN            RGB565CONVERT(0, 128, 128)
    #define RED             RGB565CONVERT(128, 0, 0)
    #define MAGENTA         RGB565CONVERT(128, 0, 128)
    #define BROWN           RGB565CONVERT(255, 128, 0)
    #define LIGHTGRAY       RGB565CONVERT(128, 128, 128)
    #define DARKGRAY        RGB565CONVERT(64, 64, 64)
    #define LIGHTBLUE       RGB565CONVERT(128, 128, 255)
    #define LIGHTGREEN      RGB565CONVERT(128, 255, 128)
    #define LIGHTCYAN       RGB565CONVERT(128, 255, 255)
    #define LIGHTRED        RGB565CONVERT(255, 128, 128)
    #define LIGHTMAGENTA    RGB565CONVERT(255, 128, 255)
    #define YELLOW          RGB565CONVERT(255, 255, 128)
    #define WHITE           RGB565CONVERT(255, 255, 255)
    #define GRAY0           RGB565CONVERT(224, 224, 224)
    #define GRAY1           RGB565CONVERT(192, 192, 192)
    #define GRAY2           RGB565CONVERT(160, 160, 160)
    #define GRAY3           RGB565CONVERT(128, 128, 128)
    #define GRAY4           RGB565CONVERT(96, 96, 96)
    #define GRAY5           RGB565CONVERT(64, 64, 64)
    #define GRAY6           RGB565CONVERT(32, 32, 32)

// Color
extern WORD     _color;

/*********************************************************************
* Overview: Clipping region control and border settings.
*
*********************************************************************/

// Clipping region enable control
extern SHORT    _clipRgn;

// Left clipping region border
extern SHORT    _clipLeft;

// Top clipping region border
extern SHORT    _clipTop;

// Right clipping region border
extern SHORT    _clipRight;

// Bottom clipping region border
extern SHORT    _clipBottom;

/*********************************************************************
* Macros:  PMPWaitBusy()
*
* Overview: Delays execution for PMP cycle time.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define PMPWaitBusy()   while(PMMODEbits.BUSY);

/*********************************************************************
* Macros:  SetIndex(index)
*
* Overview: Writes index register.
*
* PreCondition: none
*
* Input: index - Index register.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetIndex(index) \
    RS_LAT_BIT = 0;         \
    PMDIN1 = index;         \
    PMPWaitBusy();

/*********************************************************************
* Macros:  WriteCommand(cmd)
*
* Overview: Writes command.
*
* PreCondition: none
*
* Input: command.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define WriteCommand(cmd) \
    RS_LAT_BIT = 1;           \
    PMDIN1 = cmd;             \
    PMPWaitBusy();

/*********************************************************************
* Macros:  WriteData(data)
*
* Overview: Writes data
*
* PreCondition: none
*
* Input: data
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #ifdef USE_16BIT_PMP
        #define WriteData(data) \
    RS_LAT_BIT = 1;             \
    PMDIN1 = data;              \
    PMPWaitBusy();
    #else
        #define WriteData(data)                    \
    RS_LAT_BIT = 1;                                \
    PMDIN1 = ((WORD_VAL) data).v[1] & 0xF8;        \
    PMPWaitBusy();                                 \
    PMDIN1 = ((data >> 3) & 0xFC);                 \
    PMPWaitBusy();                                 \
    PMDIN1 = (((WORD_VAL) data).v[0] << 3) & 0xF8; \
    PMPWaitBusy();
    #endif

/*********************************************************************
* Macros:  SetAddress(x,y)
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
    #define SetAddress(x, y)                  \
    SetIndex(0x02);                           \
    WriteCommand(((WORD_VAL) (WORD) x).v[1]); \
    SetIndex(0x03);                           \
    WriteCommand(((WORD_VAL) (WORD) x).v[0]); \
    SetIndex(0x06);                           \
    WriteCommand(((WORD_VAL) (WORD) y).v[1]); \
    SetIndex(0x07);                           \
    WriteCommand(((WORD_VAL) (WORD) y).v[0]); \
    SetIndex(0x22);

/*********************************************************************
* Function:  void ResetDevice()
*
* Overview: Initializes LCD module.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void    ResetDevice(void);

/*********************************************************************
* Macros:  GetMaxX()
*
* Overview: Returns maximum horizontal coordinate.
*
* PreCondition: none
*
* Input: none
*
* Output: Maximum horizontal coordinate.
*
* Example:
*   <CODE> 
*
*	// Create a window that will occupy the whole screen.
*	WndCreate(0xFF,				    	// ID
*             0,0,
*			  GetMaxX(),GetMaxY(),		// dimension
*             WND_DRAW,					// will be dislayed after creation
*             (void*)&mchpIcon,         // use icon used
*             pText,	   				// set to text pointed to by pText
*             NULL);					// use default scheme 
*
*  </CODE>
*
* Side Effects: none
*
********************************************************************/
    #if (DISP_ORIENTATION == 90)
        #define GetMaxX()   (DISP_VER_RESOLUTION - 1)
    #elif (DISP_ORIENTATION == 0)
        #define GetMaxX()   (DISP_HOR_RESOLUTION - 1)
    #endif

/*********************************************************************
* Macros:  GetMaxY()
*
* Overview: Returns maximum vertical coordinate.
*
* PreCondition: none
*
* Input: none
*
* Output: Maximum vertical coordinate.
*
* Example: (see GetMaxX()) example.
*
* Side Effects: none
*
********************************************************************/
    #if (DISP_ORIENTATION == 90)
        #define GetMaxY()   (DISP_HOR_RESOLUTION - 1)
    #elif (DISP_ORIENTATION == 0)
        #define GetMaxY()   (DISP_VER_RESOLUTION - 1)
    #endif

/*********************************************************************
* Macros:  SetColor(color)
*
* Overview: Sets current drawing color.
*
* PreCondition: none
*
* Input: color - Color coded in 5:6:5 RGB format.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetColor(color) _color = color;

/*********************************************************************
* Macros:  GetColor()
*
* Overview: Returns current drawing color.
*
* PreCondition: none
*
* Input: none
*
* Output: Color coded in 5:6:5 RGB format.
*
* Side Effects: none
*
********************************************************************/
    #define GetColor()  _color

/*********************************************************************
* Macros:  SetActivePage(page)
*
* Overview: Sets active graphic page.
*
* PreCondition: none
*
* Input: page - Graphic page number.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetActivePage(page)

/*********************************************************************
* Macros: SetVisualPage(page)
*
* Overview: Sets graphic page to display.
*
* PreCondition: none
*
* Input: page - Graphic page number
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetVisualPage(page)

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* Overview: Puts pixel with the given x,y coordinate position.
*
* PreCondition: none
*
* Input: x - x position of the pixel.
*		 y - y position of the pixel.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void    PutPixel(SHORT x, SHORT y);

/*********************************************************************
* Function: WORD GetPixel(SHORT x, SHORT y)
*
* Overview: Returns pixel color at the given x,y coordinate position.
*
* PreCondition: none
*
* Input: x - x position of the pixel.
*		 y - y position of the pixel.
*
* Output: pixel color
*
* Side Effects: none
*
********************************************************************/
WORD    GetPixel(SHORT x, SHORT y);

/*********************************************************************
* Macros: SetClipRgn(left, top, right, bottom)
*
* Overview: Sets clipping region.
*
* PreCondition: none
*
* Input: left - Defines the left clipping region border.
*		 top - Defines the top clipping region border.
*		 right - Defines the right clipping region border.
*	     bottom - Defines the bottom clipping region border.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetClipRgn(left, top, right, bottom) \
    _clipLeft = left;                            \
    _clipTop = top;                              \
    _clipRight = right;                          \
    _clipBottom = bottom;

/*********************************************************************
* Macros: GetClipLeft()
*
* Overview: Returns left clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Left clipping border.
*
* Side Effects: none
*
********************************************************************/
    #define GetClipLeft()   _clipLeft

/*********************************************************************
* Macros: GetClipRight()
*
* Overview: Returns right clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Right clipping border.
*
* Side Effects: none
*
********************************************************************/
    #define GetClipRight()  _clipRight

/*********************************************************************
* Macros: GetClipTop()
*
* Overview: Returns top clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Top clipping border.
*
* Side Effects: none
*
********************************************************************/
    #define GetClipTop()    _clipTop

/*********************************************************************
* Macros: GetClipBottom()
*
* Overview: Returns bottom clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Bottom clipping border.
*
* Side Effects: none
*
********************************************************************/
    #define GetClipBottom() _clipBottom

/*********************************************************************
* Macros: SetClip(control)
*
* Overview: Enables/disables clipping.
*
* PreCondition: none
*
* Input: control - Enables or disables the clipping.
*			- 0: Disable clipping
*			- 1: Enable clipping
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetClip(control)    _clipRgn = control;

/*********************************************************************
* Macros: IsDeviceBusy()
*
* Overview: Returns non-zero if LCD controller is busy 
*           (previous drawing operation is not completed).
*
* PreCondition: none
*
* Input: none
*
* Output: Busy status.
*
* Side Effects: none
*
********************************************************************/
    #define IsDeviceBusy()  0

/*********************************************************************
* Macros: SetPalette(colorNum, color)
*
* Overview:  Sets palette register.
*
* PreCondition: none
*
* Input: colorNum - Register number.
*        color - Color.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
    #define SetPalette(colorNum, color)

/*********************************************************************
* Function:  void  DelayMs(WORD time)
*
* Overview: Delays execution on time specified in milliseconds.
*           The delay is correct only for 16MIPS.
*
* PreCondition: none
*
* Input: time - Delay in milliseconds.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void    DelayMs(WORD time);
#endif // _HX8347_H
