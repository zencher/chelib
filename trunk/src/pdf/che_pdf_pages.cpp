#include "../../include/pdf/che_pdf_pages.h"
#include <memory.h>

CHE_PDF_Document::CHE_PDF_Document() : m_ID1(), m_ID2()
{
	m_pInfoDict = NULL;
	m_pRootDict = NULL;
	m_pParser = NULL;
	m_pPageObjNumList = NULL;
}

CHE_PDF_Document::CHE_PDF_Document( IHE_Read * pFileRead )
{
	if ( pFileRead == NULL )
	{
		m_ID1 = "";
		m_ID2 = "";
		m_pInfoDict = NULL;
		m_pRootDict = NULL;
		m_pParser = NULL;
		m_pPageObjNumList = NULL;
	}else{
		m_pParser = new CHE_PDF_Parser();
		m_pParser->StartParse( pFileRead );
		m_pParser->GetStartxrefOffset( 1024 );
		m_pParser->ParseXRef();
		//m_pParser->VerifyXRef();
		m_pRootDict = m_pParser->GetRootDict();
		m_pInfoDict = m_pParser->GetInfoDict();
		HE_DWORD pageCount = m_pParser->GetPageCount();
		m_pPageObjNumList = new HE_DWORD[pageCount];
		memset( m_pPageObjNumList, 0, pageCount * sizeof(HE_DWORD) );
		m_pParser->GetPageObjList( m_pPageObjNumList );
		CHE_PDF_Object * pIDArray = m_pParser->GetIDArray();
		if ( pIDArray && pIDArray->GetType() != PDFOBJ_ARRAY )
		{
			CHE_PDF_String * pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 0 );
			m_ID1 = pStrObj->GetString();
			pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 1 );
			m_ID2 = pStrObj->GetString();
		}
	}
}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( m_pPageObjNumList )
	{
		delete [] m_pPageObjNumList;
	}
	if ( m_pParser )
	{
		m_pParser->CloseParser();
		delete m_pParser;
	}
}

HE_BOOL CHE_PDF_Document::Load( IHE_Read * pFileRead )
{
	if ( pFileRead == NULL )
	{
		return FALSE;
	}

	m_pParser = new CHE_PDF_Parser();
	m_pParser->StartParse( pFileRead );
	m_pParser->GetStartxrefOffset( 1024 );
	m_pParser->ParseXRef();
	//m_pParser->VerifyXRef();
	m_pRootDict = m_pParser->GetRootDict();
	m_pInfoDict = m_pParser->GetInfoDict();
	HE_DWORD pageCount = m_pParser->GetPageCount();
	m_pPageObjNumList = new HE_DWORD[pageCount];
	memset( m_pPageObjNumList, 0, pageCount * sizeof(HE_DWORD) );
	m_pParser->GetPageObjList( m_pPageObjNumList );
	CHE_PDF_Object * pIDArray = m_pParser->GetIDArray();
	if ( pIDArray && pIDArray->GetType() == PDFOBJ_ARRAY )
	{
		CHE_PDF_String * pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 0 );
		m_ID1 = pStrObj->GetString();
		pStrObj = (CHE_PDF_String*)((CHE_PDF_Array*)pIDArray)->GetElement( 1 );
		m_ID2 = pStrObj->GetString();
	}
	return TRUE;
}

//HE_BOOL CHE_PDF_Document::Save( IHE_Write * pFileRead );

HE_DWORD CHE_PDF_Document::GetPageCount() const
{
	if ( m_pParser )
	{
		return m_pParser->GetPageCount();
	}
	return 0;
}
 
CHE_PDF_Page* CHE_PDF_Document::GetPage( HE_DWORD iPageIndex )
{
	if ( m_pParser == NULL )
	{
		return NULL;
	}
	if ( iPageIndex >= GetPageCount() )
	{
		return NULL;
	}
	HE_DWORD objNum = m_pPageObjNumList[iPageIndex];
	CHE_PDF_Object * pPageObj =  m_pParser->GetIndirectObject( objNum );
	if ( pPageObj == NULL )
	{
		return NULL;
	}
	pPageObj = ((CHE_PDF_IndirectObject*)pPageObj)->GetObject();
	CHE_PDF_Page * pPage = new CHE_PDF_Page( (CHE_PDF_Dictionary*)pPageObj, this );
	return pPage;
}

HE_BOOL CHE_PDF_Document::IsEncrypted()
{
	if ( m_pParser == NULL )
	{
		return FALSE;
	}
	if ( m_pParser->GetTrailerDict() == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Object * pObj = m_pParser->GetTrailerDict()->GetElement( CHE_ByteString("Encrypt") );
	if ( pObj == NULL )
	{
		return FALSE;
	}else{
		return TRUE;
	}
}
 
// HE_DWORD CHE_PDF_Document::GetPageIndex( HE_DWORD objnum );

CHE_PDF_Page::CHE_PDF_Page( CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc )
{
	m_pPageObj = NULL;
	m_fPageHeight = 0.0;
	m_fPageWidth = 0.0;
	m_pDoc = pDoc;
	if ( pDict )
	{
		m_pPageObj = pDict;
		CHE_PDF_Object * pObj = m_pPageObj->GetElement( CHE_ByteString("MediaBox") );
		if ( pObj )
		{
			if ( pObj->GetType() == PDFOBJ_ARRAY )
			{
				CHE_PDF_Number * pNum1 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 0 );
				CHE_PDF_Number * pNum2 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 1 );
				CHE_PDF_Number * pNum3 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 2 );
				CHE_PDF_Number * pNum4 = (CHE_PDF_Number*)((CHE_PDF_Array*)pObj)->GetElement( 3 );
				m_fPageWidth = pNum3->GetFloatNumber() - pNum1->GetFloatNumber();
				m_fPageHeight = pNum4->GetFloatNumber() - pNum2->GetFloatNumber();
			}
		}
	}
}

