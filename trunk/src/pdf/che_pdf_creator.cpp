#include "../../include/pdf/che_pdf_creator.h"
#include "../../include/pdf/che_pdf_xref.h"

#include <cstdio>
#include <math.h>

//string resouces for creator
//file structure relative
const HE_CHAR * gpStrPDFVersion17 = "%PDF-1.7\n";		HE_DWORD glStrPDFVersion = 9;
const HE_CHAR * gpStrPDFVersion16 = "%PDF-1.6\n";
const HE_CHAR * gpStrPDFVersion15 = "%PDF-1.5\n";
const HE_CHAR * gpStrPDFVersion14 = "%PDF-1.4\n";
const HE_CHAR * gpStrPDFVersion13 = "%PDF-1.3\n";
const HE_CHAR * gpStrPDFVersion12 = "%PDF-1.2\n";
const HE_CHAR * gpStrPDFVersion11 = "%PDF-1.1\n";
const HE_CHAR * gpStrPDFVersion10 = "%PDF-1.0\n";
const HE_CHAR	* gpStrNewLine = "\n";					const HE_DWORD glStrNewLine = 1;
const HE_CHAR * gpStrObjBegin = "obj\n";				const HE_DWORD glStrObjBegin = 4;
const HE_CHAR * gpStrObjEnd = "\nendobj\n";				const HE_DWORD glStrObjEnd = 8;
const HE_CHAR * gpStrXrefMark = "xref\n";				const HE_DWORD glstrXrefMark = 5;
const HE_CHAR * gpStrXrefFirstFreeEntry = "0000000000 65535 f \n";	const HE_DWORD glStrXrefEntry = 20;
const HE_CHAR * gpStrTrailerMark = "trailer\n";			const HE_DWORD glStrTrailerMark = 8;
const HE_CHAR * gpStrXrefStartMark = "startxref\n";		const HE_DWORD glStrXrefStartMark = 10;
const HE_CHAR * gpStrFileEnd = "%%EOF";					const HE_DWORD glStrFileEnd = 5;
const HE_CHAR * gpStrSingleSpace = " ";					const HE_DWORD glStrSingleSpace = 1;
//obj relative
const HE_CHAR * gpStrNullObj = "null";					const HE_DWORD glStrNullObj = 5;
const HE_CHAR * gpStrBoolObjFalse = "false";			const HE_DWORD glStrBoolObjFalse = 5;
const HE_CHAR * gpStrBoolObjTrue = "true";				const HE_DWORD glStrBoolObjTrue = 4;
const HE_CHAR * gpStrNameObjPre = "/";					const HE_DWORD glStrNameObjPre = 1;
const HE_CHAR * gpStrStrObjLeft = "(";					const HE_DWORD glStrStrObj = 1;
const HE_CHAR * gpStrStrObjRight = ")";
const HE_CHAR * gpStrArrayObjLeft = "[";				const HE_DWORD glStrArrayObj = 1;
const HE_CHAR * gpStrArrayObjRight = "]";
const HE_CHAR * gpStrDictObjLeft = "<<";				const HE_DWORD glStrDictObj = 2;
const HE_CHAR * gpStrDictObjRight = ">>";
const HE_CHAR * gpStrStreamObjBegin = "stream\n";		const HE_DWORD glStrStreamObjBegin = 7;
const HE_CHAR * gpStrStreamObjEnd = "\nendstream";		const HE_DWORD glStrStreamObjEnd = 10;

CHE_PDF_Creator * CHE_PDF_Creator::Create( IHE_Write * pWrite, CHE_Allocator * pAllocator )
{
	if ( pWrite == NULL )
	{
		return NULL;
	}
	if ( pAllocator == NULL )
	{
		pAllocator == GetDefaultAllocator();
	}
	return pAllocator->New<CHE_PDF_Creator>( pWrite, pAllocator );
}

