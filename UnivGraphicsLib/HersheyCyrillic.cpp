//*******************************************************************************
//*	
//*	                             Font handling for
//*	                             Sub-Processing 
//*                        for TouchShield library
//*	         by inthebitz @ antipastohw.blogspot.com & liquidware.com
//*	
//*******************************************************************************
//*	Jan 21,	2009	<MLS> Started working on large fonts
//*	Jan 22,	2009	<MLS> Got Hershey fonts fully working
//*	Jan 23,	2009	<MLS> Added font name as the first string
//*******************************************************************************


#if defined(__AVR__)
	#include	<avr/pgmspace.h>
#endif

#include	"HersheyFonts.h"


#ifdef _ENABLE_HERSHEY_FONTS_
//*******************************************************************************
//*	Hershey font definition files
//*		these are put into .h files so they are more managable
//*******************************************************************************



#ifdef _ENABLE_HERSHEY_CYRILLIC_
	#include	"HersheyCyrillic.h"
	//*******************************************************************************
	void	GetFontDef_Cyrillic(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyCyrillicFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyCyrillicFontTable, tableIndex);
	}


#include	"HersheyCyrillic1.h"
	//*******************************************************************************
	void	GetFontDef_Cyrillic1(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyCyrillic1FontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyCyrillic1FontTable, tableIndex);
	}

#endif




#endif		//*	_ENABLE_HERSHEY_FONTS_