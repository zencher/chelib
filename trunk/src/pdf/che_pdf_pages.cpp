#include "../../include/pdf/che_pdf_pages.h"
#include <memory>

class IHE_DefaultGetPDFFont : public IHE_PDF_GetFont
{
public:
	IHE_DefaultGetPDFFont( CHE_PDF_Document * pDocument, CHE_Allocator * pAllocator ) : IHE_PDF_GetFont( pAllocator )
		{ m_pDoc = pDocument; }
	~IHE_DefaultGetPDFFont() {};
	
	CHE_PDF_Font * GetFont( HE_DWORD objNum ) { return (m_pDoc!= NULL)?m_pDoc->GetFont(objNum):NULL; }
	
private:
	CHE_PDF_Document * m_pDoc;
};

CHE_PDF_Document::CHE_PDF_Document( CHE_Allocator * pAllocator ) 
: CHE_Object( pAllocator ), m_ID1( pAllocator ), m_ID2( pAllocator ), m_FontMap( pAllocator )
{
	m_pInfoDict = NULL;
	m_pRootDict = NULL;
	m_pParser = NULL;
	m_pPageObjNumList = NULL;
	m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
}

CHE_PDF_Document::CHE_PDF_Document( IHE_Read * pFileRead, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator ), m_ID1( pAllocator ), m_ID2( pAllocator ), m_FontMap( pAllocator )
{
	if ( pFileRead == NULL )
	{
		m_ID1 = "";
		m_ID2 = "";
		m_pInfoDict = NULL;
		m_pRootDict = NULL;
		m_pParser = NULL;
		m_pPageObjNumList = NULL;
		m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
	}else{
		m_pParser = GetAllocator()->New<CHE_PDF_Parser>( GetAllocator() );
		m_pParser->StartParse( pFileRead );
		m_pParser->GetStartxrefOffset( 1024 );
		m_pParser->ParseXRef();
		//m_pParser->VerifyXRef();
		m_pRootDict = m_pParser->GetRootDict();
		m_pInfoDict = m_pParser->GetInfoDict();
		HE_DWORD pageCount = m_pParser->GetPageCount();
		m_pPageObjNumList = GetAllocator()->NewArray<HE_DWORD>( pageCount );
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
		m_pIHE_GetPDFFont = GetAllocator()->New<IHE_DefaultGetPDFFont>( this, GetAllocator() );
	}
}

CHE_PDF_Document::~CHE_PDF_Document()
{
	if ( m_pPageObjNumList )
	{
		GetAllocator()->DeleteArray<HE_DWORD>( m_pPageObjNumList );
	}
	if ( m_pIHE_GetPDFFont )
	{
		GetAllocator()->Delete<IHE_PDF_GetFont>( m_pIHE_GetPDFFont );
	}
	if ( m_pParser )
	{
		m_pParser->CloseParser();
		GetAllocator()->Delete<CHE_PDF_Parser>( m_pParser );
	}
	if ( m_FontMap.GetCount() > 0 )
	{
		CHE_PDF_Font * pTmpFont = NULL;
		for ( HE_DWORD i = 0; i < m_FontMap.GetCount(); i++ )
		{
			pTmpFont = (CHE_PDF_Font*)m_FontMap.GetItemByIndex( i );
			if ( pTmpFont )
			{
				GetAllocator()->Delete<CHE_PDF_Font>( pTmpFont );
			}
		}
		m_FontMap.Clear();
	}
}