CHE_PDF_Creator::CHE_PDF_Creator( IHE_Write * pWrite, CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpWrite( pWrite ), mpEncrypt( NULL ) {}
	
CHE_PDF_Creator::~CHE_PDF_Creator()
{
	if ( mpEncrypt )
	{
		mpEncrypt->GetAllocator()->Delete( mpEncrypt );
	}
}

// HE_BOOL CHE_PDF_Creator::SaveUpdate( IHE_Write * pWrite, CHE_PDF_Parser * pParser )
// {
// 	if ( pWrite == NULL || pParser == NULL )
// 	{
// 		return FALSE;
// 	}
// 	HE_CHAR tempStr[1024];
// 	CHE_PDF_XREF_Table xrefTable;
// 	CHE_PDF_Object * pObj = NULL;
// 	CHE_PDF_IndirectObject * pInObj = NULL;
// 	HE_DWORD lObjCount = pParser->GetUpdatedInObjCount();
// 	if ( lObjCount > 0 )
// 	{
// 		for ( HE_DWORD i = 0; i < lObjCount; i++ )
// 		{
// 			pInObj = pParser->GetUpdatedInObj( i );
// 			if ( pInObj != NULL )
// 			{
// 				pObj = pInObj->GetObj();
// 				xrefTable.NewSection( pInObj->GetObjNum() );
// 				CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, pWrite->GetCurOffset(), 0 );
// 				xrefTable.NewNode( tmpEntry );
// 				sprintf( tempStr, "%d %d obj\n", pInObj->GetObjNum(), 0 );
// 				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
// 				OutPutObject( pObj, pWrite );
// 				pWrite->WriteBlock( (HE_LPVOID)gpStrObjEnd, glStrObjEnd );
// 			}
// 		}
// 	}
// 
// 	HE_DWORD xrefOffset = pWrite->GetCurOffset();
// 
// 	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefMark, glstrXrefMark );
// 
// 	sprintf( tempStr, "%d %d\n", 0, 1 );
// 	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
// 	pWrite->WriteBlock(  (HE_LPVOID)gpStrXrefFirstFreeEntry, glStrXrefEntry );
// 
// 	CHE_PDF_XREF_Entry tmpEntry;
// 	for ( HE_DWORD i = 1; i <= xrefTable.GetMaxObjNum(); i++ )
// 	{
// 		if ( xrefTable.GetEntry( i, tmpEntry ) == TRUE )
// 		{
// 			sprintf( tempStr, "%d %d\n", i, 1 );
// 			pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
// 			sprintf( tempStr, "%010d %05d n \n", tmpEntry.GetOffset(), 0 );
// 			pWrite->WriteBlock( (HE_LPVOID)tempStr, 20 );
// 		}
// 	}
// 
// 	CHE_PDF_Dictionary * pTrailerDict = pParser->GetTrailerDict();
// 	if ( pTrailerDict )
// 	{
// 		pTrailerDict->SetAtInteger( "Prev", pParser->m_lStartxref );
// 
// 		//Ð´Î²×Öµä
// 		pWrite->WriteBlock( (HE_LPVOID)gpStrTrailerMark, glStrTrailerMark );
// 		OutPutObject( pTrailerDict, pWrite );
// 	}
// 
// 	pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );
// 	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefStartMark, glStrXrefStartMark );
// 	sprintf( tempStr, "%d\n", xrefOffset );
// 	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
// 	pWrite->WriteBlock( (HE_LPVOID)gpStrFileEnd, glStrFileEnd );
// 
// 	pWrite->Flush();
// 
// 	return TRUE;
// }

// CHE_PDF_IndirectObject * CHE_PDF_Creator::AddType1Font_Standard14( HE_BYTE fontType, PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/ )
// {
// 	if ( m_bNewDocument != TRUE )
// 	{
// 		return NULL;
// 	}
// 	if ( fontType > 13 || fontType < 0 )
// 	{
// 		return NULL;
// 	}
// 	CHE_PDF_IndirectObject * pInFontDictObj = AppendIndirectObj_Dict();
// 	CHE_PDF_Dictionary * pFontDict = pInFontDictObj->GetObj()->ToDict();
// 	if ( pFontDict == NULL )
// 	{
// 		return NULL;
// 	}
// 	pFontDict->SetAtName( "Type", "Font" );
// 	pFontDict->SetAtName( "Subtype", "Type1" );	
// 	switch( fontType )
// 	{
// 	case FONT_TYPE1_STANDARD14_TIMES_ROMAN:
// 		pFontDict->SetAtName( "BaseFont", "Times-Roman" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_TIMES_BOLD:
// 		pFontDict->SetAtName( "BaseFont", "Times-Bold" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_TIMES_ITALIC:
// 		pFontDict->SetAtName( "BaseFont", "Times-Italic" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_TIMES_BOLDITALIC:
// 		pFontDict->SetAtName( "BaseFont", "Times-BoldItalic" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_HELVETICA:
// 		pFontDict->SetAtName( "BaseFont", "Helvetica" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_HELVETICA_BOLD:
// 		pFontDict->SetAtName( "BaseFont", "Helvetica-Bold" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_HELVETICA_OBILQUE:
// 		pFontDict->SetAtName( "BaseFont", "Helvetica-Obilque" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_HELVETICA_BOLDOBILQUE:
// 		pFontDict->SetAtName( "BaseFont", "Helvetica-BoldObilque" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_COURIER:
// 		pFontDict->SetAtName( "BaseFont", "Courier" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_COURIER_BOLD:
// 		pFontDict->SetAtName( "BaseFont", "Courier-Bold" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_COURIER_OBILQUE:
// 		pFontDict->SetAtName( "BaseFont", "Courier-Obilque" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_COURIER_BOLDOBILQUE:
// 		pFontDict->SetAtName( "BaseFont", "Courier-BoldObilque" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_SYMBOL:
// 		pFontDict->SetAtName( "BaseFont", "Symbol" );
// 		break;
// 	case FONT_TYPE1_STANDARD14_ZAPFDINGBATS:
// 		pFontDict->SetAtName( "BaseFont", "ZapfDingbats" );
// 		break;
// 	default:
// 		break;
// 	}
// 	CHE_PDF_IndirectObject * pInWidthArrayObj = AppendIndirectObj_Array();
// 	CHE_PDF_Array * pWidthsArr = pInWidthArrayObj->GetObj()->ToArray();
// 	if ( pWidthsArr )
// 	{
// 		for ( HE_DWORD i = 0; i < 256; i++ )
// 		{
// 			CHE_PDF_Number * pNumber = CHE_PDF_Number::Create( (HE_INT32)1000, GetAllocator() );
// 			pWidthsArr->Append( pNumber );
// 		}
// 		pFontDict->SetAtInteger( "FirstChar", 0 );
// 		pFontDict->SetAtInteger( "LastChar", 255 );
// 		pFontDict->SetAtReference( "Widths", pInWidthArrayObj->GetObjNum(), pInWidthArrayObj->GetGenNum(), NULL );
// 	}
// 	switch ( encoding )
// 	{
// 	case FONT_ENCODING_WINANSI:
// 		pFontDict->SetAtName( "Encoding", "WinAnsiEncoding" );
// 		break;
// 	case FONT_ENCODING_MACROMAN:
// 		pFontDict->SetAtName( "Encoding", "MacRomanEncoding" );
// 		break;
// 	case FONT_ENCODING_MACEXPERT:
// 		pFontDict->SetAtName( "Encoding", "MacExpertEncoding" );
// 		break;
// 	default:
// 		break;
// 	}
// 	return pInFontDictObj;
// }
// 
// 
// CHE_PDF_IndirectObject * CHE_PDF_Creator::AddType1Font(	const CHE_ByteString & baseFont,
// 														HE_DWORD firstChar /*= 0*/, HE_DWORD lastChar /*= 255*/,
// 														PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSI*/,
// 														CHE_PDF_Array * pWidths /*= NULL*/,
// 														CHE_PDF_Dictionary * pFontDescriptor /*= NULL*/,
// 														CHE_PDF_IndirectObject * pToUnicodeStream /*= NULL*/ )
// {
// 	if ( !m_bNewDocument || baseFont.GetLength() == 0 )
// 	{
// 		return NULL;
// 	}
// 	CHE_PDF_IndirectObject * pInFontDictObj = AppendIndirectObj_Dict();
// 	CHE_PDF_Dictionary * pFontDict = pInFontDictObj->GetObj()->ToDict();
// 	if ( pFontDict == NULL )
// 	{
// 		return NULL;
// 	}
// 	pFontDict->SetAtName( "Type", "Font" );
// 	pFontDict->SetAtName( "Subtype", "Type1" );
// 	pFontDict->SetAtName( "BaseFont", baseFont );
// 	pFontDict->SetAtInteger( "FirstChar", 0 );
// 	pFontDict->SetAtInteger( "LastChar", 255 );
// 	switch( encoding )
// 	{
// 	case FONT_ENCODING_WINANSI:
// 		pFontDict->SetAtName( "Encoding", "WinAnsiEncoding" );
// 		break;
// 	case FONT_ENCODING_MACROMAN:
// 		pFontDict->SetAtName( "Encoding", "MacRomanEncoding" );
// 		break;
// 	case FONT_ENCODING_MACEXPERT:
// 		pFontDict->SetAtName( "Encoding", "MacExpertEncoding" );
// 		break;
// 	default:
// 		break;
// 	}
// 	if ( pWidths )
// 	{
// 		pFontDict->SetAtArray( "Widths", pWidths );
// 	}
// 	if ( pFontDescriptor )
// 	{
// 		pFontDict->SetAtDictionary( "FontDescriptor", pFontDescriptor );
// 	}
// 	if ( pToUnicodeStream )
// 	{
// 		pFontDict->SetAtReference( "ToUnicode", pToUnicodeStream->GetObjNum(), pToUnicodeStream->GetGenNum(), NULL );
// 	}
// 	return pInFontDictObj;
// }

// CHE_PDF_Dictionary* CHE_PDF_Creator::AddTrueTypeFont(	const CHE_ByteString & baseFont, 
// 														HE_DWORD firstChar/* = 0*/, HE_DWORD lastChar/* = 255*/,
// 														PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/,
// 														const CHE_PDF_Array * pWidths/* = NULL*/, 
// 														const CHE_PDF_Dictionary * pFontDescriptor/* = NULL*/,
// 														const CHE_PDF_Stream * pToUnicode/* = NULL*/ )
// {
// 	if ( m_bNewDocument != TRUE || baseFont.GetLength() == 0 )
// 	{
// 		return NULL;
// 	}
// 	CHE_PDF_Dictionary * pDict = AppendIndirectObj_Dict();
// 	if ( pDict == NULL )
// 	{
// 		return NULL;
// 	}
// 	pDict->SetAtName( "Type", "Font" );
// 	pDict->SetAtName( "Subtype", "TrueType" );
// 	pDict->SetAtName( "BaseFont", baseFont );
// 	pDict->SetAtInteger( "FirstChar", firstChar );
// 	pDict->SetAtInteger( "LastChar", lastChar );
// 	switch( encoding )
// 	{
// 	case FONT_ENCODING_WINANSI:
// 		pDict->SetAtName( "Encoding", "WinAnsiEncoding" );
// 		break;
// 	case FONT_ENCODING_MACROMAN:
// 		pDict->SetAtName( "Encoding", "MacRomanEncoding" );
// 		break;
// 	case FONT_ENCODING_MACEXPERT:
// 		pDict->SetAtName( "Encoding", "MacExpertEncoding" );
// 		break;
// 	default:
// 		break;
// 	}
// 	if ( pWidths )
// 	{
// 		pDict->SetAtReference( "Widths", pWidths->GetObjNum() );
// 	}
// 	if ( pFontDescriptor )
// 	{
// 		pDict->SetAtReference( "FontDescriptor", pWidths->GetObjNum() );
// 	}
// 	if ( pToUnicode )
// 	{
// 		pDict->SetAtReference( "FontDescriptor", pToUnicode->GetObjNum() );
// 	}
// 	return pDict;
// }

// CHE_PDF_Dictionary* CHE_PDF_Creator::AddTrueTypeFont( const char * pFontFile, HE_BOOL bEmbed/* = FALSE*/ )
// {
// 	if( pFontFile == NULL )
// 		return NULL;
// 
// 	FT_Error eRet = 0;
// 	FT_Library ftLib = NULL;
// 	FT_Face ftFace = NULL;
// 	eRet = FT_Init_FreeType( &ftLib );
// 	if ( eRet != 0 )
// 	{
// 		return NULL;
// 	}
// 	eRet = FT_New_Face( ftLib, pFontFile, 0, &ftFace );
// 	if ( eRet != 0 )
// 	{
// 		FT_Done_FreeType( ftLib );
// 		return NULL;
// 	}
// 	const char * pFontName = FT_Get_Postscript_Name( ftFace );
// 	if ( pFontName == NULL )
// 	{
// 		FT_Done_Face( ftFace );
// 		FT_Done_FreeType( ftLib );
// 		return NULL;
// 	}
// 	CHE_PDF_Dictionary * pFontDict = AddTrueTypeFont( pFontName, 0, 0 );
// 	if ( pFontDict == NULL )
// 	{
// 		FT_Done_Face( ftFace );
// 		FT_Done_FreeType( ftLib );
// 		return NULL;
// 	}
// 	HE_FLOAT tmpF = ftFace->units_per_EM * 1.0 / 1000;
// 	CHE_PDF_Array * pArr = CHE_PDF_Array::Create( pFontDict->GetObjNum(), 0, NULL, pFontDict->GetAllocator() );
// 	CHE_PDF_Number * pWidth = CHE_PDF_Number::Create( (HE_INT32)( ftFace->max_advance_width/tmpF + 0.5 ), pFontDict->GetObjNum(), 0, NULL, pFontDict->GetAllocator() );
// 	pArr->Append( pWidth );
// 	pFontDict->SetAtArray( "Widths", pArr );
// 	CHE_PDF_Dictionary * pFontDescriptor = AddDictionary();
// 	if ( pFontDescriptor )
// 	{
// 		pFontDict->SetAtReference( "FontDescriptor", pFontDescriptor->GetObjNum() );
// 		pFontDescriptor->SetAtName( "Type", "FontDescriptor" );
// 		pFontDescriptor->SetAtName( "FontName", pFontName );
// 		
// 		CHE_PDF_Array * pArr = CHE_PDF_Array::Create( pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
// 		CHE_PDF_Number* f1 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.xMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
// 		CHE_PDF_Number* f2 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.yMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
// 		CHE_PDF_Number* f3 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.xMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
// 		CHE_PDF_Number* f4 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.yMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
// 		pArr->Append( f1 );
// 		pArr->Append( f2 );
// 		pArr->Append( f3 );
// 		pArr->Append( f4 );
// 		pFontDescriptor->SetAtArray( "FontBBox", pArr );
// 		pFontDescriptor->SetAtInteger( "Ascent", ftFace->ascender/tmpF + 0.5 );
// 		pFontDescriptor->SetAtInteger( "Descent", ftFace->descender/tmpF + 0.5 );
// 		pFontDescriptor->SetAtInteger( "CapHeight", 500 );
// 		pFontDescriptor->SetAtInteger( "MissingWidth", ftFace->max_advance_width/tmpF + 0.5 );
// 		pFontDescriptor->SetAtInteger( "Flags", 33 );
// 		pFontDescriptor->SetAtInteger( "ItalicAngle", 0 );
// 		pFontDescriptor->SetAtInteger( "StemV", 80 );
// 
// 		if ( bEmbed == TRUE )
// 		{
// 			IHE_Read * pRead = HE_CreateFileRead( pFontFile );
// 			if ( pRead != NULL )
// 			{
// 				HE_LPBYTE pByte = GetAllocator()->NewArray<HE_BYTE>( pRead->GetSize() );
// 				pRead->ReadBlock( pByte, 0, pRead->GetSize() );
// 				CHE_PDF_Stream * pStm = AddStream();
// 				pFontDescriptor->SetAtReference( "FontFile2", pStm->GetObjNum() );
// 				pStm->SetRawData( pByte, pRead->GetSize(), STREAM_FILTER_FLATE );
// 				pStm->GetDict()->SetAtInteger( "Length1", pRead->GetSize() );
// 				HE_DestoryIHERead( pRead );
// 				GetAllocator()->DeleteArray<HE_BYTE>( pByte );
// 			}
// 		}
// 	}
// 	FT_Done_Face( ftFace );
// 	FT_Done_FreeType( ftLib );
// 	return pFontDict;
// }

HE_VOID CHE_PDF_Creator::OutPutFileHead( PDF_VERSION version )
{
	switch( version )
	{
	case PDF_VERSION_1_0:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion10, glStrPDFVersion );
		break;
	case PDF_VERSION_1_1:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion11, glStrPDFVersion );
		break;
	case PDF_VERSION_1_2:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion12, glStrPDFVersion );
		break;
	case PDF_VERSION_1_3:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion13, glStrPDFVersion );
		break;
	case PDF_VERSION_1_4:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion14, glStrPDFVersion );
		break;
	case PDF_VERSION_1_5:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion15, glStrPDFVersion );
		break;
	case PDF_VERSION_1_6:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion16, glStrPDFVersion );
		break;
	case PDF_VERSION_1_7:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion17, glStrPDFVersion );
		break;
	default:
		mpWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion17, glStrPDFVersion );
		break;
	}
	mpWrite->WriteBlock( (HE_LPVOID)"%\255\255\255\255\n", 6 );
}

