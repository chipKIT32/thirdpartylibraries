/*********************************************************************
 *	HardwareProfile.h
 *	MikroE PIC32MX4 MMB Hardware specific definitions
 *
 *********************************************************************
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Lucio Di Jasio       12/08/09    MikroE PIC32MX4 MMB 
 ********************************************************************/
#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#define PIC32MX4_MMB    400
#define GRAPHICS_PICTAIL_VERSION 4
#define  Beep()   

//    #include <plib.h>
#ifndef _P32XXXX_H
	#include <p32xxxx.h>
#endif

/*********************************************************************
* GetSystemClock() returns system clock frequency.
*
* GetPeripheralClock() returns peripheral clock frequency.
*
********************************************************************/

#define GetSystemClock() 			(80000000ul)

/*********************************************************************
* Macro: #define	GetPeripheralClock() 
*
* Overview: This macro returns the peripheral clock frequency 
*			used in Hertz.
*			* value for PIC32 is <PRE>(GetSystemClock()/(1<<OSCCONbits.PBDIV)) </PRE>
*
********************************************************************/
#define	GetPeripheralClock()		(GetSystemClock()/(1<<OSCCONbits.PBDIV))
/*********************************************************************

/*********************************************************************
* Overview:
*   Graphics Panel on the MikroE- PIC32 MX4 MMB Board 
*                                        
********************************************************************/
#define GRAPHICS_HARDWARE_PLATFORM      PIC32MX4_MMB
#define DISPLAY_CONTROLLER				HX8347D
/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_HOR_RESOLUTION				240
#define DISP_VER_RESOLUTION				320
/*********************************************************************
* Overview: Defines color depth. The 1,2,4,8,16 bit per pixel 
*			color depths are valid.
*
********************************************************************/
#define COLOR_DEPTH						16
#define USE_16BIT_PMP    

/*********************************************************************
* Overview: Image orientation (can be 0, 90, 180, 270 degrees).
*********************************************************************/
#define DISP_ORIENTATION				90

/*********************************************************************
* IOS FOR THE DISPLAY CONTROLLER
*********************************************************************/
#define ADC_POT         8   // temperature sensor input
// Definitions for reset pin
#define RST_TRIS_BIT        _TRISC1
#define RST_LAT_BIT         _LATC1

// Definitions for RS pin
#define RS_TRIS_BIT         _TRISB15
#define RS_LAT_BIT          _LATB15

// Definitions for CS pin
#define CS_TRIS_BIT         _TRISF12
#define CS_LAT_BIT          _LATF12

// Definitions for Backlight power pin 
#define BL_TRIS_BIT         _TRISD2
#define BL_LAT_BIT          _LATD2

// RF module interface
#define RF_CS               _LATD8
#define RF_CS_TRIS          _TRISD8

// Display Backlight 
#define BL_CS               _LATD2
#define BL_CS_TRIS          _TRISD2

#define BacklightOn()    BL_CS = 1
#define BacklightOff()   BL_CS = 0

/*********************************************************************
* IOS FOR THE TOUCH SCREEN
*********************************************************************/
#define ADC_XPOS        ADC_CH0_POS_SAMPLEA_AN11
#define ADC_YPOS        ADC_CH0_POS_SAMPLEA_AN10

// X port definitions
#define ADPCFG_XPOS     _PCFG13
#define LAT_XNEG        _LATB11
#define LAT_XPOS        _LATB13
#define TRIS_XNEG       _TRISB11
#define TRIS_XPOS       _TRISB13

// Y port definitions
#define ADPCFG_YPOS     AD1PCFGbits.PCFG12
#define LAT_YNEG        _LATB10  
#define LAT_YPOS        _LATB12 
#define TRIS_YNEG       _TRISB10
#define TRIS_YPOS       _TRISB12 


/*********************************************************************
* IOS FOR SERIAL FLASH 
*********************************************************************/
#define CS_FLASH_TRIS_BIT   _TRISC2
#define CS_FLASH_LAT_BIT    _LATC2

/*********************************************************************
* IOS FOR RF MODULE
*********************************************************************/
#define CS_RF_TRIS_BIT      _TRISD8
#define CS_RF_LAT_BIT       _LATD8