HE_BOOL CHE_PDF_Document::Load( IHE_Read * pFileRead )
{
	if ( pFileRead == NULL )
	{
		return FALSE;
	}

	m_pParser = GetAllocator()->New<CHE_PDF_Parser>( GetAllocator() );
	m_pParser->StartParse( pFileRead );
	m_pParser->GetStartxrefOffset( 1024 );
	m_pParser->ParseXRef();
	//m_pParser->VerifyXRef();
	m_pRootDict = m_pParser->GetRootDict();
	m_pInfoDict = m_pParser->GetInfoDict();
	HE_DWORD pageCount = m_pParser->GetPageCount();
	m_pPageObjNumList = GetAllocator()->NewArray<HE_DWORD>( pageCount );
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
	GetAllocator()->Delete<CHE_PDF_Parser>( m_pParser );
	m_pParser = NULL;
	if ( m_pPageObjNumList )
	{
		GetAllocator()->DeleteArray<HE_DWORD>( m_pPageObjNumList );
		m_pPageObjNumList = NULL;
	}
	if ( m_pIHE_GetPDFFont )
	{
		GetAllocator()->Delete<IHE_PDF_GetFont>( m_pIHE_GetPDFFont );
		m_pIHE_GetPDFFont = NULL;
	}
	if ( m_FontMap.GetCount() > 0 )
	{
		CHE_PDF_Font * pTmpFont = NULL;
		for ( HE_DWORD i = 0; i < m_FontMap.GetCount(); i++ )
		{
			pTmpFont = (CHE_PDF_Font*)m_FontMap.GetItemByIndex( i );
			if ( pTmpFont )
			{
				GetAllocator()->Delete<CHE_PDF_Font>( pTmpFont );
			}
		}
		m_FontMap.Clear();
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
	CHE_PDF_Page * pPage = GetAllocator()->New<CHE_PDF_Page>( iPageIndex, (CHE_PDF_Dictionary*)pPageObj, this, GetAllocator() );
	return pPage;
}

HE_VOID	CHE_PDF_Document::ClosePage( CHE_PDF_Page * pPage )
{
	if ( pPage )
	{
		GetAllocator()->Delete<CHE_PDF_Page>( pPage );
	}
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
		pFont = GetAllocator()->New<CHE_PDF_Font>( pDict, m_pParser->GetIHE_GetPDFInObj(), GetAllocator() );
		m_FontMap.Append( objNum, pFont );
		return pFont;
	}
	return pFont;
}

CHE_PDF_Page::CHE_PDF_Page( HE_DWORD lPageIndex, CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc, CHE_Allocator * pAllocator )
: CHE_Object( pAllocator ), m_arrContentObj( pAllocator )
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
	CHE_PDF_ContentObject * pContentObj = NULL;
	for ( HE_DWORD i = 0; i < m_arrContentObj.GetCount(); i++ )
	{
		pContentObj = (CHE_PDF_ContentObject *)m_arrContentObj.GetItem( i );
		if ( pContentObj )
		{
			pContentObj->Release();
		}
	}
	m_arrContentObj.Clear();
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
			CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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
						CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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
						CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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
							CHE_PDF_StreamAcc stmAcc( GetAllocator() );
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