HE_DWORD CHE_PDF_Creator::OutPutInObject( CHE_PDF_IndirectObject * pInObj )
{
	if ( pInObj == NULL )
	{
		return 0;
	}
	HE_DWORD offset = mpWrite->GetCurOffset();
	HE_CHAR tempStr[128];
	sprintf( tempStr, "%d %d obj\n", pInObj->GetObjNum(), pInObj->GetGenNum() );
	mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
	OutPutObject( pInObj->GetObj() );
	mpWrite->WriteBlock( (HE_LPVOID)gpStrObjEnd, glStrObjEnd );
	return offset;
}

HE_DWORD CHE_PDF_Creator::OutPutXRefTable( const CHE_PDF_XREF_Data & xref )
{
	HE_DWORD offset = mpWrite->GetCurOffset();

	mpWrite->WriteBlock( (HE_LPVOID)gpStrXrefMark, glstrXrefMark );

	HE_CHAR tempStr[128];
	PDF_XREF_SECTION * pTemSec = xref.mpFirstSec;

	while ( pTemSec )
	{
		sprintf( tempStr, "%d %d\n", pTemSec->lBeginNum, pTemSec->lCount );
		mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );

		PDF_XREF_ENTRY_NODE * pTmpNode = pTemSec->pFirstEntry;
		while ( pTmpNode )
		{
			if ( pTmpNode->entry.GetType() == XREF_ENTRY_TYPE_FREE )
			{
				sprintf( tempStr, "%010d %05d f \n", 0, pTmpNode->entry.GetGenNum() );
			}else{
				sprintf( tempStr, "%010d %05d n \n", pTmpNode->entry.GetOffset(), pTmpNode->entry.GetGenNum() );
			}
			mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
			pTmpNode = pTmpNode->pNext;
		}
		pTemSec = pTemSec->pNextSec;
	}
	return offset;
}

