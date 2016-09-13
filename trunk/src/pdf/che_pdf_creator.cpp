#include "../../include/pdf/che_pdf_creator.h"
#include "../../include/pdf/che_pdf_xref.h"

#include <cstdio>
#include <math.h>
#include <queue>

namespace chelib {
    
//string resources for creator
//file structure relative
const char * gpStrPDFVersion17			= "%PDF-1.7\n";				const size_t glStrPDFVersion = 9;
const char * gpStrPDFVersion16			= "%PDF-1.6\n";
const char * gpStrPDFVersion15			= "%PDF-1.5\n";
const char * gpStrPDFVersion14			= "%PDF-1.4\n";
const char * gpStrPDFVersion13			= "%PDF-1.3\n";
const char * gpStrPDFVersion12			= "%PDF-1.2\n";
const char * gpStrPDFVersion11			= "%PDF-1.1\n";
const char * gpStrPDFVersion10			= "%PDF-1.0\n";
const char * gpStrNewLine				= "\n";						const size_t glStrNewLine = 1;
const char * gpStrObjBegin				= "obj\n";					//const size_t glStrObjBegin = 4;
const char * gpStrObjEnd					= "\nendobj\n";				const size_t glStrObjEnd = 8;
const char * gpStrXrefMark				= "xref\n";					const size_t glstrXrefMark = 5;
const char * gpStrXrefFirstFreeEntry		= "0000000000 65535 f \n";	const size_t glStrXrefEntry = 20;
const char * gpStrTrailerMark			= "trailer\n";				const size_t glStrTrailerMark = 8;
const char * gpStrXrefStartMark			= "startxref\n";			const size_t glStrXrefStartMark = 10;
const char * gpStrFileEnd				= "%%EOF";					const size_t glStrFileEnd = 5;
const char * gpStrSingleSpace			= " ";						//const size_t glStrSingleSpace = 1;
//obj relative
const char * gpStrNullObj				= "null";					const size_t glStrNullObj = 5;
const char * gpStrBoolObjFalse			= "false";					const size_t glStrBoolObjFalse = 5;
const char * gpStrBoolObjTrue			= "true";					const size_t glStrBoolObjTrue = 4;
const char * gpStrNameObjPre				= "/";						//const size_t glStrNameObjPre = 1;
const char * gpStrStrObjLeft				= "(";						const size_t glStrStrObj = 1;
const char * gpStrStrObjRight			= ")";
const char * gpStrArrayObjLeft			= "[";						//const size_t glStrArrayObj = 1;
const char * gpStrArrayObjRight			= "]";
const char * gpStrDictObjLeft			= "<<";						//const size_t glStrDictObj = 2;
const char * gpStrDictObjRight			= ">>";
const char * gpStrStreamObjBegin			= "stream\n";				const size_t glStrStreamObjBegin = 7;
const char * gpStrStreamObjEnd			= "\nendstream";			const size_t glStrStreamObjEnd = 10;

CPDF_Creator * CPDF_Creator::Create( IWrite * pWrite, CAllocator * pAllocator )
{
	if ( pWrite == nullptr )
	{
		return nullptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	return pAllocator->New<CPDF_Creator>( pWrite, pAllocator );
}

CPDF_Creator::CPDF_Creator( IWrite * pWrite, CAllocator * pAllocator )
	: CObject( pAllocator ), mpWrite( pWrite ), mpEncrypt( nullptr ), mbCompress(TRUE) {}
	
CPDF_Creator::~CPDF_Creator()
{
// 	if ( mpEncrypt )
// 	{
// 		mpEncrypt->GetAllocator()->Delete( mpEncrypt );
// 	}
}

// bool CPDF_Creator::SaveUpdate( IHE_Write * pWrite, CPDF_Parser * pParser )
// {
// 	if ( pWrite == nullptr || pParser == nullptr )
// 	{
// 		return false;
// 	}
// 	char tempStr[1024];
// 	CPDF_XREF_Table xrefTable;
// 	CPDF_Object * pObj = nullptr;
// 	CPDF_IndirectObject * pInObj = nullptr;
// 	size_t lObjCount = pParser->GetUpdatedInObjCount();
// 	if ( lObjCount > 0 )
// 	{
// 		for ( size_t i = 0; i < lObjCount; i++ )
// 		{
// 			pInObj = pParser->GetUpdatedInObj( i );
// 			if ( pInObj != nullptr )
// 			{
// 				pObj = pInObj->GetObj();
// 				xrefTable.NewSection( pInObj->GetObjNum() );
// 				CPDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, pWrite->GetCurOffset(), 0 );
// 				xrefTable.NewNode( tmpEntry );
// 				sprintf( tempStr, "%d %d obj\n", pInObj->GetObjNum(), 0 );
// 				pWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
// 				OutPutObject( pObj, pWrite );
// 				pWrite->WriteBlock( (void *)gpStrObjEnd, glStrObjEnd );
// 			}
// 		}
// 	}
// 
// 	size_t xrefOffset = pWrite->GetCurOffset();
// 
// 	pWrite->WriteBlock( (void *)gpStrXrefMark, glstrXrefMark );
// 
// 	sprintf( tempStr, "%d %d\n", 0, 1 );
// 	pWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
// 	pWrite->WriteBlock(  (void *)gpStrXrefFirstFreeEntry, glStrXrefEntry );
// 
// 	CPDF_XREF_Entry tmpEntry;
// 	for ( size_t i = 1; i <= xrefTable.GetMaxObjNum(); i++ )
// 	{
// 		if ( xrefTable.GetEntry( i, tmpEntry ) == TRUE )
// 		{
// 			sprintf( tempStr, "%d %d\n", i, 1 );
// 			pWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
// 			sprintf( tempStr, "%010d %05d n \n", tmpEntry.GetOffset(), 0 );
// 			pWrite->WriteBlock( (void *)tempStr, 20 );
// 		}
// 	}
// 
// 	CPDF_Dictionary * pTrailerDict = pParser->GetTrailerDict();
// 	if ( pTrailerDict )
// 	{
// 		pTrailerDict->SetAtInteger( "Prev", pParser->m_lStartxref );
// 
// 		//Ð´Î²×Öµä
// 		pWrite->WriteBlock( (void *)gpStrTrailerMark, glStrTrailerMark );
// 		OutPutObject( pTrailerDict, pWrite );
// 	}
// 
// 	pWrite->WriteBlock( (void *)gpStrNewLine, glStrNewLine );
// 	pWrite->WriteBlock( (void *)gpStrXrefStartMark, glStrXrefStartMark );
// 	sprintf( tempStr, "%d\n", xrefOffset );
// 	pWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
// 	pWrite->WriteBlock( (void *)gpStrFileEnd, glStrFileEnd );
// 
// 	pWrite->Flush();
// 
// 	return TRUE;
// }

// CPDF_IndirectObject * CPDF_Creator::AddType1Font_Standard14( BYTE fontType, PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/ )
// {
// 	if ( m_bNewDocument != TRUE )
// 	{
// 		return nullptr;
// 	}
// 	if ( fontType > 13 || fontType < 0 )
// 	{
// 		return nullptr;
// 	}
// 	CPDF_IndirectObject * pInFontDictObj = AppendIndirectObj_Dict();
// 	CPDF_Dictionary * pFontDict = pInFontDictObj->GetObj()->GetDictPtr();
// 	if ( pFontDict == nullptr )
// 	{
// 		return nullptr;
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
// 	CPDF_IndirectObject * pInWidthArrayObj = AppendIndirectObj_Array();
// 	CPDF_Array * pWidthsArr = pInWidthArrayObj->GetObj()->GetArrayPtr();
// 	if ( pWidthsArr )
// 	{
// 		for ( size_t i = 0; i < 256; i++ )
// 		{
// 			CPDF_Number * pNumber = CPDF_Number::Create( (int32)1000, GetAllocator() );
// 			pWidthsArr->Append( pNumber );
// 		}
// 		pFontDict->SetAtInteger( "FirstChar", 0 );
// 		pFontDict->SetAtInteger( "LastChar", 255 );
// 		pFontDict->SetAtReference( "Widths", pInWidthArrayObj->GetObjNum(), pInWidthArrayObj->GetGenNum(), nullptr );
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
// CPDF_IndirectObject * CPDF_Creator::AddType1Font(	const CByteString & baseFont,
// 														size_t firstChar /*= 0*/, size_t lastChar /*= 255*/,
// 														PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSI*/,
// 														CPDF_Array * pWidths /*= nullptr*/,
// 														CPDF_Dictionary * pFontDescriptor /*= nullptr*/,
// 														CPDF_IndirectObject * pToUnicodeStream /*= nullptr*/ )
// {
// 	if ( !m_bNewDocument || baseFont.GetLength() == 0 )
// 	{
// 		return nullptr;
// 	}
// 	CPDF_IndirectObject * pInFontDictObj = AppendIndirectObj_Dict();
// 	CPDF_Dictionary * pFontDict = pInFontDictObj->GetObj()->GetDictPtr();
// 	if ( pFontDict == nullptr )
// 	{
// 		return nullptr;
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
// 		pFontDict->SetAtReference( "ToUnicode", pToUnicodeStream->GetObjNum(), pToUnicodeStream->GetGenNum(), nullptr );
// 	}
// 	return pInFontDictObj;
// }

// CPDF_Dictionary* CPDF_Creator::AddTrueTypeFont(	const CByteString & baseFont, 
// 														size_t firstChar/* = 0*/, size_t lastChar/* = 255*/,
// 														PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/,
// 														const CPDF_Array * pWidths/* = nullptr*/, 
// 														const CPDF_Dictionary * pFontDescriptor/* = nullptr*/,
// 														const CPDF_Stream * pToUnicode/* = nullptr*/ )
// {
// 	if ( m_bNewDocument != TRUE || baseFont.GetLength() == 0 )
// 	{
// 		return nullptr;
// 	}
// 	CPDF_Dictionary * pDict = AppendIndirectObj_Dict();
// 	if ( pDict == nullptr )
// 	{
// 		return nullptr;
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

// CPDF_Dictionary* CPDF_Creator::AddTrueTypeFont( const char * pFontFile, bool bEmbed/* = false*/ )
// {
// 	if( pFontFile == nullptr )
// 		return nullptr;
// 
// 	FT_Error eRet = 0;
// 	FT_Library ftLib = nullptr;
// 	FT_Face ftFace = nullptr;
// 	eRet = FT_Init_FreeType( &ftLib );
// 	if ( eRet != 0 )
// 	{
// 		return nullptr;
// 	}
// 	eRet = FT_New_Face( ftLib, pFontFile, 0, &ftFace );
// 	if ( eRet != 0 )
// 	{
// 		FT_Done_FreeType( ftLib );
// 		return nullptr;
// 	}
// 	const char * pFontName = FT_Get_Postscript_Name( ftFace );
// 	if ( pFontName == nullptr )
// 	{
// 		FT_Done_Face( ftFace );
// 		FT_Done_FreeType( ftLib );
// 		return nullptr;
// 	}
// 	CPDF_Dictionary * pFontDict = AddTrueTypeFont( pFontName, 0, 0 );
// 	if ( pFontDict == nullptr )
// 	{
// 		FT_Done_Face( ftFace );
// 		FT_Done_FreeType( ftLib );
// 		return nullptr;
// 	}
// 	FLOAT tmpF = ftFace->units_per_EM * 1.0 / 1000;
// 	CPDF_Array * pArr = CPDF_Array::Create( pFontDict->GetObjNum(), 0, nullptr, pFontDict->GetAllocator() );
// 	CPDF_Number * pWidth = CPDF_Number::Create( (int32)( ftFace->max_advance_width/tmpF + 0.5 ), pFontDict->GetObjNum(), 0, nullptr, pFontDict->GetAllocator() );
// 	pArr->Append( pWidth );
// 	pFontDict->SetAtArray( "Widths", pArr );
// 	CPDF_Dictionary * pFontDescriptor = AddDictionary();
// 	if ( pFontDescriptor )
// 	{
// 		pFontDict->SetAtReference( "FontDescriptor", pFontDescriptor->GetObjNum() );
// 		pFontDescriptor->SetAtName( "Type", "FontDescriptor" );
// 		pFontDescriptor->SetAtName( "FontName", pFontName );
// 		
// 		CPDF_Array * pArr = CPDF_Array::Create( pFontDescriptor->GetObjNum(), 0, nullptr, pFontDescriptor->GetAllocator() );
// 		CPDF_Number* f1 = CPDF_Number::Create( (int32)( ftFace->bbox.xMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, nullptr, pFontDescriptor->GetAllocator() );
// 		CPDF_Number* f2 = CPDF_Number::Create( (int32)( ftFace->bbox.yMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, nullptr, pFontDescriptor->GetAllocator() );
// 		CPDF_Number* f3 = CPDF_Number::Create( (int32)( ftFace->bbox.xMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, nullptr, pFontDescriptor->GetAllocator() );
// 		CPDF_Number* f4 = CPDF_Number::Create( (int32)( ftFace->bbox.yMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, nullptr, pFontDescriptor->GetAllocator() );
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
// 			if ( pRead != nullptr )
// 			{
// 				PBYTE pByte = GetAllocator()->NewArray<BYTE>( pRead->GetSize() );
// 				pRead->ReadBlock( pByte, 0, pRead->GetSize() );
// 				CPDF_Stream * pStm = AddStream();
// 				pFontDescriptor->SetAtReference( "FontFile2", pStm->GetObjNum() );
// 				pStm->SetRawData( pByte, pRead->GetSize(), STREAM_FILTER_FLATE );
// 				pStm->GetDictPtr()->SetAtInteger( "Length1", pRead->GetSize() );
// 				HE_DestoryIHERead( pRead );
// 				GetAllocator()->DeleteArray<BYTE>( pByte );
// 			}
// 		}
// 	}
// 	FT_Done_Face( ftFace );
// 	FT_Done_FreeType( ftLib );
// 	return pFontDict;
// }

void CPDF_Creator::OutPutFileHead( PDF_VERSION version )
{
	switch( version )
	{
	case PDF_VERSION_1_0:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion10, glStrPDFVersion );
		break;
	case PDF_VERSION_1_1:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion11, glStrPDFVersion );
		break;
	case PDF_VERSION_1_2:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion12, glStrPDFVersion );
		break;
	case PDF_VERSION_1_3:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion13, glStrPDFVersion );
		break;
	case PDF_VERSION_1_4:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion14, glStrPDFVersion );
		break;
	case PDF_VERSION_1_5:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion15, glStrPDFVersion );
		break;
	case PDF_VERSION_1_6:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion16, glStrPDFVersion );
		break;
	case PDF_VERSION_1_7:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion17, glStrPDFVersion );
		break;
	default:
		mpWrite->WriteBlock( (void *)gpStrPDFVersion17, glStrPDFVersion );
		break;
	}
    if (mbCompress)
    {
        mpWrite->WriteBlock( (void *)"%\255\255\255\255\255\255\n", 8 );
    }else{
        mpWrite->WriteByte('\n');
    }
}

