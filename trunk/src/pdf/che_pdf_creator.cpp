#include "../../include/pdf/che_pdf_creator.h"
#include "../../include/pdf/che_pdf_xref.h"

#include <cstdio>

CHE_PDF_Creator::CHE_PDF_Creator( CHE_Allocator * pAllocator )
	:CHE_Object( pAllocator ), m_collector( pAllocator )
{
	m_bNewDocument = FALSE;

	m_pCatalogDict = NULL;
	m_pPagesDict = NULL;

	m_dwObjNumIndex = 0;
}
	
CHE_PDF_Creator::~CHE_PDF_Creator()
{
	//m_collector按照默认的析构即可清除对象
}

HE_VOID	CHE_PDF_Creator::NewDocument()
{
	if ( m_bNewDocument == TRUE )
	{
		m_bNewDocument = FALSE;
		m_collector.ReleaseObj();
	}

	m_bNewDocument = TRUE;
	m_dwObjNumIndex = 1;
	m_pPagesDict = CHE_PDF_Dictionary::Create( m_dwObjNumIndex, 0, NULL, GetAllocator() );
	m_pPagesDict->SetAtName( "Type", "Pages" );
	CHE_PDF_Array * pTmpArray = CHE_PDF_Array::Create( m_dwObjNumIndex, 0, NULL, GetAllocator() );
	m_pPagesDict->SetAtArray( "Kids", pTmpArray );
	m_pPagesDict->SetAtInteger( "Count", 0 );

	CHE_PDF_IndirectObject * pTmpInObj = CHE_PDF_IndirectObject::Create( m_dwObjNumIndex, 0, m_pPagesDict, NULL, GetAllocator() );
	m_collector.Add( pTmpInObj );

	m_dwObjNumIndex++;
	m_pCatalogDict = CHE_PDF_Dictionary::Create( m_dwObjNumIndex, 0, NULL, GetAllocator() );
	m_pCatalogDict->SetAtName( "Type", "Catalog" );
	m_pCatalogDict->SetAtReference( "Pages", m_pPagesDict->GetObjNum() );
	pTmpInObj = CHE_PDF_IndirectObject::Create( m_dwObjNumIndex, 0, m_pCatalogDict, NULL, GetAllocator() );
	m_collector.Add( pTmpInObj );
}