CHE_PDF_Page::~CHE_PDF_Page()
{

}

HE_BOOL CHE_PDF_Page::GetPageContent( CHE_DynBuffer & buffer )
{
	CHE_PDF_Dictionary * pPageDict = m_pPageObj;
	if ( pPageDict == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Object * pPageContent = pPageDict->GetElement( CHE_ByteString("Contents") );
	if ( pPageContent == NULL )
	{
		return FALSE;
	}
	if ( m_pDoc == NULL )
	{
		return FALSE;
	}
	if (m_pDoc->GetParser() == NULL )
	{
		return FALSE;
	}
	if ( pPageContent->GetType() == PDFOBJ_REFERENCE )
	{
		HE_DWORD objNum = ((CHE_PDF_Reference*)pPageContent)->GetRefNuml();
		CHE_PDF_IndirectObject * pInObj = m_pDoc->GetParser()->GetIndirectObject( objNum );
		if ( pInObj == NULL )
		{
			return FALSE;
		}
		CHE_PDF_Object *  pContentObj = pInObj->GetObject();
		if ( pContentObj->GetType() == PDFOBJ_STREAM )
		{
			CHE_PDF_StreamAcc stmAcc;
			stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
			buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
			stmAcc.Detach();
		}else if ( pContentObj->GetType() == PDFOBJ_ARRAY )
		{
			CHE_PDF_Object * pTmpObj = NULL;
			for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pContentObj)->GetCount(); i++ )
			{
				pTmpObj = ((CHE_PDF_Array*)pContentObj)->GetElement( i );
				if ( pTmpObj->GetType() == PDFOBJ_STREAM )
				{
					CHE_PDF_StreamAcc stmAcc;
					stmAcc.Attach( (CHE_PDF_Stream*)pTmpObj );
					buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
					stmAcc.Detach();
					
				}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
				{
					HE_DWORD objNum = ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml();
					CHE_PDF_IndirectObject * pInObj = m_pDoc->GetParser()->GetIndirectObject( objNum );
					if ( pInObj == NULL )
					{
						continue;
						//return FALSE;
					}
					CHE_PDF_Object *  pContentObj = pInObj->GetObject();
					if ( pContentObj->GetType() == PDFOBJ_STREAM )
					{
						CHE_PDF_StreamAcc stmAcc;
						stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
						buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
						stmAcc.Detach();
					}
				}
			}
		}
		return TRUE;
	}else if ( pPageContent->GetType() == PDFOBJ_ARRAY )
	{
		HE_DWORD objCount = ((CHE_PDF_Array*)pPageContent)->GetCount();
		for ( HE_DWORD i = 0; i < objCount; i++ )
		{
			CHE_PDF_Object * pObj = ((CHE_PDF_Array*)pPageContent)->GetElement( i );
			if ( pObj == NULL )
			{
				continue;
			}
			if ( pObj->GetType() != PDFOBJ_REFERENCE )
			{
				continue;
			}
			HE_DWORD objNum = ((CHE_PDF_Reference*)pObj)->GetRefNuml();
			CHE_PDF_IndirectObject * pInObj = m_pDoc->GetParser()->GetIndirectObject( objNum );
			if ( pInObj == NULL )
			{
				continue;
			}
			CHE_PDF_Object *  pContentObj = pInObj->GetObject();

			if ( pContentObj->GetType() == PDFOBJ_STREAM )
			{
				CHE_PDF_StreamAcc stmAcc;
				stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
				buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
				stmAcc.Detach();
			}else if ( pContentObj->GetType() == PDFOBJ_ARRAY )
			{
				CHE_PDF_Object * pTmpObj = NULL;
				for ( HE_DWORD i = 0; i < ((CHE_PDF_Array*)pContentObj)->GetCount(); i++ )
				{
					pTmpObj = ((CHE_PDF_Array*)pContentObj)->GetElement( i );
					if ( pTmpObj->GetType() == PDFOBJ_STREAM )
					{
						CHE_PDF_StreamAcc stmAcc;
						stmAcc.Attach( (CHE_PDF_Stream*)pTmpObj );
						buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
						stmAcc.Detach();
						
					}else if ( pTmpObj->GetType() == PDFOBJ_REFERENCE )
					{
						HE_DWORD objNum = ((CHE_PDF_Reference*)pTmpObj)->GetRefNuml();
						CHE_PDF_IndirectObject * pInObj = m_pDoc->GetParser()->GetIndirectObject( objNum );
						if ( pInObj == NULL )
						{
							continue;
						}
						CHE_PDF_Object *  pContentObj = pInObj->GetObject();
						if ( pContentObj->GetType() == PDFOBJ_STREAM )
						{
							CHE_PDF_StreamAcc stmAcc;
							stmAcc.Attach( (CHE_PDF_Stream*)pContentObj );
							buffer.Write( stmAcc.GetData(), stmAcc.GetSize() );
							stmAcc.Detach();
						}
					}
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}