size_t CPDF_Creator::OutPutObject( const PDF_RefInfo refInfo, const CPDF_ObjectPtr & pObj )
{
	if ( pObj )
	{
		return 0;
	}
	size_t offset = mpWrite->GetCurOffset();
	OutPutObject( mpWrite, refInfo, pObj, nullptr );
	return offset;
}

size_t CPDF_Creator::OutPutInObject( const PDF_RefInfo & refInfo, const CPDF_ObjectPtr & objPtr, bool bEncrypt )
{
	if ( ! objPtr )
	{
		return 0;
	}
	size_t offset = mpWrite->GetCurOffset();
	char tempStr[128];
	sprintf( tempStr, "%ld %ld obj\n", refInfo.objNum, refInfo.genNum );
	mpWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
	if ( bEncrypt )
	{
		OutPutObject( mpWrite, refInfo, objPtr, mpEncrypt );
	}else{
		OutPutObject( mpWrite, refInfo, objPtr, nullptr );
	}
	
	mpWrite->WriteBlock( (void *)gpStrObjEnd, glStrObjEnd );
	return offset;
}

size_t CPDF_Creator::OutPutXRefTable( CPDF_XRefTable & xref )
{
	char tempStr[128];
	size_t lBeginNum = 0;
	size_t lNextObjNum = 0;
	size_t lCountNum = 0;
	CPDF_XRefEntry entry;
    std::queue<CPDF_XRefEntry> entryList;

	size_t offset = mpWrite->GetCurOffset();
	mpWrite->WriteBlock( (void *)gpStrXrefMark, glstrXrefMark );
	
	xref.MoveFirst();
	if ( ! xref.GetCurNode( entry ) )
	{
		return offset;
	}
	lBeginNum = entry.GetObjNum();
	lNextObjNum = lBeginNum+1;
	lCountNum = 1;
	entryList.push( entry );
	
	xref.MoveNext();

	while( !xref.IsEOF() )
	{
		if ( xref.GetCurNode( entry ) )
		{
			if ( entry.GetObjNum() == lNextObjNum )
			{
				entryList.push( entry );
				++lCountNum;
			}else{
				CPDF_XRefEntry tmpEntry;
				sprintf( tempStr, "%ld %ld\n", lBeginNum, lCountNum );
				mpWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
				while( !entryList.empty() )
				{
                    tmpEntry = entryList.front();
                    entryList.pop();
					if ( entry.GetObjNum() == 0 )
					{
						mpWrite->WriteBlock( (void *)gpStrXrefFirstFreeEntry, glStrXrefEntry );
					}else{
						sprintf( tempStr, "%010ld %05ld n \n", tmpEntry.GetOffset(), tmpEntry.GetGenNum() );
						mpWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
					}
				}
				entryList.push( entry );
				lBeginNum = entry.GetObjNum();
				lCountNum = 1;
				lNextObjNum = lBeginNum;
			}
			++lNextObjNum;
		}else{
			break;
		}
		xref.MoveNext();
	}

	if ( lCountNum > 0 )
	{
		CPDF_XRefEntry tmpEntry;
		sprintf( tempStr, "%ld %ld\n", lBeginNum, lCountNum );
		mpWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
		while( !entryList.empty() )
		{
            tmpEntry = entryList.front();
            entryList.pop();
			if ( tmpEntry.GetObjNum() == 0 )
			{
				mpWrite->WriteBlock( (void *)gpStrXrefFirstFreeEntry, glStrXrefEntry );
			}else{
				sprintf( tempStr, "%010ld %05ld n \n", tmpEntry.GetOffset(), tmpEntry.GetGenNum() );
				mpWrite->WriteBlock( (void *)tempStr, strlen( tempStr ) );
			}
		}
	}

	return offset;
}