HE_VOID	CHE_PDF_Creator::OutPutFileTailer( HE_DWORD startxref )
{
	HE_CHAR tempStr[128];
	mpWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );
	mpWrite->WriteBlock( (HE_LPVOID)gpStrXrefStartMark, glStrXrefStartMark );
	sprintf( tempStr, "%d\n", startxref );
	mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
	mpWrite->WriteBlock( (HE_LPVOID)gpStrFileEnd, glStrFileEnd );
}

HE_VOID CHE_PDF_Creator::OutPutTailerDict( CHE_PDF_Dictionary * pDcit )
{
	mpWrite->WriteBlock( (HE_LPVOID)gpStrTrailerMark, glStrTrailerMark );
	OutPutObject( pDcit );
}

HE_VOID CHE_PDF_Creator::OutPutObject( CHE_PDF_Object * pObj )
{
	if ( pObj == NULL )
	{
		return;
	}
	static HE_CHAR tempStr[1024];
	switch( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			mpWrite->WriteBlock( (HE_LPVOID)gpStrNullObj, glStrNullObj );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			if( ((CHE_PDF_Boolean*)pObj)->GetValue() != FALSE )
			{
				mpWrite->WriteBlock( (HE_LPVOID)gpStrBoolObjTrue, glStrBoolObjTrue );
			}else{
				mpWrite->WriteBlock( (HE_LPVOID)gpStrBoolObjFalse, glStrBoolObjFalse );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			HE_LPVOID pData = (HE_LPVOID)( ((CHE_PDF_String*)pObj)->GetString().GetData() );
			HE_DWORD length = ((CHE_PDF_String*)pObj)->GetString().GetLength();
			HE_BOOL bHex = FALSE;
			for ( HE_DWORD i = 0; i < length; i++ )
			{
				if ( pObj->ToString()->GetString()[i] == ')' )
				{
					bHex  = TRUE;
					break;
				}
			}
			if ( bHex == FALSE )
			{
				mpWrite->WriteBlock( (HE_LPVOID)gpStrStrObjLeft, glStrStrObj );
				mpWrite->WriteBlock( pData, length );
				mpWrite->WriteBlock( (HE_LPVOID)gpStrStrObjRight, glStrStrObj );
			}else{
				mpWrite->WriteBlock( (HE_LPVOID)"<", 1 );
				HE_CHAR tmpByte[32];
				HE_DWORD tmpVal = 0;
				for ( HE_DWORD i = 0; i < length; i++ )
				{
					tmpVal = (pObj->ToString()->GetString())[i];
					sprintf( tmpByte, "%08X", tmpVal );
					mpWrite->WriteBlock( (HE_LPVOID)(tmpByte+6), 2 );
				}
				mpWrite->WriteBlock( (HE_LPVOID)">", 1 );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			HE_BYTE * pData = (HE_BYTE*)( ((CHE_PDF_Name*)pObj)->GetString().GetData() );
			HE_DWORD length = ((CHE_PDF_String*)pObj)->GetString().GetLength();
			mpWrite->WriteBlock( (HE_LPVOID)gpStrNameObjPre, 1 );
			char tmpStr[16];
			for ( HE_DWORD i = 0; i < length; ++i )
			{
				if ( 32 < pData[i] && pData[i] < 127 )
				{
					mpWrite->WriteBlock( (HE_LPVOID*)(pData+i), 1 );
				}else{
					sprintf( tmpStr, "#%02X", pData[i] ); 
					mpWrite->WriteBlock( (HE_LPVOID*)tmpStr, 3 );
				}
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			if ( ((CHE_PDF_Number*)pObj)->IsInteger() == TRUE )
			{
				sprintf( tempStr, "%d", ((CHE_PDF_Number*)pObj)->GetInteger() );
				mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}else{
				float value = ((CHE_PDF_Number*)pObj)->GetFloat();
				sprintf( tempStr, "%g", value );
				if ( strchr( tempStr, 'e' ) )
				{
					sprintf( tempStr, (fabsf(value))>1? "%1.1f":"%1.8f", value );
				}
				mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			sprintf( tempStr, "%d 0 R", ((CHE_PDF_Reference*)pObj)->GetRefNum() );
			mpWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_Array * pArray = (CHE_PDF_Array*)pObj;
			CHE_PDF_Object * pElement = NULL;
			mpWrite->WriteBlock( (HE_LPVOID)gpStrArrayObjLeft, 1 );
			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					mpWrite->WriteBlock( (HE_LPVOID)gpStrSingleSpace, 1 );
				}
				pElement = pArray->GetElement( i );
				OutPutObject( pElement );
			}
			mpWrite->WriteBlock( (HE_LPVOID)gpStrArrayObjRight, 1 );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pObj;
			CHE_PDF_Object * pElement = NULL;
			mpWrite->WriteBlock( (HE_LPVOID)gpStrDictObjLeft, 2 );

			CHE_ByteString keyStr;
			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
			{
				if ( pDict->GetKeyByIndex( i, keyStr ) == TRUE )
				{
					HE_LPVOID pData = (HE_LPVOID)( keyStr.GetData() );
					HE_DWORD length = keyStr.GetLength();
					mpWrite->WriteBlock( (HE_LPVOID)gpStrNameObjPre, 1 );
					mpWrite->WriteBlock( pData, length );
					pElement = pDict->GetElementByIndex( i );
					if ( pElement->GetType() == OBJ_TYPE_NULL || pElement->GetType() == OBJ_TYPE_NUMBER || pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_BOOLEAN )
					{
						mpWrite->WriteBlock( (HE_LPVOID)gpStrSingleSpace, 1 );
					}
					if ( pElement )
					{
						OutPutObject( pElement );
					}
				}
			}
			mpWrite->WriteBlock( (HE_LPVOID)gpStrDictObjRight, 2 );
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			CHE_PDF_Stream * pStm = (CHE_PDF_Stream*)pObj;
			if( pStm->GetDict() != NULL )
			{
				OutPutObject( pStm->GetDict() );
				mpWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, 1 );
			}
			mpWrite->WriteBlock( (HE_LPVOID)gpStrStreamObjBegin, glStrStreamObjBegin );
			HE_LPBYTE pByte = GetAllocator()->NewArray<HE_BYTE>( pStm->GetRawSize() + 16 );
			pStm->GetRawData( 0, pByte, pStm->GetRawSize() );
			HE_DWORD length = 0;
			if ( mpEncrypt )
			{
				mpEncrypt->Encrypt( pByte, pStm->GetRawSize(), pStm->GetObjNum(), pStm->GetGenNum() );
			}
			mpWrite->WriteBlock( (HE_LPVOID)( pByte ), pStm->GetRawSize() );
			GetAllocator()->DeleteArray<HE_BYTE>( pByte );
			mpWrite->WriteBlock( (HE_LPVOID)gpStrStreamObjEnd, glStrStreamObjEnd );
			break;
		}
	default:
		break;
	}
}

HE_BOOL CHE_PDF_Creator::SetEncryption(	const CHE_ByteString id, const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,
										HE_BYTE algorithm, HE_BYTE keyLength, HE_BYTE revision,  HE_BOOL bMetaData, HE_DWORD pValue )
{
	if ( mpEncrypt )
	{
		GetAllocator()->Delete( mpEncrypt );
	}
	mpEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id, algorithm, keyLength, revision, bMetaData, pValue );
	mpEncrypt->Init( userPassword, ownerPassword );
	return mpEncrypt->Authenticate( userPassword );
}