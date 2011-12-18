#include "../../include/pdf/che_pdf_creator.h"
#include "../../include/pdf/che_pdf_xref.h"

#include "../../extlib/freetype/ft2build.h"
#include "../../extlib/freetype/freetype/freetype.h"

#include <cstdio>

//string resouces for creator
//file structure relative
HE_CHAR * gpStrPDFVersion17 = "%PDF-1.7\n";		HE_DWORD glStrPDFVersion = 9;
HE_CHAR * gpStrPDFVersion16 = "%PDF-1.6\n";
HE_CHAR * gpStrPDFVersion15 = "%PDF-1.5\n";
HE_CHAR * gpStrPDFVersion14 = "%PDF-1.4\n";
HE_CHAR * gpStrPDFVersion13 = "%PDF-1.3\n";
HE_CHAR * gpStrPDFVersion12 = "%PDF-1.2\n";
HE_CHAR * gpStrPDFVersion11 = "%PDF-1.1\n";
HE_CHAR * gpStrPDFVersion10 = "%PDF-1.0\n";
HE_CHAR	* gpStrNewLine = "\n";					HE_DWORD glStrNewLine = 1;
HE_CHAR * gpStrObjBegin = "obj\n";				HE_DWORD glStrObjBegin = 4;
HE_CHAR * gpStrObjEnd = "\nendobj\n";			HE_DWORD glStrObjEnd = 8;
HE_CHAR * gpStrXrefMark = "xref\n";				HE_DWORD glstrXrefMark = 5;
HE_CHAR * gpStrXrefFirstFreeEntry = "0000000000 65535 f \n";	HE_DWORD glStrXrefEntry = 20;
HE_CHAR * gpStrTrailerMark = "trailer\n";		HE_DWORD glStrTrailerMark = 8;
HE_CHAR * gpStrXrefStartMark = "startxref\n";	HE_DWORD glStrXrefStartMark = 10;
HE_CHAR * gpStrFileEnd = "%%EOF";				HE_DWORD glStrFileEnd = 5;
HE_CHAR * gpStrSingleSpace = " ";				HE_DWORD glStrSingleSpace = 1;
//obj relative
HE_CHAR * gpStrNullObj = "null";				HE_DWORD glStrNullObj = 5;
HE_CHAR * gpStrBoolObjFalse = "false";			HE_DWORD glStrBoolObjFalse = 5;
HE_CHAR * gpStrBoolObjTrue = "true";			HE_DWORD glStrBoolObjTrue = 4;
HE_CHAR * gpStrNameObjPre = "/";				HE_DWORD glStrNameObjPre = 1;
HE_CHAR * gpStrStrObjLeft = "(";				HE_DWORD glStrStrObj = 1;
HE_CHAR * gpStrStrObjRight = ")";
HE_CHAR * gpStrArrayObjLeft = "[";				HE_DWORD glStrArrayObj = 1;
HE_CHAR * gpStrArrayObjRight = "]";
HE_CHAR * gpStrDictObjLeft = "<<";				HE_DWORD glStrDictObj = 2;
HE_CHAR * gpStrDictObjRight = ">>";
HE_CHAR * gpStrStreamObjBegin = "stream\n";		HE_DWORD glStrStreamObjBegin = 7;
HE_CHAR * gpStrStreamObjEnd = "\nendstream";	HE_DWORD glStrStreamObjEnd = 10;

CHE_PDF_Creator::CHE_PDF_Creator( CHE_Allocator * pAllocator )
	:CHE_Object( pAllocator ), m_collector( pAllocator ), m_arrPageDict( pAllocator )
{
	m_bNewDocument = FALSE;

	m_pCatalogDict = NULL;
	m_pPagesDict = NULL;
	m_pEncryptDict = NULL;
	m_pInfoDict = NULL;

	m_pEncrypt = NULL;

	m_dwObjNumIndex = 0;
}
	
CHE_PDF_Creator::~CHE_PDF_Creator()
{
	//m_collector按照默认的析构即可清除对象
}

HE_BOOL	CHE_PDF_Creator::NewDocument()
{
	if ( m_bNewDocument == TRUE )
	{
		ResetCreator();
	}
	ResetObjIndex();

	m_pPagesDict = AppendIndirectObj_Dict();
	if ( m_pPagesDict == NULL )
	{
		return FALSE;
	}
	m_pPagesDict->SetAtName( "Type", "Pages" );
	CHE_PDF_Array * pTmpArray = CHE_PDF_Array::Create( GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pTmpArray == NULL )
	{
		ResetObjIndex();
		m_collector.ReleaseObj();
		return FALSE;
	}
	m_pPagesDict->SetAtArray( "Kids", pTmpArray );
	m_pPagesDict->SetAtInteger( "Count", 0 );

	m_pCatalogDict = AppendIndirectObj_Dict();
	if ( m_pCatalogDict == NULL )
	{
		ResetObjIndex();
		m_collector.ReleaseObj();
		return FALSE;
	}
	m_pCatalogDict->SetAtName( "Type", "Catalog" );
	m_pCatalogDict->SetAtReference( "Pages", m_pPagesDict->GetObjNum() );

	m_bNewDocument = TRUE;
	return TRUE;
}

HE_BOOL CHE_PDF_Creator::SetVersion( PDF_VERSION version )
{
	m_Version = version;
	return TRUE;
}