HE_VOID CHE_PDF_Creator::NewPage()
{
	if ( m_bNewDocument == FALSE || m_pPagesDict == NULL )
	{
		return;
	}

	m_dwObjNumIndex++;
	CHE_PDF_Dictionary * pTmpPageDict = CHE_PDF_Dictionary::Create( m_dwObjNumIndex, 0, NULL, GetAllocator() );
	pTmpPageDict->SetAtName( "Type", "Page" );
	pTmpPageDict->SetAtReference( "Parent", m_pPagesDict->GetObjNum() );

	CHE_PDF_Array * pMediaBoxArray = CHE_PDF_Array::Create( m_dwObjNumIndex, 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum1 = CHE_PDF_Number::Create( 0, m_dwObjNumIndex, 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum2 = CHE_PDF_Number::Create( 0, m_dwObjNumIndex, 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum3 = CHE_PDF_Number::Create( 612, m_dwObjNumIndex, 0, NULL, GetAllocator() );
	CHE_PDF_Number * pNum4 = CHE_PDF_Number::Create( 792, m_dwObjNumIndex, 0, NULL, GetAllocator() );
	pMediaBoxArray->Append( pNum1 );
	pMediaBoxArray->Append( pNum2 );
	pMediaBoxArray->Append( pNum3 );
	pMediaBoxArray->Append( pNum4 );
	pTmpPageDict->SetAtArray( "MediaBox", pMediaBoxArray );
	CHE_PDF_IndirectObject * pTmpInObj = CHE_PDF_IndirectObject::Create( m_dwObjNumIndex, 0, pTmpPageDict, NULL, GetAllocator() );
	m_collector.Add( pTmpInObj );

	CHE_PDF_Reference * pPageRef = CHE_PDF_Reference::Create( pTmpPageDict->GetObjNum(),  m_pPagesDict->GetObjNum(), 0, NULL, GetAllocator() );
	CHE_PDF_Array * pPageArray = (CHE_PDF_Array*)( m_pPagesDict->GetElement( "Kids", PDFOBJ_ARRAY ) );
	if( pPageArray != NULL )
	{
		pPageArray->Append(  pPageRef );
	}
	CHE_PDF_Number * pPageArrayCount = (CHE_PDF_Number*)( m_pPagesDict->GetElement( "Count", PDFOBJ_NUMBER ) );
	if ( pPageArrayCount != NULL )
	{
		pPageArrayCount->SetValue( pPageArrayCount->GetInteger()+1 );
	}
}

HE_VOID	CHE_PDF_Creator::Save( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return;
	}
	CHE_PDF_XREF_Table xrefTable;
	xrefTable.NewSection( 1 );

	HE_DWORD dwFileWriteOffset = 0;

	HE_CHAR * pPDFVersion = "%PDF-1.7\n";
	HE_CHAR	* pNewLine = "\n";
	HE_CHAR * pEndObj = "endobj\n";
	HE_CHAR * pXref = "xref\n";
	HE_CHAR * pFirstXrefEntry = "0000000000 65535 f \n";
	HE_CHAR * pStartXref = "startxref\n";
	HE_CHAR * pEndFile = "%%EOF";
	HE_CHAR * pNullObj = "null";
	HE_CHAR * pNamePre = "/";
	HE_CHAR * pTrailer = "trailer\n";

	pWrite->WriteBlock( (HE_LPDWORD)pPDFVersion, 9 );

	HE_CHAR tempStr[1024];

	CHE_PDF_IndirectObject * pInObj = NULL;
	CHE_PDF_Object * pObj = NULL;
	for ( HE_DWORD i = 0; i < m_collector.GetCount(); i++ )
	{
		pInObj = m_collector.GetObjByIndex( i );
		if ( pInObj == NULL )
		{
			continue;
		}
		CHE_PDF_XREF_Entry tmpEntry( 2, pWrite->GetCurOffset(), 0, pInObj->GetObjNum() );
		xrefTable.NewNode( tmpEntry );

		sprintf( tempStr, "%d %d obj\n", pInObj->GetObjNum(), 0 );
		pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );

		pObj = pInObj->GetObject();
		OutPutObject( pObj, pWrite );
		
		pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );
		pWrite->WriteBlock( (HE_LPVOID)pEndObj, 7 );
	}

	pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );
	pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );

	HE_DWORD xrefOffset = pWrite->GetCurOffset();

	pWrite->WriteBlock( (HE_LPVOID)pXref, 5 );

	sprintf( tempStr, "%d %d\n", 0, xrefTable.GetCount() + 1 );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen( tempStr ) );
	pWrite->WriteBlock(  (HE_LPVOID)pFirstXrefEntry, 20 );

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

	//写尾字典
	pWrite->WriteBlock( (HE_LPVOID)pTrailer, 8 );
	OutPutObject( pTrailerDict, pWrite );
	pTrailerDict->Release();
	pTrailerDict = NULL;

	pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );
	pWrite->WriteBlock( (HE_LPVOID)pStartXref, 10 );
	sprintf( tempStr, "%d\n", xrefOffset );
	pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
	pWrite->WriteBlock( (HE_LPVOID)pEndFile, 5 );

	pWrite->Flush();

}

