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



#ifdef _ENABLE_HERSHEY_GOTHIC_ENGLISH_
	#include	"HersheyGothicEnglish.h"
	//*******************************************************************************
	void	GetFontDef_GothicEnglish(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyGothicEngFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyGothicEngFontTable, tableIndex);
	}
#endif

#ifdef _ENABLE_HERSHEY_GOTHIC_GERMAN_
	#include	"HersheyGothicGerman.h"
	//*******************************************************************************
	void	GetFontDef_GothicGerman(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyGothicGermanFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyGothicGermanFontTable, tableIndex);
	}
#endif


#ifdef _ENABLE_HERSHEY_GOTHIC_ITALIAN_
	#include	"HersheyGothicItalian.h"
	//*******************************************************************************
	void	GetFontDef_GothicItalian(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyGothicItalianFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyGothicItalianFontTable, tableIndex);
	}
#endif


#ifdef _ENABLE_HERSHEY_GOTHIC_GBT_
	#include	"HersheyGothgbt.h"
	//*******************************************************************************
	void	GetFontDef_GothicGBT(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyGothgbtFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyGothgbtFontTable, tableIndex);
	}
#endif

#ifdef _ENABLE_HERSHEY_GOTHITT_
	#include	"HersheyGothitt.h"
	//*******************************************************************************
	void	GetFontDef_Gothitt(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyGothittFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyGothittFontTable, tableIndex);
	}
#endif

#endif		//*	_ENABLE_HERSHEY_FONTS_