HE_BOOL CHE_PDF_Creator::SetDocumentInfo( PDF_DOCUMENT_INFO infoType, CHE_ByteString & str )
{
	if ( m_bNewDocument == FALSE || str.GetLength() == 0 )
	{
		return FALSE;
	}
	if ( infoType > 8 || infoType < 0 )
	{
		return FALSE;
	}
	if ( m_pInfoDict == NULL )
	{
		m_pInfoDict = AppendIndirectObj_Dict();
	}
	if ( m_pInfoDict == NULL )
	{
		return FALSE;
	}
	switch( infoType )
	{
	case DOCUMENT_INFO_TITLE:
		m_pInfoDict->SetAtString( "Title", str );
		break;
	case DOCUMENT_INFO_AUTHOR:
		m_pInfoDict->SetAtString( "Author", str );
		break;
	case DOCUMENT_INFO_SUBJECT:
		m_pInfoDict->SetAtString( "Subject", str );
		break;
	case DOCUMENT_INFO_KEYWORDS:
		m_pInfoDict->SetAtString( "Keywords", str );
		break;
	case DOCUMENT_INFO_CREATOR:
		m_pInfoDict->SetAtString( "Creator", str );
		break;
	case DOCUMENT_INFO_PRODUCER:
		m_pInfoDict->SetAtString( "Producer", str );
		break;
	case DOCUMENT_INFO_CREATIONDATE:
		m_pInfoDict->SetAtString( "CreationDate", str );
		break;
	case DOCUMENT_INFO_MODDATE:
		m_pInfoDict->SetAtString( "ModDate", str );
		break;
	case DOCUMENT_INFO_TRAPPED:
		m_pInfoDict->SetAtString( "Trapped", str );
		break;
	default:
		break;
	}
	return TRUE;
}