void CPDF_Creator::OutPutFileTailer( size_t startxref )
{
	char tempStr[128];
	mpWrite->WriteBlock( (void *)gpStrXrefStartMark, glStrXrefStartMark );
	sprintf( tempStr, "%ld\n", startxref );
	mpWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
	mpWrite->WriteBlock( (void *)gpStrFileEnd, glStrFileEnd );
}

void CPDF_Creator::OutPutTailerDict( const CPDF_DictionaryPtr & pDcit )
{
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;
	mpWrite->WriteBlock( (void *)gpStrTrailerMark, glStrTrailerMark );
	OutPutObject( mpWrite, refInfo, pDcit, nullptr );
	mpWrite->WriteBlock( (void *)gpStrNewLine, glStrNewLine );
}

void CPDF_Creator::OutPutObject( IWrite * pWrite, const PDF_RefInfo refInfo,
								 const CPDF_ObjectPtr & pObj, CPDF_Encrypt * pEncrypt )
{
	if ( pWrite == nullptr || ! pObj )
	{
		return;
	}
	static char tempStr[32768];
	switch( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			pWrite->WriteBlock( (void *)gpStrNullObj, glStrNullObj );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			CPDF_BooleanPtr ptr = pObj->GetBooleanPtr();
			if( ptr->GetValue() != false )
			{
				pWrite->WriteBlock( (void *)gpStrBoolObjTrue, glStrBoolObjTrue );
			}else{
				pWrite->WriteBlock( (void *)gpStrBoolObjFalse, glStrBoolObjFalse );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			CPDF_StringPtr ptr = pObj->GetStringPtr();
			CByteString str = ptr->GetString();
			PBYTE pData = (PBYTE)( str.GetData() );
			size_t length = str.GetLength();

			if ( pEncrypt )
			{
				PBYTE pBytes = GetDefaultAllocator()->NewArray<BYTE>( length /*+ 16*/ );
				memcpy( pBytes, pData, length );
				pEncrypt->Encrypt( pBytes, length, refInfo.objNum, refInfo.genNum );
				pData = pBytes;
				//length += 16;
			}
			bool bHex = false;
			for ( size_t i = 0; i < length; i++ )
			{
				if ( pData[i] > 127 || pData[i] < 32 || pData[i] == '(' || pData[i] == ')' ) //´ýÍêÉÆ
				{
					bHex  = TRUE;
					break;
				}
			}
			if ( bHex == false )
			{
				pWrite->WriteBlock( (void *)gpStrStrObjLeft, glStrStrObj );
				pWrite->WriteBlock( pData, length );
				pWrite->WriteBlock( (void *)gpStrStrObjRight, glStrStrObj );
			}else{
				pWrite->WriteBlock( (void *)"<", 1 );
				char tmpByte[32];
				size_t tmpVal = 0;
				for ( size_t i = 0; i < length; i++ )
				{
					tmpVal = pData[i];
					sprintf( tmpByte, "%08lX", tmpVal );
					pWrite->WriteBlock( (void *)(tmpByte+6), 2 );
				}
				pWrite->WriteBlock( (void *)">", 1 );
			}
			if ( pEncrypt )
			{
				GetDefaultAllocator()->DeleteArray<BYTE>( pData );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			CPDF_NamePtr ptr = pObj->GetNamePtr();
			PBYTE pData = (PBYTE)( ptr->GetString().GetData() );
			size_t length = ptr->GetString().GetLength();
			pWrite->WriteBlock( (void *)gpStrNameObjPre, 1 );
			char tmpStr[16];
			for ( size_t i = 0; i < length; ++i )
			{
				if ( 32 < pData[i] && pData[i] < 127 )
				{
					pWrite->WriteBlock( (void **)(pData+i), 1 );
				}else{
					sprintf( tmpStr, "#%02X", pData[i] ); 
					pWrite->WriteBlock( (void **)tmpStr, 3 );
				}
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			CPDF_NumberPtr ptr = pObj->GetNumberPtr();
			if ( ptr->IsInteger() == TRUE )
			{
				sprintf( tempStr, "%d", ptr->GetInteger() );
				pWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
			}else{
				float value = ptr->GetFloat();
				sprintf( tempStr, "%g", value );
				if ( strchr( tempStr, 'e' ) )
				{
					sprintf( tempStr, (fabsf(value))>1? "%1.1f":"%1.8f", value );
				}
				pWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
			}
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			CPDF_ReferencePtr ptr = pObj->GetRefPtr();
			sprintf( tempStr, "%ld 0 R", ptr->GetRefNum() );
			pWrite->WriteBlock( (void *)tempStr, strlen(tempStr) );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CPDF_ArrayPtr ptr = pObj->GetArrayPtr();
			CPDF_ObjectPtr pElement;
			pWrite->WriteBlock( (void *)gpStrArrayObjLeft, 1 );
			for ( size_t i = 0; i < ptr->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					pWrite->WriteBlock( (void *)gpStrSingleSpace, 1 );
				}
				pElement = ptr->GetElement( i );
				OutPutObject( pWrite, refInfo, pElement, pEncrypt );
			}
			pWrite->WriteBlock( (void *)gpStrArrayObjRight, 1 );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CPDF_DictionaryPtr ptr = pObj->GetDictPtr();
			CPDF_ObjectPtr pElement;
			pWrite->WriteBlock( (void *)gpStrDictObjLeft, 2 );

			CByteString keyStr;
            ptr->MoveToFirst();
			while ( ptr->GetKeyAndElement( keyStr, pElement ) )
			{
                void * pData = (void *)( keyStr.GetData() );
                size_t length = keyStr.GetLength();
                pWrite->WriteBlock( (void *)gpStrNameObjPre, 1 );
                pWrite->WriteBlock( pData, length );
                if ( pElement->GetType() == OBJ_TYPE_NULL || pElement->GetType() == OBJ_TYPE_NUMBER || pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() ==OBJ_TYPE_BOOLEAN )
                {
                    pWrite->WriteBlock( (void *)gpStrSingleSpace, 1 );
                }
                if ( pElement )
                {
                    OutPutObject( pWrite, refInfo, pElement, pEncrypt );
                }
			}
			pWrite->WriteBlock( (void *)gpStrDictObjRight, 2 );
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			CPDF_StreamPtr stmPtr = pObj->GetStreamPtr();
			CPDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
			if( ! dictPtr )
			{
				break;
			}
            
            CPDF_ObjectPtr tmpPtr = dictPtr->GetElement( "Filter" );
            if (!tmpPtr && mbCompress)
            {
                CPDF_StreamAcc stmAcc;
                CDynBuffer buf;
                stmAcc.Attach( stmPtr );
                buf.Write( stmAcc.GetData(), stmAcc.GetSize() );
                stmAcc.Detach();
                stmPtr->SetRawData( buf.GetData(), buf.GetSize(), STREAM_FILTER_FLATE );
            }else if (tmpPtr && !mbCompress)
            {
                CPDF_StreamAcc stmAcc;
                CDynBuffer buf;
                stmAcc.Attach( stmPtr );
                buf.Write( stmAcc.GetData(), stmAcc.GetSize() );
                stmAcc.Detach();
                stmPtr->SetRawData( buf.GetData(), buf.GetSize(), STREAM_FILTER_NULL );
            }

			OutPutObject( pWrite, refInfo, stmPtr->GetDictPtr(), pEncrypt );
			pWrite->WriteBlock( (void *)gpStrNewLine, 1 );

			pWrite->WriteBlock( (void *)gpStrStreamObjBegin, glStrStreamObjBegin );
			PBYTE pBytes = GetDefaultAllocator()->NewArray<BYTE>( stmPtr->GetRawSize() );
			stmPtr->GetRawData( 0, pBytes, stmPtr->GetRawSize() );
			if ( pEncrypt )
			{
				pEncrypt->Encrypt( pBytes, stmPtr->GetRawSize(), stmPtr->GetObjNum(), stmPtr->GetGenNum() );
			}
			pWrite->WriteBlock( (void *)( pBytes ), stmPtr->GetRawSize() );
			GetDefaultAllocator()->DeleteArray( pBytes );
			pWrite->WriteBlock( (void *)gpStrStreamObjEnd, glStrStreamObjEnd );
			break;
		}
	default:
		break;
	}
}

bool CPDF_Creator::SetEncryption(	const CByteString id, const CByteString userPassword, const CByteString ownerPassword,
										BYTE algorithm, BYTE keyLength, BYTE revision,  bool bMetaData, size_t pValue )
{
	if ( mpEncrypt )
	{
		GetAllocator()->Delete( mpEncrypt );
	}
	mpEncrypt = GetAllocator()->New<CPDF_Encrypt>( id, algorithm, revision, revision, keyLength, bMetaData, pValue ); //ÐèÒªÍêÉÆ
	mpEncrypt->Init( userPassword, ownerPassword );
	return mpEncrypt->Authenticate( userPassword );
}

}//namespace