/*********************************************************************
* IOS FOR ACCELEROMETER
*********************************************************************/
#define CS_ACC_TRIS_BIT     _TRISC3
#define CS_ACC_LAT_BIT      _LATC3

/*********************************************************************
* IOS FOR THE Joystick
*********************************************************************/
#define BTN_A       _RB0
#define BTN_B       _RB1
#define BTN_C       _RB2
#define BTN_D       _RB3
#define BTN_CP      _RA10

// joystick switch assignement
#if (DISP_ORIENTATION == 0)     // portrait
// vertical 
#define JOY_SELECT  16
#define JOY_UP      8
#define JOY_RIGHT   4
#define JOY_DOWN    2
#define JOY_LEFT    1

#else                           // landscape
// horizontal
#define JOY_SELECT  16
#define JOY_UP      4
#define JOY_RIGHT   2
#define JOY_DOWN    1
#define JOY_LEFT    8
#endif

/*********************************************************************
* IOS FOR THE LEDs
*********************************************************************/
#define LD0_TRIS    _TRISA0
#define LD1_TRIS    _TRISA1
#define LD2_TRIS    _TRISA2
#define LD3_TRIS    _TRISA3
#define LD0         _LATA0
#define LD1         _LATA1
#define LD2         _LATA2
#define LD3         _LATA3

// LED logic is inverted
#define LED_ON      0
#define LED_OFF     1


/*********************************************************************
* IOS FOR THE SD/MMC CARD INTERFACE
*********************************************************************/

#define USE_SD_INTERFACE_WITH_SPI

    // Registers for the SPI module 
    #define MDD_USE_SPI_2

	// MDD SPI Configuration
	#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
    #define SPI_START_CFG_2     (SPI_ENABLE)

    // Define the SPI frequency
    #define SPI_FREQUENCY			(20000000)


    // Description: SD-SPI Chip Select Output bit
    #define SD_CS               _LATG9
    // Description: SD-SPI Chip Select TRIS bit
    #define SD_CS_TRIS          _TRISG9

    // Description: SD-SPI Card Detect Input bit
    #define SD_CD               _RA6
    // Description: SD-SPI Card Detect TRIS bit
    #define SD_CD_TRIS          _TRISA6

    // Description: SD-SPI Write Protect Check Input bit
    #define SD_WE               _RA7
    // Description: SD-SPI Write Protect Check TRIS bit
    #define SD_WE_TRIS          _TRISA7

    // Description: The main SPI control register
    #define SPICON1             SPI2CON
    // Description: The SPI status register
    #define SPISTAT             SPI2STAT
    // Description: The SPI Buffer
    #define SPIBUF              SPI2BUF
    // Description: The receive buffer full bit in the SPI status register
    #define SPISTAT_RBF         SPI2STATbits.SPIRBF
    // Description: The bitwise define for the SPI control register (i.e. _____bits)
    #define SPICON1bits         SPI2CONbits
    // Description: The bitwise define for the SPI status register (i.e. _____bits)
    #define SPISTATbits         SPI2STATbits
    // Description: The enable bit for the SPI module
    #define SPIENABLE           SPI2CONbits.ON
    // Description: The definition for the SPI baud rate generator register (PIC32)
    #define SPIBRG			    SPI2BRG

    // Tris pins for SCK/SDI/SDO lines

    // Description: The TRIS bit for the SCK pin
    #define SPICLOCK            TRISGbits.TRISG6
    // Description: The TRIS bit for the SDI pin
    #define SPIIN               TRISGbits.TRISG7
    // Description: The TRIS bit for the SDO pin
    #define SPIOUT              TRISGbits.TRISG8
    
    //SPI library functions
    #define putcSPI             putcSPI2
    #define getcSPI             getcSPI2
    #define OpenSPI(config1, config2)   OpenSPI2(config1, config2)

    // Will generate an error if the clock speed is too low to interface to the card
    #if (GetSystemClock() < 100000)
        #error Clock speed must exceed 100 kHz
    #endif


#endif // __HARDWARE_PROFILE_H