CHE_PDF_Dictionary * CHE_PDF_Creator::NewPage( HE_DWORD width, HE_DWORD height )
{
	if ( m_bNewDocument == FALSE || m_pPagesDict == NULL )
	{
		return NULL;
	}

	CHE_PDF_Dictionary * pTmpPageDict = AppendIndirectObj_Dict();
	if ( pTmpPageDict == NULL )
	{
		return NULL;
	}
	pTmpPageDict->SetAtName( "Type", "Page" );
	pTmpPageDict->SetAtReference( "Parent", m_pPagesDict->GetObjNum() );
	CHE_PDF_Array * pMediaBoxArray = CHE_PDF_Array::Create( GetObjIndex(), 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum1 = CHE_PDF_Number::Create( 0, GetObjIndex(), 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum2 = CHE_PDF_Number::Create( 0, GetObjIndex(), 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum3 = CHE_PDF_Number::Create( (HE_INT32)width, GetObjIndex(), 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum4 = CHE_PDF_Number::Create( (HE_INT32)height, GetObjIndex(), 0, NULL, GetAllocator() );
	pMediaBoxArray->Append( pNum1 );
	pMediaBoxArray->Append( pNum2 );
	pMediaBoxArray->Append( pNum3 );
	pMediaBoxArray->Append( pNum4 );
	pTmpPageDict->SetAtArray( "MediaBox", pMediaBoxArray );

	CHE_PDF_Reference * pPageRef = CHE_PDF_Reference::Create( pTmpPageDict->GetObjNum(),  m_pPagesDict->GetObjNum(), 0, NULL, GetAllocator() );
	CHE_PDF_Array * pPageArray = (CHE_PDF_Array*)( m_pPagesDict->GetElement( "Kids", OBJ_TYPE_ARRAY ) );
	if( pPageArray != NULL )
	{
		pPageArray->Append(  pPageRef );
	}
	CHE_PDF_Number * pPageArrayCount = (CHE_PDF_Number*)( m_pPagesDict->GetElement( "Count", OBJ_TYPE_NUMBER ) );
	if ( pPageArrayCount != NULL )
	{
		pPageArrayCount->SetValue( pPageArrayCount->GetInteger()+1 );
	}
	m_arrPageDict.Append( (HE_LPVOID)pTmpPageDict );	//加入到指针数组中便于快速访问
	return pTmpPageDict;
}

HE_BOOL	CHE_PDF_Creator::SetAsPageResource( HE_DWORD pageIndex, const CHE_PDF_Dictionary * pDict )
{
	CHE_PDF_Dictionary * pPageDict = (CHE_PDF_Dictionary*)( m_arrPageDict.GetItem( pageIndex ) );
	if ( pPageDict == NULL )
	{ 
		return FALSE;
	}
	if ( pDict == NULL )
	{
		return FALSE;
	}
	CHE_ByteString str;
	CHE_PDF_Object * pObj = pDict->GetElement( "Type" );
	if ( pObj == NULL )
	{
		return FALSE;
	}
	if ( pObj->GetType() == OBJ_TYPE_NAME )
	{
		str = ((CHE_PDF_Name*)pObj)->GetString();
	}else{
		return FALSE;
	}
	if ( str == "Font" )
	{
		CHE_PDF_Dictionary * pPageResourcesDict = NULL;
		pPageResourcesDict = (CHE_PDF_Dictionary*)( pPageDict->GetElement( "Resources" ) );
		if ( pPageResourcesDict == NULL )
		{
			pPageResourcesDict = CHE_PDF_Dictionary::Create( pPageDict->GetObjNum(), 0, NULL, GetAllocator() );
			if ( pPageResourcesDict == NULL )
			{
				return FALSE;
			}
			pPageDict->SetAtDictionary( "Resources", pPageResourcesDict );
		}
		CHE_PDF_Dictionary * pPageResoucesFontDict = (CHE_PDF_Dictionary *)( pPageResourcesDict->GetElement( "Font" ) );
		if ( pPageResoucesFontDict == NULL )
		{
			pPageResoucesFontDict = CHE_PDF_Dictionary::Create( pPageDict->GetObjNum(), 0, NULL, GetAllocator() );
			if ( pPageResoucesFontDict == NULL )
			{
				return FALSE;
			}
			pPageResourcesDict->SetAtDictionary( "Font", pPageResoucesFontDict );
		}
		HE_DWORD fontIndex = pPageResoucesFontDict->GetCount();
		HE_CHAR tmpStr[16];
		sprintf( tmpStr, "F%d", fontIndex );
		pPageResoucesFontDict->SetAtReference( tmpStr, pDict->GetObjNum() );
	}else{
	}
	return TRUE;
}

CHE_PDF_Stream * CHE_PDF_Creator::AddStreamAsPageContents( HE_DWORD pageIndex )
{
	CHE_PDF_Dictionary * pPageDict = GetPageDict( pageIndex );
	if ( pPageDict == NULL )
	{
		return NULL;
	}
	CHE_PDF_Stream * pStm = AppendIndirectObj_Stream();
	if ( pStm == NULL )
	{
		return NULL;
	}
	pPageDict->SetAtReference( "Contents", pStm->GetObjNum() );
	return pStm;
}

HE_BOOL	CHE_PDF_Creator::Save( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return FALSE;
	}

	CHE_PDF_Dictionary* pEncryptDict = NULL;
	if ( m_pEncrypt )
	{
		pEncryptDict = AppendIndirectObj_Dict();
		pEncryptDict->SetAtName( "Filter", "Standard" );
		pEncryptDict->SetAtInteger( "V", 2/*m_pEncrypt->m_algorithm*/ );
		pEncryptDict->SetAtInteger( "R", m_pEncrypt->m_revision );
		pEncryptDict->SetAtInteger( "P", m_pEncrypt->m_PValue );
		pEncryptDict->SetAtInteger( "Length", m_pEncrypt->m_keyLength );
		//pEncryptDict->SetAtBoolean( "EncryptMetadata", m_pEncrypt->m_bMetaData );
		CHE_ByteString str;
		str.SetData( m_pEncrypt->m_OValue, 32 );
		pEncryptDict->SetAtString( "O", str );
		str.SetData( m_pEncrypt->m_UValue, 32 );
		pEncryptDict->SetAtString( "U", str );
// 		CHE_PDF_Dictionary * pCFDict = CHE_PDF_Dictionary::Create( pEncryptDict->GetObjNum(), pEncryptDict->GetGenNum(), NULL, GetAllocator() );
// 		CHE_PDF_Dictionary * pStdCFDict = CHE_PDF_Dictionary::Create( pEncryptDict->GetObjNum(), pEncryptDict->GetGenNum(), NULL, GetAllocator() );
// 		pStdCFDict->SetAtInteger( "Length", 16 );
// 		pStdCFDict->SetAtName( "CFM", "V2" );
// 		pStdCFDict->SetAtName( "AuthEvent", "DocOpen" );
// 		pCFDict->SetAtDictionary( "CF", pStdCFDict );
// 		pEncryptDict->SetAtName( "StmF", "StdCF" );
	}

	CHE_PDF_XREF_Table xrefTable;
	xrefTable.NewSection( 1 );

	switch( m_Version )
	{
	case PDF_VERSION_1_0:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion10, glStrPDFVersion );
		break;
	case PDF_VERSION_1_1:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion11, glStrPDFVersion );
		break;
	case PDF_VERSION_1_2:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion12, glStrPDFVersion );
		break;
	case PDF_VERSION_1_3:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion13, glStrPDFVersion );
		break;
	case PDF_VERSION_1_4:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion14, glStrPDFVersion );
		break;
	case PDF_VERSION_1_5:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion15, glStrPDFVersion );
		break;
	case PDF_VERSION_1_6:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion16, glStrPDFVersion );
		break;
	case PDF_VERSION_1_7:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion17, glStrPDFVersion );
		break;
	default:
		pWrite->WriteBlock( (HE_LPVOID)gpStrPDFVersion17, glStrPDFVersion );
		break;
	}
	
	pWrite->WriteBlock( (HE_LPVOID)"%\255\255\255\255\n", 6 );

	HE_CHAR tempStr[1024];

	CHE_PDF_Object * pObj = NULL;
	for ( HE_DWORD i = 0; i < m_collector.GetCount(); i++ )
	{
		pObj = m_collector.GetObjByIndex( i );
		if ( pObj == NULL )
		{
			continue;
		}
		CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, pWrite->GetCurOffset(), 0 );
		xrefTable.NewNode( tmpEntry );

		sprintf( tempStr, "%d %d obj\n", pObj->GetObjNum(), 0 );
		pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
		OutPutObject( pObj, pWrite );

		pWrite->WriteBlock( (HE_LPVOID)gpStrObjEnd, glStrObjEnd );
		pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );
	}

	pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );

	HE_DWORD xrefOffset = pWrite->GetCurOffset();

	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefMark, glstrXrefMark );

	sprintf( tempStr, "%d %d\n", 0, xrefTable.GetCount() + 1 );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
	pWrite->WriteBlock(  (HE_LPVOID)gpStrXrefFirstFreeEntry, glStrXrefEntry );

	CHE_PDF_XREF_Entry tmpEntry;
	for ( HE_DWORD i = 1; i <= xrefTable.GetCount(); i++ )
	{
		if ( xrefTable.GetEntry( i, tmpEntry ) == TRUE )
		{
			sprintf( tempStr, "%010d %05d n \n", tmpEntry.GetOffset(), 0 );
			pWrite->WriteBlock( (HE_LPVOID)tempStr, 20 );
		}
	}

	CHE_PDF_Dictionary * pTrailerDict = CHE_PDF_Dictionary::Create( 0, 0, NULL, GetAllocator() );
	pTrailerDict->SetAtInteger( "Size", xrefTable.GetCount() );
	pTrailerDict->SetAtReference( "Root", m_pCatalogDict->GetObjNum() );
	if ( pEncryptDict )
	{
		pTrailerDict->SetAtReference( "Encrypt", pEncryptDict->GetObjNum() );
	}
	if ( m_pInfoDict != NULL )
	{
		pTrailerDict->SetAtReference( "Info", m_pInfoDict->GetObjNum() );
	}

	//写尾字典
	pWrite->WriteBlock( (HE_LPVOID)gpStrTrailerMark, glStrTrailerMark );
	OutPutObject( pTrailerDict, pWrite );
	pTrailerDict->Release();
	pTrailerDict = NULL;

	pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );
	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefStartMark, glStrXrefStartMark );
	sprintf( tempStr, "%d\n", xrefOffset );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
	pWrite->WriteBlock( (HE_LPVOID)gpStrFileEnd, glStrFileEnd );

	pWrite->Flush();

	return TRUE;
}