HE_DWORD CHE_PDF_Page::ParseContent()
{
	if ( m_arrContentObj.GetCount() > 0 )
	{
		return m_arrContentObj.GetCount();
	}

	CHE_DynBuffer buf;
	if ( GetPageContent( buf ) == FALSE )
	{
		return 0;
	}

	IHE_Read * pRead = HE_CreateMemBufRead( buf.GetData(), buf.GetSize(), GetAllocator() );
	if ( pRead == NULL )
	{
		return 0;
	}
	CHE_PDF_SyntaxParser sParser(GetAllocator() );
	if ( sParser.InitParser( pRead ) == FALSE )
	{
		pRead->Release();
		HE_DestoryIHERead( pRead );
		return 0;
	}
	
	CHE_PtrStack OpdStack( GetAllocator() );
	CHE_PDF_PARSER_WORD_DES wordDes( GetAllocator() );
	CHE_PDF_Object * pTmpNode = NULL;
	HE_BOOL	bOpd = TRUE;

	//Graphics state
	CHE_PDF_GraphState * pGraphState = GetAllocator()->New<CHE_PDF_GraphState>( GetAllocator() );
	CHE_PtrStack graphStateStack( GetAllocator() );
// 	pGraphState->m_MatrixA = 1;
// 	pGraphState->m_MatrixB = 0;
// 	pGraphState->m_MatrixC = 0;
// 	pGraphState->m_MatrixD = -1;
// 	pGraphState->m_MatrixE = 0;
// 	pGraphState->m_MatrixF = -GetPageHeight();

	//用于维护path对象
	CHE_PDF_PathObject * pPathForCliping = NULL;
	CHE_PDF_PathObject * pLastPath = NULL;
	CHE_PtrQueue SupPathQueue( GetAllocator() );
	HE_FLOAT fBeginX = 0, fBeginY = 0;
	HE_FLOAT fCurX = 0, fCurY = 0;
	HE_BOOL	bSubPathClosed = FALSE;

	CHE_PDF_Dictionary* pFontDict =  GetPageResources();
	pFontDict = (CHE_PDF_Dictionary*)pFontDict->GetElement( "Font" );

	while( sParser.GetWord( wordDes ) == TRUE )
	{
		bOpd = TRUE;
		switch ( wordDes.type )
		{
		case PDFPARSER_WORD_INTEGER:
			pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_FLOAT:
			pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_NAME:
			pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_ARRAY_B:
			sParser.SetPos( wordDes.offset );
			pTmpNode = sParser.GetArray();
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_DICT_B:
			sParser.SetPos( wordDes.offset );
			pTmpNode = sParser.GetDictionary();
			OpdStack.Push( pTmpNode );
			break;
		case PDFPARSER_WORD_STRING:
			pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, GetAllocator() );
			OpdStack.Push( pTmpNode );
			break;
		default:
			bOpd = FALSE;
			break;
		}
		if ( bOpd == TRUE )
		{
			continue;
		}

		//text对象
		if ( wordDes.str == "BT" )
		{
			CHE_PDF_TextObject * pTmpText = CHE_PDF_TextObject::Create( GetAllocator() );
			CHE_PDF_TextObjectItem * pTmpTextItem = NULL; 

			CHE_PDF_ContentObject * pCurContentObj = pTmpText;

			while ( sParser.GetWord( wordDes ) == TRUE )
			{
				bOpd = TRUE;
				if ( wordDes.str == "ET" )
				{
					if ( pTmpTextItem != NULL )
					{
						GetAllocator()->Delete<CHE_PDF_TextObjectItem>( pTmpTextItem );
					}
					m_arrContentObj.Append( (HE_LPVOID)pCurContentObj );
					break;
				}
				if ( pTmpTextItem == NULL )
				{
					pTmpTextItem = GetAllocator()->New<CHE_PDF_TextObjectItem>( GetAllocator() );
				}

				switch ( wordDes.type )
				{
				case PDFPARSER_WORD_INTEGER:
					pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetInteger(), 0, 0, GetAllocator() );
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_FLOAT:
					pTmpNode = CHE_PDF_Number::Create( wordDes.str.GetFloat(), 0, 0, GetAllocator() );
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_NAME:
					pTmpNode = CHE_PDF_Name::Create( wordDes.str, 0, 0, GetAllocator() );
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_ARRAY_B:
					sParser.SetPos( wordDes.offset );
					pTmpNode = sParser.GetArray();
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_DICT_B:
					sParser.SetPos( wordDes.offset );
					pTmpNode = sParser.GetDictionary();
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_STRING:
					pTmpNode = CHE_PDF_String::Create( wordDes.str, 0, 0, GetAllocator() );
					OpdStack.Push( pTmpNode );
					break;
				case PDFPARSER_WORD_UNKNOWN:
					{
						if ( wordDes.str == "null" )
						{
							pTmpNode = CHE_PDF_Null::Create( 0, 0, GetAllocator() );
							OpdStack.Push( pTmpNode );
							break;
						}else if ( wordDes.str == "false" )
						{
							pTmpNode = CHE_PDF_Boolean::Create( FALSE, 0, 0, GetAllocator() );
							OpdStack.Push( pTmpNode );
							break;
						}else if ( wordDes.str == "true" )
						{
							pTmpNode = CHE_PDF_Boolean::Create( TRUE, 0, 0, GetAllocator() );
							OpdStack.Push( pTmpNode );
							break;
						}
					}
				default:
					bOpd = FALSE;
					break;
				}
				if ( bOpd == TRUE )
				{
					continue;
				}

				//处理指令
				if ( wordDes.str == "Tc" )	//char space //1 opd
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetCharSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber() );
						pTmpNode->Release();
					}
				}else if ( wordDes.str == "Tw" )	//word space //1 opd
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetWordSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber() );
						pTmpNode->Release();
					}
				}else if ( wordDes.str == "Tz" )	//word space //1 opd
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetScale( ((CHE_PDF_Number*)pTmpNode)->GetInteger() );
						pTmpNode->Release();
					}
				}else if ( wordDes.str == "TL" )	//word space //1 opd
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetLeading( ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber() );
						pTmpNode->Release();
					}
				}else if ( wordDes.str == "Tf" )
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetSize( ((CHE_PDF_Number*)pTmpNode)->GetInteger() );
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if ( pFontDict == NULL )
					{
						if ( pTmpNode != NULL )
						{
							pTmpNode->Release();
						}
					}else{
						if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NAME )
						{
							CHE_PDF_Reference * pFontRef =  (CHE_PDF_Reference *)pFontDict->GetElement( ((CHE_PDF_Name*)pTmpNode)->GetString() );
							if ( pFontRef != NULL && pFontRef->GetType() == PDFOBJ_REFERENCE )
							{
								pTmpTextItem->SetFontObj( pFontRef->GetRefNuml() );
							}
							pTmpNode->Release();
						}
					}
				}else if ( wordDes.str == "Tr" )
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetRender( ((CHE_PDF_Number*)pTmpNode)->GetInteger() );\
						pTmpNode->Release();
					}
				}else if ( wordDes.str == "Ts" )
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetRise( ((CHE_PDF_Number*)pTmpNode)->GetInteger() );
						pTmpNode->Release();
					}
				}else if( wordDes.str == "Td" )
				{
					HE_DWORD x = 0, y = 0; 
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						y = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						x = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
						pTmpNode->Release();
					}
					pTmpTextItem->SetPosi( x, y );
				}else if( wordDes.str == "TD" )
				{
					HE_FLOAT x = 0, y = 0; 
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						y = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						x = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					HE_FLOAT f = y;
					pTmpTextItem->SetLeading( -f );
					pTmpTextItem->SetPosi( (HE_DWORD)x, (HE_DWORD)y );
				}else if( wordDes.str == "Tm" )
				{
					HE_FLOAT a = 0, b = 0, c = 0, d = 0, e = 0, f = 0; 
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						f = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						e = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						d = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						c = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						b = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						a = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
						pTmpNode->Release();
					}
					pTmpTextItem->SetMatrix( a, b, c, d, e, f );
				}else if( wordDes.str == "T*" )
				{
					pTmpTextItem->SetPosi( 0, (HE_DWORD)pTmpTextItem->GetLeading() );
				}else if ( wordDes.str == "Tj" || wordDes.str == "'" ) 
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if ( pTmpNode != NULL )
					{
						pTmpTextItem->SetText( pTmpNode );
						pTmpText->AppendItem( pTmpTextItem );
					}
					pTmpTextItem = NULL;
				}else if ( wordDes.str == "\"" )
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if ( pTmpNode != NULL )
					{
						pTmpTextItem->SetText( pTmpNode );
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetCharSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber() );
						pTmpNode->Release();
					}
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
					{
						pTmpTextItem->SetWordSpace( ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber() );
						pTmpText->AppendItem( pTmpTextItem );
						pTmpNode->Release();
					}
					pTmpTextItem = NULL;
				}else if ( wordDes.str == "TJ" )
				{
					OpdStack.Pop( (HE_LPVOID*)&pTmpNode );
					if ( pTmpNode != NULL )
					{
						pTmpTextItem->SetText( pTmpNode );
						pTmpText->AppendItem( pTmpTextItem );
					}
					pTmpTextItem = NULL;
				}
				//清除无用的操作数
				while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
				{
					if ( pTmpNode != NULL )
					{
						pTmpNode->Release();
					}
				}
			}
		}

		//external object
		if ( wordDes.str == "Do" )
		{
			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//inline images object
		if ( wordDes.str == "BI" )
		{
			//跳过inline images
			sParser.SeekToMark( CHE_ByteString("EI") );

			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//shading object
		if ( wordDes.str == "sh" )
		{
			//清除无用的操作数
			while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL )
				{
					pTmpNode->Release();
				}
			}
			continue;
		}

		//其他的对象path，cliping，gs，ss，mark content，text state，color等相关对象和操作符号的处理
		if ( wordDes.str == "m" )
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					fCurY = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					fCurX = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			fBeginX = fCurX;
			fBeginY = fCurY;
			bSubPathClosed = FALSE;
		}else if ( wordDes.str == "l" )
		{
			HE_FLOAT x = 0, y = 0; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, x, y, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x;
			fCurY = y;
		}else if ( wordDes.str == "c" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y3 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x3 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, x1, y1, x2, y2, x3, y3, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x3;
			fCurY = y3;
		}else if ( wordDes.str == "v" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, fCurX, fCurY, x1, y1, x2, y2, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x2;
			fCurY = y2;
		}else if ( wordDes.str == "y" )
		{
			HE_FLOAT x1 = 0, y1 = 0, x2 = 0, y2 = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x2 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x1 = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_BCurve>( fCurX, fCurY, x1, y1, x2, y2, x2, y2, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x2;
			fCurY = y2;
		}else if ( wordDes.str == "h" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
		}else if ( wordDes.str == "re" )
		{
			HE_FLOAT x = 0, y = 0, width = 0, height = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					height = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					width = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					y = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					x = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Rectangle>( x, y, width, height, GetAllocator() );
			SupPathQueue.Push( (HE_LPVOID)pGraph );
			fCurX = x;
			fCurY = y;
			bSubPathClosed = TRUE;
		}else if ( wordDes.str == "S" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			pPath->SetOperator( PATH_OPERATOR_STROKE );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "s" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			pPath->SetOperator( PATH_OPERATOR_STROKE );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "f" || wordDes.str == "F" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			pPath->SetOperator( PATH_OPERATOR_FILL );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "f*" )
		{
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			pPath->SetOperator( PATH_OPERATOR_FILL );
			pPath->SetFillMode( PATH_FILL_MODE_EVERODD );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "B" )
		{
			CHE_PDF_PathObject * pPathFill = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_PDF_PathObject * pPathStroke = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPathFill->AppendItem( pSubPath );
					pPathStroke->AppendItem( pSubPath );
				}
			}
			pPathFill->SetOperator( PATH_OPERATOR_FILL );
			m_arrContentObj.Append( (HE_LPVOID)pPathFill );
			m_arrContentObj.Append( (HE_LPVOID)pPathStroke );
			pLastPath = pPathStroke;
		}else if ( wordDes.str == "B*" )
		{
			CHE_PDF_PathObject * pPathFill = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_PDF_PathObject * pPathStroke = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPathFill->AppendItem( pSubPath );
					pPathStroke->AppendItem( pSubPath );
				}
			}
			pPathFill->SetOperator( PATH_OPERATOR_FILL );
			pPathFill->SetFillMode( PATH_FILL_MODE_EVERODD );
			m_arrContentObj.Append( (HE_LPVOID)pPathFill );
			m_arrContentObj.Append( (HE_LPVOID)pPathStroke );
			pLastPath = pPathStroke;
		}else if ( wordDes.str == "b" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetOperator( PATH_OPERATOR_FILLSTROKE );
			pPath->SetFillMode( PATH_FILL_MODE_NOZERO );
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "b*" )
		{
			if ( bSubPathClosed == FALSE )
			{
				CHE_GraphicsObject * pGraph = GetAllocator()->New<CHE_Line>( fCurX, fCurY, fBeginX, fBeginY, GetAllocator() );
				SupPathQueue.Push( (HE_LPVOID)pGraph );
				bSubPathClosed = TRUE;
			}
			CHE_PDF_PathObject * pPath = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPath->AppendItem( pSubPath );
				}
			}
			pPath->SetOperator( PATH_OPERATOR_FILLSTROKE );
			pPath->SetFillMode( PATH_FILL_MODE_EVERODD );
			m_arrContentObj.Append( (HE_LPVOID)pPath );
			pLastPath = pPath;
		}else if ( wordDes.str == "n" )
		{
			if ( pPathForCliping != NULL )
			{
				pPathForCliping->GetAllocator()->Delete<CHE_PDF_PathObject>( pPathForCliping );
			}
			pPathForCliping = GetAllocator()->New<CHE_PDF_PathObject>( pGraphState->Clone(), GetAllocator() );
			CHE_GraphicsObject * pSubPath = NULL;
			while( SupPathQueue.Pop( (HE_LPVOID*)&pSubPath ) == TRUE )
			{
				if ( pSubPath )
				{
					pPathForCliping->AppendItem( pSubPath );
				}
			}
		}else if ( wordDes.str == "W" )
		{
		}else if ( wordDes.str == "W*" )
		{
		}else if ( wordDes.str == "cm" )
		{
			HE_FLOAT a = 0, b = 0, c = 0, d = 0, e = 0, f = 0 ; 
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					f = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					e = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					d = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					c = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					b = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					a = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			pGraphState->m_MatrixA = pGraphState->m_MatrixA * a + pGraphState->m_MatrixB * c;
			pGraphState->m_MatrixB = pGraphState->m_MatrixA * b + pGraphState->m_MatrixB * a;
			pGraphState->m_MatrixC = pGraphState->m_MatrixC * a + pGraphState->m_MatrixD * c;
			pGraphState->m_MatrixD = pGraphState->m_MatrixC * b + pGraphState->m_MatrixD * d;
			pGraphState->m_MatrixE = pGraphState->m_MatrixE * a + pGraphState->m_MatrixF * c + e;
			pGraphState->m_MatrixF = pGraphState->m_MatrixE * b + pGraphState->m_MatrixF * d + f;
		}else if ( wordDes.str == "q" )
		{
			graphStateStack.Push( (HE_LPVOID)pGraphState );
			CHE_PDF_GraphState * pTmp = GetAllocator()->New<CHE_PDF_GraphState>( GetAllocator() );
			memcpy( pTmp, pGraphState, sizeof(CHE_PDF_GraphState) );
			pGraphState = pTmp;
		}else if ( wordDes.str == "Q" )
		{
			CHE_PDF_GraphState * pTmp = NULL;
			graphStateStack.Pop( (HE_LPVOID*)&pTmp );
			pGraphState->GetAllocator()->Delete<CHE_PDF_GraphState>( pGraphState );
			pGraphState = pTmp;
		}else if ( wordDes.str == "gs" )
		{
		}else if ( wordDes.str == "w" )	//line width
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					pGraphState->m_LineWidth = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "J" )	//line cap
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					pGraphState->m_LineCap = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "j" )	//line join
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					pGraphState->m_LineJoin = ((CHE_PDF_Number*)pTmpNode)->GetInteger();
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "M" )	//Miter limit
		{
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					pGraphState->m_MiterLimit = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
		}else if ( wordDes.str == "d" )	//dash
		{
			HE_FLOAT dashPhase = 0;
			HE_FLOAT * dashArray = 0;
			HE_DWORD dashArraySize = 0;
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_NUMBER )
				{
					dashPhase = ((CHE_PDF_Number*)pTmpNode)->GetFloatNumber();
					pTmpNode->Release();
				}
			}
			if( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
			{
				if ( pTmpNode != NULL && pTmpNode->GetType() == PDFOBJ_ARRAY )
				{
					dashArraySize = ((CHE_PDF_Array*)pTmpNode)->GetCount();
					dashArray = GetAllocator()->NewArray<HE_FLOAT>( dashArraySize );
					CHE_PDF_Object * pObj = NULL;
					for ( HE_DWORD i = 0; i < dashArraySize; i++ )
					{
						pObj = ((CHE_PDF_Array*)pTmpNode)->GetElement( i );
						if ( pObj != NULL && pObj->GetType() == PDFOBJ_NUMBER )
						{
							dashArray[i] = ((CHE_PDF_Number*)pObj)->GetFloatNumber();
						}
					}
					pTmpNode->Release();
				}
			}
			if ( dashArray )
			{
				pGraphState->SetDashArray( dashArraySize, dashArray, dashPhase );
			}
		}else if ( wordDes.str == "ri" )
		{
			//
		}else if ( wordDes.str == "i" )
		{
			//
		}
		 
		//清除无用的操作数
		while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
		{
			if ( pTmpNode != NULL )
			{
				pTmpNode->Release();
			}
		}
	}


	if ( pPathForCliping != NULL )
	{
		pPathForCliping->GetAllocator()->Delete<CHE_PDF_PathObject>( pPathForCliping );
	}

	pRead->Release();
	HE_DestoryIHERead( pRead );

	while ( OpdStack.Pop( (HE_LPVOID*)&pTmpNode ) == TRUE )
	{
		if ( pTmpNode != NULL )
		{
			pTmpNode->Release();
		}
	}
	//清除无用的subpath
	CHE_GraphicsObject * pTmpGraph = NULL;
	while ( SupPathQueue.Pop( (HE_LPVOID*)&pTmpGraph ) == TRUE )
	{
		if ( pTmpGraph != NULL )
		{
			pTmpGraph->Release();
		}
	}


	return m_arrContentObj.GetCount();
}