#include "../../include/pdf/che_pdf_pages.h"
#include <memory>

class IHE_DefaultGetPDFFont : public IHE_PDF_GetFont
{
public:
	IHE_DefaultGetPDFFont( CHE_PDF_Document * pDocument ) { m_pDoc = pDocument; }
	~IHE_DefaultGetPDFFont() {};
	
	CHE_PDF_Font * GetFont( HE_DWORD objNum ) { return (m_pDoc!= NULL)?m_pDoc->GetFont(objNum):NULL; }
	
private:
	CHE_PDF_Document * m_pDoc;
};

CHE_PDF_Document::CHE_PDF_Document() : m_ID1(), m_ID2()
{
	m_pInfoDict = NULL;
	m_pRootDict = NULL;
	m_pParser = NULL;
	m_pPageObjNumList = NULL;
	m_pIHE_GetPDFFont = new IHE_DefaultGetPDFFont( this );
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
		m_pIHE_GetPDFFont = new IHE_DefaultGetPDFFont( this );
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
		m_pIHE_GetPDFFont = new IHE_DefaultGetPDFFont( this );
	}
}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( m_pPageObjNumList )
	{
		delete [] m_pPageObjNumList;
	}
	if ( m_pIHE_GetPDFFont )
	{
		delete m_pIHE_GetPDFFont;
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

HE_VOID CHE_PDF_Document::Unload()
{
	m_pParser->CloseParser();
	delete m_pParser;
	m_pParser = NULL;
	if ( m_pPageObjNumList )
	{
		delete [] m_pPageObjNumList;
		m_pPageObjNumList = NULL;
	}
	if ( m_pIHE_GetPDFFont )
	{
		delete m_pIHE_GetPDFFont;
		m_pIHE_GetPDFFont = NULL;
	}
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
	if ( objNum == 0 )
	{
		return NULL;
	}
	CHE_PDF_Object * pPageObj =  m_pParser->GetIndirectObject( objNum );
	if ( pPageObj == NULL )
	{
		return NULL;
	}
	pPageObj = ((CHE_PDF_IndirectObject*)pPageObj)->GetObject();
	CHE_PDF_Page * pPage = new CHE_PDF_Page( iPageIndex, (CHE_PDF_Dictionary*)pPageObj, this );
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
	CHE_PDF_Object * pObj = m_pParser->GetTrailerDict()->GetElement( "Encrypt" );
	if ( pObj == NULL )
	{
		return FALSE;
	}else{
		return TRUE;
	}
}
 
CHE_PDF_Array * CHE_PDF_Document::GetPageMediaBox( CHE_PDF_Dictionary * pPageDict )
{
	if ( pPageDict == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = pPageDict->GetElement( "MediaBox" );
	if ( pObj == NULL )
	{
		CHE_PDF_IndirectObject * pInObj = NULL;
		CHE_PDF_Dictionary * pCurDict = pPageDict;
		while ( pCurDict )
		{
			pObj = pCurDict->GetElement( "Parent" );
			if ( pObj == NULL )
			{
				return NULL;
			}
			if ( pObj->GetType() != PDFOBJ_REFERENCE )
			{
				return NULL;
			}else{
				pInObj = m_pParser->GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
				if ( pInObj == NULL )
				{
					return NULL;
				}
				pObj = pInObj->GetDict();
				if ( pObj == NULL || pObj->GetType() != PDFOBJ_DICTIONARY )
				{
					return NULL;
				}
				pObj = ((CHE_PDF_Dictionary*)pObj)->GetElement( "MediaBox" );
				if ( pObj != NULL )
				{
					break;
				}else{
					pCurDict = pInObj->GetDict();
				}
			}
		}
	}
	if ( pObj->GetType() == PDFOBJ_ARRAY )
	{
		return (CHE_PDF_Array*)pObj;
	}else if ( pObj->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInOBj = m_pParser->GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
		if ( pInOBj == NULL )
		{
			return NULL;
		}else if ( pInOBj->GetObject()->GetType() == PDFOBJ_ARRAY )
		{
			return (CHE_PDF_Array*)pInOBj->GetObject();
		}else{
			return NULL;
		}
	}else{
		return NULL;
	}
}

CHE_PDF_Dictionary * CHE_PDF_Document::GetPageResources( CHE_PDF_Dictionary * pPageDict )
{
	if ( pPageDict == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = pPageDict->GetElement( "Resources" );
	if ( pObj == NULL )
	{
		CHE_PDF_IndirectObject * pInObj = NULL;
		CHE_PDF_Dictionary * pCurDict = pPageDict;
		while ( pCurDict )
		{
			pObj = pCurDict->GetElement( "Parent" );
			if ( pObj == NULL )
			{
				return NULL;
			}
			if ( pObj->GetType() != PDFOBJ_REFERENCE )
			{
				return NULL;
			}else{
				pInObj = m_pParser->GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
				if ( pInObj == NULL )
				{
					return NULL;
				}
				pObj = pInObj->GetDict();
				if ( pObj == NULL || pObj->GetType() != PDFOBJ_DICTIONARY )
				{
					return NULL;
				}
				pObj = ((CHE_PDF_Dictionary*)pObj)->GetElement( "Resources" );
				if ( pObj != NULL )
				{
					break;
				}else{
					pCurDict = pInObj->GetDict();
				}
			}
		}
	}
	if ( pObj == NULL )
	{
		return NULL;
	}
	if ( pObj->GetType() == PDFOBJ_DICTIONARY )
	{
		return (CHE_PDF_Dictionary*)pObj;
	}else if ( pObj->GetType() == PDFOBJ_REFERENCE )
	{
		CHE_PDF_IndirectObject * pInOBj = m_pParser->GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
		if ( pInOBj == NULL )
		{
			return NULL;
		}else if ( pInOBj->GetObject()->GetType() == PDFOBJ_DICTIONARY )
		{
			return (CHE_PDF_Dictionary*)pInOBj->GetObject();
		}else{
			return NULL;
		}
	}else{
		return NULL;
	}
}

CHE_PDF_Font *	CHE_PDF_Document::GetFont( HE_DWORD objNum )
{
	CHE_PDF_Font * pFont = (CHE_PDF_Font *)m_FontMap.GetItem( objNum );
	if ( pFont == NULL )
	{
		CHE_PDF_IndirectObject * pInObj = m_pParser->GetIndirectObject( objNum );
		if ( pInObj == NULL )
		{
			return NULL;
		}
		CHE_PDF_Dictionary * pDict =  pInObj->GetDict();
		if ( pDict == NULL )
		{
			return NULL;
		}
		pFont = new CHE_PDF_Font( pDict, m_pParser->GetIHE_GetPDFInObj() );
		m_FontMap.Append( objNum, pFont );
		return pFont;
	}
	return pFont;
}

CHE_PDF_Page::CHE_PDF_Page( HE_DWORD lPageIndex, CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc )
{
	m_lPageIndex = lPageIndex;
	m_pPageObj = NULL;
	m_fPageHeight = 0.0;
	m_fPageWidth = 0.0;
	m_pDoc = pDoc;
	if ( pDict )
	{
		m_pPageObj = pDict;
		CHE_PDF_Object * pObj = NULL;
		if ( m_pDoc )
		{
			pObj = m_pDoc->GetPageMediaBox( m_pPageObj );
		}
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
	CHE_PDF_Object * pPageContent = pPageDict->GetElement( "Contents" );
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