HE_BOOL CHE_PDF_Creator::SaveUpdate( IHE_Write * pWrite, CHE_PDF_Parser * pParser )
{
	if ( pWrite == NULL || pParser == NULL )
	{
		return FALSE;
	}
	HE_CHAR tempStr[1024];
	CHE_PDF_XREF_Table xrefTable;
	CHE_PDF_Object * pObj = NULL;
	HE_DWORD lObjCount = pParser->GetModifiedObjectCount();
	if ( lObjCount > 0 )
	{
		for ( HE_DWORD i = 0; i < lObjCount; i++ )
		{
			pObj = pParser->GetModifiedObject( i );
			if ( pObj != NULL )
			{
				xrefTable.NewSection( pObj->GetObjNum() );
				CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, pWrite->GetCurOffset(), 0 );
				xrefTable.NewNode( tmpEntry );
				sprintf( tempStr, "%d %d obj\n", pObj->GetObjNum(), 0 );
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
				OutPutObject( pObj, pWrite );
				pWrite->WriteBlock( (HE_LPVOID)gpStrObjEnd, glStrObjEnd );
			}
		}
	}
	lObjCount = pParser->GetNewObjectCount();
	if ( lObjCount > 0 )
	{
		for ( HE_DWORD i = 0; i < lObjCount; i++ )
		{
			pObj = pParser->GetNewObject( i );
			if ( pObj != NULL )
			{
				xrefTable.NewSection( pObj->GetObjNum() );
				CHE_PDF_XREF_Entry tmpEntry( XREF_ENTRY_TYPE_COMMON, pWrite->GetCurOffset(), 0 );
				xrefTable.NewNode( tmpEntry );
				sprintf( tempStr, "%d %d obj\n", pObj->GetObjNum(), 0 );
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
				OutPutObject( pObj, pWrite );
				pWrite->WriteBlock( (HE_LPVOID)gpStrObjEnd, glStrObjEnd );
			}
		}
	}

	HE_DWORD xrefOffset = pWrite->GetCurOffset();

	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefMark, glstrXrefMark );

	sprintf( tempStr, "%d %d\n", 0, 1 );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
	pWrite->WriteBlock(  (HE_LPVOID)gpStrXrefFirstFreeEntry, glStrXrefEntry );

	CHE_PDF_XREF_Entry tmpEntry;
	for ( HE_DWORD i = 1; i <= xrefTable.GetMaxObjNum(); i++ )
	{
		if ( xrefTable.GetEntry( i, tmpEntry ) == TRUE )
		{
			sprintf( tempStr, "%d %d\n", i, 1 );
			pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
			sprintf( tempStr, "%010d %05d n \n", tmpEntry.GetOffset(), 0 );
			pWrite->WriteBlock( (HE_LPVOID)tempStr, 20 );
		}
	}

	CHE_PDF_Dictionary * pTrailerDict = pParser->GetTrailerDict();
	if ( pTrailerDict )
	{
		pTrailerDict->SetAtInteger( "Prev", pParser->m_lStartxref );

		//写尾字典
		pWrite->WriteBlock( (HE_LPVOID)gpStrTrailerMark, glStrTrailerMark );
		OutPutObject( pTrailerDict, pWrite );
	}

	pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, glStrNewLine );
	pWrite->WriteBlock( (HE_LPVOID)gpStrXrefStartMark, glStrXrefStartMark );
	sprintf( tempStr, "%d\n", xrefOffset );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
	pWrite->WriteBlock( (HE_LPVOID)gpStrFileEnd, glStrFileEnd );

	pWrite->Flush();

	return TRUE;
}