HE_VOID CHE_PDF_Creator::OutPutObject( CHE_PDF_Object * pObj, IHE_Write * pWrite )
{
	if ( pObj == NULL || pWrite == NULL )
	{
		return;
	}

	HE_CHAR	* pNewLine = "\n";
	HE_CHAR * pEndObj = "endobj\n";
	HE_CHAR * pXref = "xref\n";
	HE_CHAR * pFirstXrefEntry = "0000000000 65535 f \n";
	HE_CHAR * pStartXref = "startxref\n";
	HE_CHAR * pEndFile = "%%EOF";
	HE_CHAR * pNullObj = "null";
	HE_CHAR * pNamePre = "/";
	HE_CHAR * pLeftB = "[";
	HE_CHAR * pRightB = "]";
	HE_CHAR * pSpace = " ";
	HE_CHAR * pLeftDict = "<<";
	HE_CHAR * pRightDict = ">>";
	HE_CHAR * pStream = "stream\n";
	HE_CHAR * pEndStream = "endstream\n";
	HE_CHAR tempStr[1024];

	switch( pObj->GetType() )
	{
	case PDFOBJ_NULL:
		{
			pWrite->WriteBlock( (HE_LPVOID)pNullObj, 4 );
			pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );
			break;
		}
	case PDFOBJ_STRING:
		{
			HE_LPVOID pData = (HE_LPVOID)( ((CHE_PDF_String*)pObj)->GetString().GetData() );
			HE_DWORD length = ((CHE_PDF_String*)pObj)->GetString().GetLength();
			pWrite->WriteBlock( pData, length );
			break;
		}
	case PDFOBJ_NAME:
		{
			HE_LPVOID pData = (HE_LPVOID)( ((CHE_PDF_Name*)pObj)->GetString().GetData() );
			HE_DWORD length = ((CHE_PDF_String*)pObj)->GetString().GetLength();
			pWrite->WriteBlock( (HE_LPVOID)pNamePre, 1 );
			pWrite->WriteBlock( pData, length );
			break;
		}
	case PDFOBJ_NUMBER:
		{
			if ( ((CHE_PDF_Number*)pObj)->IsInteger() == TRUE )
			{
				sprintf( tempStr, "%d", ((CHE_PDF_Number*)pObj)->GetInteger() );
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}else{
				sprintf( tempStr, "%f", ((CHE_PDF_Number*)pObj)->GetFloatNumber() );
				pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			}
			break;
		}
	case PDFOBJ_REFERENCE:
		{
			sprintf( tempStr, "%d 0 R", ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
			pWrite->WriteBlock( (HE_LPVOID)tempStr, strlen(tempStr) );
			break;
		}
	case PDFOBJ_ARRAY:
		{
			CHE_PDF_Array * pArray = (CHE_PDF_Array*)pObj;
			CHE_PDF_Object * pElement = NULL;
			pWrite->WriteBlock( (HE_LPVOID)pLeftB, 1 );
			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
			{
				if ( i != 0 )
				{
					pWrite->WriteBlock( (HE_LPVOID)pSpace, 1 );
				}
				pElement = pArray->GetElement( i );
				OutPutObject( pElement, pWrite );
			}
			pWrite->WriteBlock( (HE_LPVOID)pRightB, 1 );
			break;
		}
	case PDFOBJ_DICTIONARY:
		{
			CHE_PDF_Dictionary * pDict = (CHE_PDF_Dictionary*)pObj;
			CHE_PDF_Object * pElement = NULL;
			pWrite->WriteBlock( (HE_LPVOID)pLeftDict, 2 );

			CHE_ByteString keyStr;
			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
			{
				if ( pDict->GetKeyByIndex( i, keyStr ) == TRUE )
				{
					HE_LPVOID pData = (HE_LPVOID)( keyStr.GetData() );
					HE_DWORD length = keyStr.GetLength();
					pWrite->WriteBlock( (HE_LPVOID)pNamePre, 1 );
					pWrite->WriteBlock( pData, length );
					pWrite->WriteBlock( (HE_LPVOID)pSpace, 1 );
					pElement = pDict->GetElementByIndex( i );
					if ( pElement )
					{
						OutPutObject( pElement, pWrite );
					}
				}
			}
			pWrite->WriteBlock( (HE_LPVOID)pRightDict, 2 );
			break;
		}
	case PDFOBJ_STREAM:
		{
			CHE_PDF_Stream * pStm = (CHE_PDF_Stream*)pObj;
			if( pStm->GetDict() != NULL )
			{
				OutPutObject( pStm->GetDict(), pWrite );
			}
			pWrite->WriteBlock( (HE_LPVOID)pNewLine, 1 );
			pWrite->WriteBlock( (HE_LPVOID)pStream, 7 );
			CHE_PDF_StreamAcc stmAcc;
			if ( stmAcc.Attach( pStm ) == TRUE )
			{
				pWrite->WriteBlock( (HE_LPVOID)( stmAcc.GetData() ), stmAcc.GetSize() );
			}
			pWrite->WriteBlock( (HE_LPVOID)pEndStream, 10 );
			break;
		}
	default:
		break;
	}
}