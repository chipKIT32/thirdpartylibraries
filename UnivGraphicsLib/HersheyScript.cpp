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



#include	"HersheyFonts.h"


#ifdef _ENABLE_HERSHEY_FONTS_
//*******************************************************************************
//*	Hershey font definition files
//*		these are put into .h files so they are more managable
//*******************************************************************************

#ifdef _ENABLE_HERSHEY_SCRIPT_SIMPLEX_
	#include	"HersheyScriptSimplex.h"
	//*******************************************************************************
	void	GetFontDef_ScriptSimplex(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyScriptSimplexFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyScriptSimplexFontTable, tableIndex);
	}
#endif

#ifdef _ENABLE_HERSHEY_SCRIPT_COMPLEX_
	#include	"HersheyScriptComplex.h"
	//*******************************************************************************
	void	GetFontDef_ScriptComplex(short tableIndex, char *fontDefString)
	{
	//	strcpy_P(fontDefString, (char*)READ_PTR_FROM_PGM(&(gHershyScriptComplexFontTable[tableIndex])));
		GetFontDefinitionString(fontDefString, gHershyScriptComplexFontTable, tableIndex);
	}
#endif





#endif		//*	_ENABLE_HERSHEY_FONTS_