CHE_PDF_Dictionary * CHE_PDF_Creator::AddType1Font_Standard14( HE_BYTE fontType, PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/ )
{
	if ( m_bNewDocument != TRUE )
	{
		return NULL;
	}
	if ( fontType > 13 || fontType < 0 )
	{
		return NULL;
	}
	CHE_PDF_Dictionary * pDict = AppendIndirectObj_Dict();
	if ( pDict == NULL )
	{
		return NULL;
	}
// 	CHE_PDF_Array * pWidthsArr = AppendIndirectObj_Array();
// 	if ( pWidthsArr == NULL )
// 	{
// 		return FALSE;	//如果这里失败了，会在文档中留下一个无用的字典间接对象
// 	}
// 	for ( HE_DWORD i = 0; i < 256; i++ )
// 	{
// 		CHE_PDF_Number * pNumber = CHE_PDF_Number::Create( (HE_INT32)1000, pWidthsArr->GetObjNum(), 0, NULL, GetAllocator() );
// 		pWidthsArr->Append( pNumber );
// 	}

	pDict->SetAtName( "Type", "Font" );
	pDict->SetAtName( "Subtype", "Type1" );
	switch( fontType )
	{
	case FONT_TYPE1_STANDARD14_TIMES_ROMAN:
		pDict->SetAtName( "BaseFont", "Times-Roman" );
		break;
	case FONT_TYPE1_STANDARD14_TIMES_BOLD:
		pDict->SetAtName( "BaseFont", "Times-Bold" );
		break;
	case FONT_TYPE1_STANDARD14_TIMES_ITALIC:
		pDict->SetAtName( "BaseFont", "Times-Italic" );
		break;
	case FONT_TYPE1_STANDARD14_TIMES_BOLDITALIC:
		pDict->SetAtName( "BaseFont", "Times-BoldItalic" );
		break;
	case FONT_TYPE1_STANDARD14_HELVETICA:
		pDict->SetAtName( "BaseFont", "Helvetica" );
		break;
	case FONT_TYPE1_STANDARD14_HELVETICA_BOLD:
		pDict->SetAtName( "BaseFont", "Helvetica-Bold" );
		break;
	case FONT_TYPE1_STANDARD14_HELVETICA_OBILQUE:
		pDict->SetAtName( "BaseFont", "Helvetica-Obilque" );
		break;
	case FONT_TYPE1_STANDARD14_HELVETICA_BOLDOBILQUE:
		pDict->SetAtName( "BaseFont", "Helvetica-BoldObilque" );
		break;
	case FONT_TYPE1_STANDARD14_COURIER:
		pDict->SetAtName( "BaseFont", "Courier" );
		break;
	case FONT_TYPE1_STANDARD14_COURIER_BOLD:
		pDict->SetAtName( "BaseFont", "Courier-Bold" );
		break;
	case FONT_TYPE1_STANDARD14_COURIER_OBILQUE:
		pDict->SetAtName( "BaseFont", "Courier-Obilque" );
		break;
	case FONT_TYPE1_STANDARD14_COURIER_BOLDOBILQUE:
		pDict->SetAtName( "BaseFont", "Courier-BoldObilque" );
		break;
	case FONT_TYPE1_STANDARD14_SYMBOL:
		pDict->SetAtName( "BaseFont", "Symbol" );
		break;
	case FONT_TYPE1_STANDARD14_ZAPFDINGBATS:
		pDict->SetAtName( "BaseFont", "ZapfDingbats" );
		break;
	default:
		break;
	}
	//pDict->SetAtInteger( "FirstChar", 0 );
	//pDict->SetAtInteger( "LastChar", 255 );
	//pDict->SetAtReference( "Widths", pWidthsArr->GetObjNum() );
	switch ( encoding )
	{
	case FONT_ENCODING_WINANSI:
		pDict->SetAtName( "Encoding", "WinAnsiEncoding" );
		break;
	case FONT_ENCODING_MACROMAN:
		pDict->SetAtName( "Encoding", "MacRomanEncoding" );
		break;
	case FONT_ENCODING_MACEXPERT:
		pDict->SetAtName( "Encoding", "MacExpertEncoding" );
		break;
	default:
		break;
	}
	return pDict;
}

CHE_PDF_Dictionary * CHE_PDF_Creator::AddType1Font(	const CHE_ByteString & baseFont,
												   HE_DWORD firstChar/* = 0*/, HE_DWORD lastChar/* = 255*/,
												   PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/, 
												   const CHE_PDF_Array * pWidths/* = NULL*/, 
												   const CHE_PDF_Dictionary * pFontDescriptor/* = NULL*/,
												   const CHE_PDF_Stream * pToUnicode/* = NULL*/ )
{
	if ( m_bNewDocument != TRUE || baseFont.GetLength() == 0 )
	{
		return NULL;
	}
	CHE_PDF_Dictionary * pDict = AppendIndirectObj_Dict();
	if ( pDict == NULL )
	{
		return NULL;
	}
	pDict->SetAtName( "Type", "Font" );
	pDict->SetAtName( "Subtype", "Type1" );
	pDict->SetAtName( "BaseFont", baseFont );
	pDict->SetAtInteger( "FirstChar", 0 );
	pDict->SetAtInteger( "LastChar", 255 );
	switch( encoding )
	{
	case FONT_ENCODING_WINANSI:
		pDict->SetAtName( "Encoding", "WinAnsiEncoding" );
		break;
	case FONT_ENCODING_MACROMAN:
		pDict->SetAtName( "Encoding", "MacRomanEncoding" );
		break;
	case FONT_ENCODING_MACEXPERT:
		pDict->SetAtName( "Encoding", "MacExpertEncoding" );
		break;
	default:
		break;
	}
	if ( pWidths )
	{
		pDict->SetAtReference( "Widths", pWidths->GetObjNum() );
	}
	if ( pFontDescriptor )
	{
		pDict->SetAtReference( "FontDescriptor", pWidths->GetObjNum() );
	}
	if ( pToUnicode )
	{
		pDict->SetAtReference( "FontDescriptor", pToUnicode->GetObjNum() );
	}
	return pDict;
}

CHE_PDF_Dictionary* CHE_PDF_Creator::AddTrueTypeFont(	const CHE_ByteString & baseFont, 
														HE_DWORD firstChar/* = 0*/, HE_DWORD lastChar/* = 255*/,
														PDF_FONT_ENCODING encoding /*= FONT_ENCODING_WINANSIENCODING*/,
														const CHE_PDF_Array * pWidths/* = NULL*/, 
														const CHE_PDF_Dictionary * pFontDescriptor/* = NULL*/,
														const CHE_PDF_Stream * pToUnicode/* = NULL*/ )
{
	if ( m_bNewDocument != TRUE || baseFont.GetLength() == 0 )
	{
		return NULL;
	}
	CHE_PDF_Dictionary * pDict = AppendIndirectObj_Dict();
	if ( pDict == NULL )
	{
		return NULL;
	}
	pDict->SetAtName( "Type", "Font" );
	pDict->SetAtName( "Subtype", "TrueType" );
	pDict->SetAtName( "BaseFont", baseFont );
	pDict->SetAtInteger( "FirstChar", firstChar );
	pDict->SetAtInteger( "LastChar", lastChar );
	switch( encoding )
	{
	case FONT_ENCODING_WINANSI:
		pDict->SetAtName( "Encoding", "WinAnsiEncoding" );
		break;
	case FONT_ENCODING_MACROMAN:
		pDict->SetAtName( "Encoding", "MacRomanEncoding" );
		break;
	case FONT_ENCODING_MACEXPERT:
		pDict->SetAtName( "Encoding", "MacExpertEncoding" );
		break;
	default:
		break;
	}
	if ( pWidths )
	{
		pDict->SetAtReference( "Widths", pWidths->GetObjNum() );
	}
	if ( pFontDescriptor )
	{
		pDict->SetAtReference( "FontDescriptor", pWidths->GetObjNum() );
	}
	if ( pToUnicode )
	{
		pDict->SetAtReference( "FontDescriptor", pToUnicode->GetObjNum() );
	}
	return pDict;
}

CHE_PDF_Dictionary* CHE_PDF_Creator::AddTrueTypeFont( const char * pFontFile, HE_BOOL bEmbed/* = FALSE*/ )
{
	if( pFontFile == NULL )
		return NULL;

	FT_Error eRet = 0;
	FT_Library ftLib = NULL;
	FT_Face ftFace = NULL;
	eRet = FT_Init_FreeType( &ftLib );
	if ( eRet != 0 )
	{
		return NULL;
	}
	eRet = FT_New_Face( ftLib, pFontFile, 0, &ftFace );
	if ( eRet != 0 )
	{
		FT_Done_FreeType( ftLib );
		return NULL;
	}
	const char * pFontName = FT_Get_Postscript_Name( ftFace );
	if ( pFontName == NULL )
	{
		FT_Done_Face( ftFace );
		FT_Done_FreeType( ftLib );
		return NULL;
	}
	CHE_PDF_Dictionary * pFontDict = AddTrueTypeFont( pFontName, 0, 0 );
	if ( pFontDict == NULL )
	{
		FT_Done_Face( ftFace );
		FT_Done_FreeType( ftLib );
		return NULL;
	}
	HE_FLOAT tmpF = ftFace->units_per_EM * 1.0 / 1000;
	CHE_PDF_Array * pArr = CHE_PDF_Array::Create( pFontDict->GetObjNum(), 0, NULL, pFontDict->GetAllocator() );
	CHE_PDF_Number * pWidth = CHE_PDF_Number::Create( (HE_INT32)( ftFace->max_advance_width/tmpF + 0.5 ), pFontDict->GetObjNum(), 0, NULL, pFontDict->GetAllocator() );
	pArr->Append( pWidth );
	pFontDict->SetAtArray( "Widths", pArr );
	CHE_PDF_Dictionary * pFontDescriptor = AddDictionary();
	if ( pFontDescriptor )
	{
		pFontDict->SetAtReference( "FontDescriptor", pFontDescriptor->GetObjNum() );
		pFontDescriptor->SetAtName( "Type", "FontDescriptor" );
		pFontDescriptor->SetAtName( "FontName", pFontName );
		
		CHE_PDF_Array * pArr = CHE_PDF_Array::Create( pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
		CHE_PDF_Number* f1 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.xMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
		CHE_PDF_Number* f2 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.yMin/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
		CHE_PDF_Number* f3 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.xMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
		CHE_PDF_Number* f4 = CHE_PDF_Number::Create( (HE_INT32)( ftFace->bbox.yMax/tmpF + 0.5 ), pFontDescriptor->GetObjNum(), 0, NULL, pFontDescriptor->GetAllocator() );
		pArr->Append( f1 );
		pArr->Append( f2 );
		pArr->Append( f3 );
		pArr->Append( f4 );
		pFontDescriptor->SetAtArray( "FontBBox", pArr );
		pFontDescriptor->SetAtInteger( "Ascent", ftFace->ascender/tmpF + 0.5 );
		pFontDescriptor->SetAtInteger( "Descent", ftFace->descender/tmpF + 0.5 );
		pFontDescriptor->SetAtInteger( "CapHeight", 500 );
		pFontDescriptor->SetAtInteger( "MissingWidth", ftFace->max_advance_width/tmpF + 0.5 );
		pFontDescriptor->SetAtInteger( "Flags", 33 );
		pFontDescriptor->SetAtInteger( "ItalicAngle", 0 );
		pFontDescriptor->SetAtInteger( "StemV", 80 );

		if ( bEmbed == TRUE )
		{
			IHE_Read * pRead = HE_CreateFileRead( pFontFile );
			if ( pRead != NULL )
			{
				HE_LPBYTE pByte = GetAllocator()->NewArray<HE_BYTE>( pRead->GetSize() );
				pRead->ReadBlock( pByte, 0, pRead->GetSize() );
				CHE_PDF_Stream * pStm = AddStream();
				pFontDescriptor->SetAtReference( "FontFile2", pStm->GetObjNum() );
				pStm->SetRawData( pByte, pRead->GetSize(), STREAM_FILTER_FLATE );
				pStm->GetDict()->SetAtInteger( "Length1", pRead->GetSize() );
				HE_DestoryIHERead( pRead );
				GetAllocator()->DeleteArray<HE_BYTE>( pByte );
			}
		}
	}
	FT_Done_Face( ftFace );
	FT_Done_FreeType( ftLib );
	return pFontDict;
}

HE_VOID CHE_PDF_Creator::OutPutObject( CHE_PDF_Object * pObj, IHE_Write * pWrite )
{
	if ( pObj == NULL || pWrite == NULL )
	{
		return;
	}
	static HE_CHAR tempStr[1024];
	switch( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			pWrite->WriteBlock( (HE_LPVOID)gpStrNullObj, glStrNullObj );
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			if( ((CHE_PDF_Boolean*)pObj)->GetValue() != FALSE )
			{
				pWrite->WriteBlock( (HE_LPVOID)gpStrBoolObjTrue, glStrBoolObjTrue );
			}else{
				pWrite->WriteBlock( (HE_LPVOID)gpStrBoolObjFalse, glStrBoolObjFalse );
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
				pWrite->WriteBlock( (HE_LPVOID)gpStrStrObjLeft, glStrStrObj );
				pWrite->WriteBlock( pData, length );
				pWrite->WriteBlock( (HE_LPVOID)gpStrStrObjRight, glStrStrObj );
			}else{
				pWrite->WriteBlock( (HE_LPVOID)"<", 1 );
				HE_CHAR tmpByte[32];
				HE_DWORD tmpVal = 0;
				for ( HE_DWORD i = 0; i < length; i++ )
				{
					tmpVal = (pObj->ToString()->GetString())[i];
					sprintf( tmpByte, "%08X", tmpVal );
					pWrite->WriteBlock( (HE_LPVOID)(tmpByte+6), 2 );
				}
				pWrite->WriteBlock( (HE_LPVOID)">", 1 );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			HE_BYTE * pData = (HE_BYTE*)( ((CHE_PDF_Name*)pObj)->GetString().GetData() );
			HE_DWORD length = ((CHE_PDF_String*)pObj)->GetString().GetLength();
			pWrite->WriteBlock( (HE_LPVOID)gpStrNameObjPre, 1 );
			char tmpStr[16];
			for ( HE_DWORD i = 0; i < length; ++i )
			{
				if ( 32 < pData[i] && pData[i] < 127 )
				{
					pWrite->WriteBlock( (HE_LPVOID*)(pData+i), 1 );
				}else{
					sprintf( tmpStr, "#%02X", pData[i] ); 
					pWrite->WriteBlock( (HE_LPVOID*)tmpStr, 3 );
				}
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			if ( ((CHE_PDF_Number*)pObj)->IsInteger() == TRUE )
			{
				sprintf( tempStr, "%d", ((CHE_PDF_Number*)pObj)->GetInteger() );
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}else{
				float value = ((CHE_PDF_Number*)pObj)->GetFloat();
				sprintf( tempStr, "%g", value );
				if ( strchr( tempStr, 'e' ) )
				{
					sprintf( tempStr, (fabsf(value))>1? "%1.1f":"%1.8f", value );
				}
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			sprintf( tempStr, "%d 0 R", ((CHE_PDF_Reference*)pObj)->GetRefNum() );
			pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_Array * pArray = (CHE_PDF_Array*)pObj;
			CHE_PDF_Object * pElement = NULL;
			pWrite->WriteBlock( (HE_LPVOID)gpStrArrayObjLeft, 1 );
			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					pWrite->WriteBlock( (HE_LPVOID)gpStrSingleSpace, 1 );
				}
				pElement = pArray->GetElement( i );
				OutPutObject( pElement, pWrite );
			}
			pWrite->WriteBlock( (HE_LPVOID)gpStrArrayObjRight, 1 );
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pObj;
			CHE_PDF_Object * pElement = NULL;
			pWrite->WriteBlock( (HE_LPVOID)gpStrDictObjLeft, 2 );

			CHE_ByteString keyStr;
			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
			{
				if ( pDict->GetKeyByIndex( i, keyStr ) == TRUE )
				{
					HE_LPVOID pData = (HE_LPVOID)( keyStr.GetData() );
					HE_DWORD length = keyStr.GetLength();
					pWrite->WriteBlock( (HE_LPVOID)gpStrNameObjPre, 1 );
					pWrite->WriteBlock( pData, length );
					pElement = pDict->GetElementByIndex( i );
					if ( pElement->GetType() == OBJ_TYPE_NULL || pElement->GetType() == OBJ_TYPE_NUMBER || pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_BOOLEAN )
					{
						pWrite->WriteBlock( (HE_LPVOID)gpStrSingleSpace, 1 );
					}
					if ( pElement )
					{
						OutPutObject( pElement, pWrite );
					}
				}
			}
			pWrite->WriteBlock( (HE_LPVOID)gpStrDictObjRight, 2 );
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			CHE_PDF_Stream * pStm = (CHE_PDF_Stream*)pObj;
			if( pStm->GetDict() != NULL )
			{
				OutPutObject( pStm->GetDict(), pWrite );
				pWrite->WriteBlock( (HE_LPVOID)gpStrNewLine, 1 );
			}
			pWrite->WriteBlock( (HE_LPVOID)gpStrStreamObjBegin, glStrStreamObjBegin );
			HE_LPBYTE pByte = GetAllocator()->NewArray<HE_BYTE>( pStm->GetRawSize() + 16 );
			pStm->GetRawData( 0, pByte, pStm->GetRawSize() );
			HE_DWORD length = 0;
			if ( m_pEncrypt )
			{
				m_pEncrypt->Encrypt( pByte, pStm->GetRawSize(), pStm->GetObjNum(), pStm->GetGenNum() );
			}
			pWrite->WriteBlock( (HE_LPVOID)( pByte ), pStm->GetRawSize() );
			GetAllocator()->DeleteArray<HE_BYTE>( pByte );
			pWrite->WriteBlock( (HE_LPVOID)gpStrStreamObjEnd, glStrStreamObjEnd );
			break;
		}
	default:
		break;
	}
}

HE_VOID CHE_PDF_Creator::ResetCreator()
{
	m_dwObjNumIndex = 0;
	m_bNewDocument = FALSE;

	m_pCatalogDict = NULL;
	m_pPagesDict = NULL;
	m_pEncryptDict = NULL;
	m_pInfoDict = NULL;

	m_arrPageDict.Clear();
	m_collector.ReleaseObj();
}

CHE_PDF_Null* CHE_PDF_Creator::AppendIndirectObj_Null()
{
	IncreaseObjIndex();
	CHE_PDF_Null * pObj = CHE_PDF_Null::Create( GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Boolean* CHE_PDF_Creator::AppendIndirectObj_Boolean( HE_BOOL value )
{
	IncreaseObjIndex();
	CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Number* CHE_PDF_Creator::AppendIndirectObj_Number( HE_INT32 value )
{
	IncreaseObjIndex();
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Number* CHE_PDF_Creator::AppendIndirectObj_Number( HE_FLOAT value )
{
	IncreaseObjIndex();
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_String* CHE_PDF_Creator::AppendIndirectObj_String( CHE_ByteString & str )
{
	IncreaseObjIndex();
	CHE_PDF_String * pObj = CHE_PDF_String::Create( str, GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Name* CHE_PDF_Creator::AppendIndirectObj_Name( CHE_ByteString & str )
{
	IncreaseObjIndex();
	CHE_PDF_Name * pObj = CHE_PDF_Name::Create( str, GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Array* CHE_PDF_Creator::AppendIndirectObj_Array()
{
	IncreaseObjIndex();
	CHE_PDF_Array * pObj = CHE_PDF_Array::Create( GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Dictionary* CHE_PDF_Creator::AppendIndirectObj_Dict()
{
	IncreaseObjIndex();
	CHE_PDF_Dictionary * pObj = CHE_PDF_Dictionary::Create( GetObjIndex(), 0, NULL, GetAllocator() );
	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
	}else{
		DecreaseObjIndex();
	}
	return pObj;
}

CHE_PDF_Stream*	CHE_PDF_Creator::AppendIndirectObj_Stream()
{
	IncreaseObjIndex();
 	CHE_PDF_Stream * pObj = CHE_PDF_Stream::Create( GetObjIndex(), 0, NULL, NULL, GetAllocator() );
 	if ( pObj != NULL )
	{
		m_collector.Add( pObj );
 	}else{
 		DecreaseObjIndex();
 	}
 	return pObj;
}

HE_BOOL CHE_PDF_Creator::SetEncryption(	const CHE_ByteString id, const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,
										HE_BYTE algorithm, HE_BYTE keyLength, HE_BYTE revision,  HE_BOOL bMetaData, HE_DWORD pValue )
{
	if ( m_pEncrypt )
	{
		GetAllocator()->Delete( m_pEncrypt );
	}
	m_pEncrypt = GetAllocator()->New<CHE_PDF_Encrypt>( id, /*userPassword, ownerPassword,*/ algorithm, keyLength, revision, bMetaData, pValue );
	m_pEncrypt->Init( userPassword, ownerPassword );
	return m_pEncrypt->Authenticate( userPassword );
}