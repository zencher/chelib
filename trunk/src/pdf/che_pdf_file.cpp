#include "../../include/pdf/che_pdf_file.h"
#include "../../include/pdf/che_pdf_creator.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mpParser(NULL), mVersion(PDF_VERSION_1_7), mObjCollector(pAllocator), mXRefTable(pAllocator) {}

CHE_PDF_File::~CHE_PDF_File()
{
	Close();
}

HE_BOOL CHE_PDF_File::Open( IHE_Read * pRead )
{
	if ( pRead == NULL )
	{
		return FALSE;
	}
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mObjCollector.Clear();
		mXRefTable.Clear();
	}
	mpParser = CHE_PDF_Parser::Create( this, pRead, &mXRefTable, GetAllocator() );
	if ( mpParser )
	{
		mVersion = mpParser->GetPDFVersion();
		return TRUE;
	}
	return FALSE;
}

HE_VOID CHE_PDF_File::Close()
{
 	mObjCollector.Clear();
 	mXRefTable.Clear();
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mpParser = NULL;
	}
}

HE_BOOL CHE_PDF_File::Save( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
	if ( pCreator == NULL )
	{
		return FALSE;
	}

	HE_DWORD objCount = 0;
	HE_DWORD offset = 0;
	CHE_PDF_XREF_Entry entry;
	CHE_PDF_XREF_Table xref( GetAllocator() );
	HE_PDF_RefInfo refInfo;
	CHE_PDF_ObjectPtr ObjPtr;

	pCreator->OutPutFileHead( GetPDFVersion() );

	entry.ObjNum = 0;
	entry.Field1 = 0;
	entry.Field2 = 65535;
	entry.Type = XREF_ENTRY_TYPE_FREE;

	xref.Add( entry );
	++objCount;

	mXRefTable.MoveFirst();
	while( !mXRefTable.IsEOF() )
	{
		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			refInfo.objNum = entry.GetObjNum();
			refInfo.genNum = 0;
			ObjPtr = GetObject( refInfo );
			if ( ObjPtr )
			{
				if ( ObjPtr->GetType() == OBJ_TYPE_STREAM )
				{
					CHE_PDF_DictionaryPtr pDict = ObjPtr.GetStreamPtr()->GetDict();
					CHE_PDF_NamePtr pName;
					pName = pDict->GetElement( "Type", OBJ_TYPE_NAME ).GetNamePtr();
					if ( pName )
					{
						if ( ( pName->GetString() == "ObjStm" ) || ( pName->GetString() == "XRef" ) )
						{
							mXRefTable.MoveNext();
							continue;
						}
					}
				}
				offset = pCreator->OutPutInObject( refInfo, ObjPtr );
				entry.Type = XREF_ENTRY_TYPE_COMMON;
				entry.ObjNum = refInfo.objNum;
				entry.Field1 = offset;
				entry.Field2 = 0;
				xref.Add( entry );
				++objCount;
			}
		}
		mXRefTable.MoveNext();
	}

	offset = pCreator->OutPutXRefTable( xref );
	CHE_PDF_ReferencePtr pRef;
	CHE_PDF_DictionaryPtr pDict = mXRefTable.GetTrailer();
	CHE_PDF_DictionaryPtr pNewDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	pRef = pDict->GetElement( "Root", OBJ_TYPE_REFERENCE ).GetReferencePtr();
	if ( pRef )
	{
		pNewDict->SetAtObj( "Root", pRef );
	}
	pRef = pDict->GetElement( "Info", OBJ_TYPE_REFERENCE ).GetReferencePtr();
	if ( pRef )
	{
		pNewDict->SetAtObj( "Info", pRef );
	}
	pNewDict->SetAtInteger( "Size", objCount );
	pCreator->OutPutTailerDict( pNewDict );
	pCreator->OutPutFileTailer( offset );
	pCreator->GetAllocator()->Delete( pCreator );
	return TRUE;
}

HE_BOOL CHE_PDF_File::SaveCompact( IHE_Write * pWrite )
{
// 	if ( pWrite == NULL )
// 	{
// 		return FALSE;
// 	}
// 
// 	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
// 	if ( pCreator == NULL )
// 	{
// 		return FALSE;
// 	}
// 
// 	CHE_DynBuffer dynBuffer( 1024, 1024 * 4, GetAllocator() );
// 	IHE_Write * pBufWrite = HE_CreateDynBufferWrite( &dynBuffer, GetAllocator() );
// 	if ( pWrite == NULL )
// 	{
// 		pCreator->GetAllocator()->Delete( pCreator );
// 		return FALSE;
// 	}
// 
// 	HE_DWORD objCount = 0;
// 	HE_DWORD objInStmCount = 0;
// 	HE_DWORD offset = 0;
// 	HE_DWORD nextObjNum = mXRefTable.GetMaxObjNum()+1;
// 	CHE_PDF_XREF_Entry entry;
// 	CHE_PDF_XREF_Table xref( GetAllocator() );
// 	HE_PDF_RefInfo refInfo;
// 	CHE_PDF_IndirectObject * pInObj = NULL;
// 	CHE_Queue< HE_DWORD > objNumQueue;
// 	CHE_Queue< HE_DWORD > objOffset;
// 
// 	pCreator->OutPutFileHead( GetPDFVersion() );
// 
// 	entry.ObjNum = 0;
// 	entry.Field1 = 0;
// 	entry.Field2 = 65535;
// 	entry.Type = XREF_ENTRY_TYPE_FREE;
// 
// 	xref.Add( entry );
// 	++objCount;
// 
// 	mXRefTable.MoveFirst();
// 	while( !mXRefTable.IsEOF() )
// 	{
// 		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
// 		{
// 			refInfo.objNum = entry.GetObjNum();
// 			refInfo.genNum = 0;
// 			pInObj = GetInObject( refInfo );
// 			if ( pInObj )
// 			{
// 				//跳过不需要输出的对象流对象和交叉索引流对象
// 				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
// 				{
// 					CHE_PDF_Dictionary * pDict = pInObj->GetObj()->GetStream()->GetDict();
// 					CHE_PDF_Object * pObj = NULL;
// 					CHE_PDF_Name * pName = NULL;
// 					//CHE_PDF_ObjectCollector objCollecor;
// 					pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME/*, objCollecor*/ );
// 					if ( pObj )
// 					{
// 						pName = pObj->GetName();
// 						if ( pName )
// 						{
// 							if ( pName->GetString() == "ObjStm" )
// 							{
// 								mXRefTable.MoveNext();
// 								continue;
// 							}
// 						}
// 					}
// 				}
// 				entry.Type = XREF_ENTRY_TYPE_FREE;
// 				xref.Add( entry );
// 				++objCount;
// 			}
// 		}
// 		mXRefTable.MoveNext();
// 	}
// 
// 	//输出Catalog Dict Object
// 	CHE_PDF_Reference * pRef = NULL;
// 	CHE_PDF_Object * pObj = NULL;
// 	CHE_PDF_Dictionary * pTrailerDict = mXRefTable.GetTrailer();
// 	pObj = pTrailerDict->GetElement( "Root" );
// 	if ( pObj->GetType() == OBJ_TYPE_REFERENCE )
// 	{
// 		pRef = pObj->GetReference();
// 	}
// 	if ( pRef )
// 	{
// 		HE_PDF_RefInfo refInfo = pRef->GetRefInfo();
// 		CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );
// 
// 		offset = pCreator->OutPutInObject( pInObj );
// 		entry.Type = XREF_ENTRY_TYPE_COMMON;
// 		entry.ObjNum = refInfo.objNum;
// 		entry.Field1 = offset;
// 		entry.Field2 = 0;
// 		xref.Update( refInfo.objNum, entry );
// 	}
// 
// 	xref.MoveFirst();
// 	while( !xref.IsEOF() )
// 	{
// 		if ( xref.GetCurNode( entry ) && entry.GetObjNum() != 0 )
// 		{
// 			if ( entry.GetType() == XREF_ENTRY_TYPE_FREE )
// 			{
// 				refInfo.objNum = entry.GetObjNum();
// 				refInfo.genNum = 0;
// 				pInObj = GetInObject( refInfo );
// 				if ( pInObj == NULL )
// 				{
// 					xref.MoveNext();
// 					continue;
// 				}
// 				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
// 				{
// 					offset = pCreator->OutPutInObject( pInObj );
// 					entry.Type = XREF_ENTRY_TYPE_COMMON;
// 					entry.ObjNum = refInfo.objNum;
// 					entry.Field1 = offset;
// 					entry.Field2 = 0;
// 					xref.Update( refInfo.objNum, entry );
// 				}else if ( entry.GetGenNum() == 0  )
// 				{
// 					offset = pBufWrite->GetCurOffset();
// 					CHE_PDF_Creator::OutPutObject( pBufWrite, pInObj->GetObj() );
// 					pBufWrite->WriteByte( '\n' );
// 					objNumQueue.Push( refInfo.objNum );
// 					objOffset.Push( offset );
// 					++objInStmCount;
// 					
// 					if ( objInStmCount == 30 )
// 					{
// 						HE_DWORD objNum = 0;
// 						HE_DWORD offset = 0;
// 						HE_DWORD index = 0;
// 						CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
// 						HE_CHAR tmpStr[1024];
// 						CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );
// 						CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
// 						while( objNumQueue.Pop( objNum ) )
// 						{
// 							objOffset.Pop( offset );
// 
// 							entry.Type = XREF_ENTRY_TYPE_COMPRESSED;
// 							entry.ObjNum = objNum;
// 							entry.Field1 = nextObjNum-1;
// 							entry.Field2 = index;
// 							xref.Update( objNum, entry );
// 
// 							sprintf( tmpStr, "%d %d ", objNum, offset );
// 							tmpBuf.Write( (HE_LPCBYTE)(&(tmpStr[0])), strlen(tmpStr) );
// 
// 							++index;
// 						}
// 
// 						offset = tmpBuf.GetSize();
// 						pDict->SetAtName( "Type", "ObjStm" );
// 						pDict->SetAtInteger( "N", objInStmCount );
// 						pDict->SetAtInteger( "First", offset );
// 						pStm->SetDict( pDict );
// 
// 						tmpBuf.Write( dynBuffer );
// 						pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );
// 
// 						CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( nextObjNum-1, 0, pStm, this, GetAllocator() );
// 						offset = pCreator->OutPutInObject( pInObj );
// 						entry.Type = XREF_ENTRY_TYPE_COMMON;
// 						entry.ObjNum = nextObjNum-1;
// 						entry.Field1 = offset;
// 						entry.Field2 = 0;
// 						xref.Add( entry );
// 				
// 						pInObj->Release();
// 
// 						dynBuffer.Clear();
// 						objInStmCount = 0;
// 						++objCount;
// 					}
// 				}else{
// 					offset = pCreator->OutPutInObject( pInObj );
// 					entry.Type = XREF_ENTRY_TYPE_COMMON;
// 					entry.ObjNum = refInfo.objNum;
// 					entry.Field1 = offset;
// 					entry.Field2 = 0;
// 					xref.Update( refInfo.objNum, entry );
// 				}
// 			}
// 		}
// 		xref.MoveNext();
// 	}
// 
// 	if ( ! objNumQueue.IsEmpty() )
// 	{
// 		HE_DWORD objNum = 0;
// 		HE_DWORD offset = 0;
// 		HE_DWORD index = 0;
// 		CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
// 		HE_CHAR tmpStr[1024];
// 		CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( nextObjNum++, 0, NULL, GetAllocator() );
// 		CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
// 		while( objNumQueue.Pop( objNum ) )
// 		{
// 			objOffset.Pop( offset );
// 
// 			entry.Type = XREF_ENTRY_TYPE_COMPRESSED;
// 			entry.ObjNum = objNum;
// 			entry.Field1 = nextObjNum-1;
// 			entry.Field2 = index;
// 			xref.Update( objNum, entry );
// 
// 			sprintf( tmpStr, "%d %d ", objNum, offset );
// 			tmpBuf.Write( (HE_LPCBYTE)(&(tmpStr[0])), strlen(tmpStr) );
// 
// 			++index;
// 		}
// 
// 		offset = tmpBuf.GetSize();
// 		pDict->SetAtName( "Type", "ObjStm" );
// 		pDict->SetAtInteger( "N", objInStmCount );
// 		pDict->SetAtInteger( "First", offset );
// 		pStm->SetDict( pDict );
// 
// 		tmpBuf.Write( dynBuffer );
// 		pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );
// 
// 		CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( nextObjNum-1, 0, pStm, this, GetAllocator() );
// 		offset = pCreator->OutPutInObject( pInObj );
// 		entry.Type = XREF_ENTRY_TYPE_COMMON;
// 		entry.ObjNum = nextObjNum-1;
// 		entry.Field1 = offset;
// 		entry.Field2 = 0;
// 		xref.Add( entry );
// 
// 		pInObj->Release();
// 
// 		dynBuffer.Clear();
// 		objInStmCount = 0;
// 
// 		++objCount;
// 	}
// 
// 	/*CHE_PDF_Object * pObj = NULL;*/
// 	/*CHE_PDF_Dictionary * pTrailerDict = mXRefTable.GetTrailer();*/
// 	CHE_PDF_Dictionary * pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
// 	CHE_PDF_Stream * pStm = CHE_PDF_Stream::Create( 0, 0, NULL, GetAllocator() );
// 	pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( xref.GetMaxObjNum() + 1, 0, pStm, this, GetAllocator() );
// 	pStm->SetDict( pDict );
// 
// 	pDict->SetAtName( "Type", "XRef" );
// 	pObj = pTrailerDict->GetElement( "Root" );
// 	if ( pObj )
// 	{
// 		pDict->SetAtObj( "Root", pObj->Clone() );
// 	}
// 	pObj = pTrailerDict->GetElement( "Info" );
// 	if ( pObj )
// 	{
// 		pDict->SetAtObj( "Info", pObj->Clone() );
// 	}
// 	pDict->SetAtInteger( "Size", objCount );
// 	CHE_PDF_Array * pArray = CHE_PDF_Array::Create( GetAllocator() );
// 	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
// 	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)3, GetAllocator() ) );
// 	pArray->Append( CHE_PDF_Number::Create( (HE_INT32)1, GetAllocator() ) );
// 	pDict->SetAtArray( "W", pArray );
// 	pArray  = CHE_PDF_Array::Create( GetAllocator() );
// 	pDict->SetAtArray( "Index", pArray );
// 
// 	CHE_DynBuffer tmpBuf( 1024, 1024, GetAllocator() );
// 
// 	HE_DWORD lBeginNum = 0;
// 	HE_DWORD lNextObjNum = 0;
// 	HE_DWORD lCountNum = 0;
// 	//CHE_PDF_XREF_Entry entry;
// 	CHE_Queue< CHE_PDF_XREF_Entry > entryList;
// 
// 	HE_DWORD field1 = 0, field2 = 0, field3 = 0;
// 
// 	xref.MoveFirst();
// 	if ( ! xref.GetCurNode( entry ) )
// 	{
// 		return offset;
// 	}
// 	lBeginNum = entry.GetObjNum();
// 	lNextObjNum = lBeginNum+1;
// 	lCountNum = 1;
// 	entryList.Push( entry );
// 
// 	xref.MoveNext();
// 
// 	while( !xref.IsEOF() )
// 	{
// 		if ( xref.GetCurNode( entry ) )
// 		{
// 			if ( entry.GetObjNum() == lNextObjNum )
// 			{
// 				entryList.Push( entry );
// 				++lCountNum;
// 			}else{
// 				CHE_PDF_XREF_Entry tmpEntry;
// 				pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
// 				pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
// 				while( entryList.Pop( tmpEntry ) )
// 				{
// 					
// 					switch ( tmpEntry.GetType() )
// 					{
// 					case XREF_ENTRY_TYPE_FREE:
// 						field1 = 0; field2 = tmpEntry.GetObjNum(); field3 = tmpEntry.GetGenNum();
// 						break;
// 					case XREF_ENTRY_TYPE_COMMON:
// 						field1 = 1; field2 = tmpEntry.GetOffset(); field3 = tmpEntry.GetGenNum();
// 						break;
// 					case XREF_ENTRY_TYPE_COMPRESSED:
// 						field1 = 2; field2 = tmpEntry.GetParentObjNum(); field3 = tmpEntry.GetIndex();
// 						break;
// 					}
// 					HE_BYTE byte = 0;
// 					tmpBuf.Write( HE_LPCBYTE( &field1 ), 1 );
// 					byte = (HE_BYTE)( field2 >> 16 );
// 					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 					byte = (HE_BYTE)( field2 >> 8 );
// 					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 					byte = (HE_BYTE)( field2 );
// 					tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 					tmpBuf.Write( HE_LPCBYTE( &field3 ), 1 );
// 				}
// 				entryList.Push( entry );
// 				lBeginNum = entry.GetObjNum();
// 				lCountNum = 1;
// 				lNextObjNum = lBeginNum;
// 			}
// 			++lNextObjNum;
// 		}else{
// 			break;
// 		}
// 		xref.MoveNext();
// 	}
// 
// 	if ( lCountNum > 0 )
// 	{
// 		CHE_PDF_XREF_Entry tmpEntry;
// 		pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lBeginNum, GetAllocator() ) );
// 		pArray->Append( CHE_PDF_Number::Create( (HE_INT32)lCountNum, GetAllocator() ) );
// 		while( entryList.Pop( tmpEntry ) )
// 		{
// 
// 			switch ( tmpEntry.GetType() )
// 			{
// 			case XREF_ENTRY_TYPE_FREE:
// 				field1 = 0; field2 = tmpEntry.GetObjNum(); field3 = tmpEntry.GetGenNum();
// 				break;
// 			case XREF_ENTRY_TYPE_COMMON:
// 				field1 = 1; field2 = tmpEntry.GetOffset(); field3 = tmpEntry.GetGenNum();
// 				break;
// 			case XREF_ENTRY_TYPE_COMPRESSED:
// 				field1 = 2; field2 = tmpEntry.GetParentObjNum(); field3 = tmpEntry.GetIndex();
// 				break;
// 			}
// 			HE_BYTE byte = 0;
// 			tmpBuf.Write( HE_LPCBYTE( &field1 ), 1 );
// 			byte = (HE_BYTE)( field2 >> 16 );
// 			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 			byte = (HE_BYTE)( field2 >> 8 );
// 			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 			byte = (HE_BYTE)( field2 );
// 			tmpBuf.Write( HE_LPCBYTE( &byte ), 1 );
// 			tmpBuf.Write( HE_LPCBYTE( &field3 ), 1 );
// 		}
// 	}
// 
// 	pStm->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );
// 
// 	offset = pCreator->OutPutInObject( pInObj );
// 	pCreator->OutPutFileTailer( offset );
// 
// 	pStm->Release();
// 	pCreator->GetAllocator()->Delete( pCreator );
 
 	return TRUE;
}

HE_BOOL CHE_PDF_File::Authenticate( const CHE_ByteString & password ) const
{
	if ( mpParser )
	{
		return mpParser->Authenticate( password );
	}
	return FALSE;
}

CHE_PDF_ObjectPtr CHE_PDF_File::GetObject( const HE_PDF_RefInfo & refInfo )
{
	CHE_PDF_ObjectPtr ObjPtr = mObjCollector.GetObj( refInfo );
	if ( ObjPtr )
	{
		return ObjPtr;
	}
	if ( mpParser == NULL )
	{
		return ObjPtr;
	}
	CHE_PDF_XREF_Entry entry;
	if ( mXRefTable.Get( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			ObjPtr = mpParser->GetObject();
			if ( ObjPtr )
			{
				mObjCollector.Add( refInfo, ObjPtr );
				return ObjPtr;
			}
		}else{
			HE_PDF_RefInfo stmRefInfo;
			stmRefInfo.objNum = entry.GetParentObjNum();
			stmRefInfo.genNum = 0;
			ObjPtr = GetObject( stmRefInfo );
			if ( ! ObjPtr )
			{
				return ObjPtr;
			}
			CHE_PDF_StreamPtr pStm = ObjPtr.GetStreamPtr();
			if ( ! pStm )
			{
				return pStm;
			}
			ObjPtr = mpParser->GetObjectInObjStm( pStm, refInfo, entry.GetIndex() );
			if ( ObjPtr )
			{
				mObjCollector.Add( refInfo, ObjPtr );
				return ObjPtr;
			}
		}
	}
	return ObjPtr;
}

HE_DWORD CHE_PDF_File::GetFileSize() const
{
	if ( mpParser )
	{
		return mpParser->GetFileSize();
	}
	return 0;
}

PDF_VERSION CHE_PDF_File::GetPDFVersion() const
{
	return mVersion;
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetTrailerDict() const
{
	return mXRefTable.GetTrailer();
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetRootDict()
{
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY ).GetDictPtr();
	}
	return dictPtr;
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetInfoDict()
{
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "Info", OBJ_TYPE_DICTIONARY ).GetDictPtr();
	}
	return dictPtr;
}

CHE_PDF_ArrayPtr CHE_PDF_File::GetIDArray()
{
	CHE_PDF_ArrayPtr arrayPtr;
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "ID", OBJ_TYPE_ARRAY ).GetArrayPtr();
	}
	return arrayPtr;
}

HE_VOID CHE_PDF_File::CreateTrailerDict()
{
	CHE_PDF_DictionaryPtr pTmpDict =  CHE_PDF_Dictionary::Create( GetAllocator() );
	mXRefTable.AddTrailerDict( pTmpDict );
}

HE_VOID CHE_PDF_File::CreateCatalogDict()
{
// 	CHE_PDF_DictionaryPtr pTmpDict = mXRefTable.GetTrailer();
// 	if ( !pTmpDict )
// 	{
// 		CreateTrailerDict();
// 		pTmpDict = mXRefTable.GetTrailer();
// 	}
// 	if ( pTmpDict )
// 	{
// 		CHE_PDF_IndirectObject * pCatalogDictInObj = CreateInObj_Dict();
// 		CHE_PDF_DictionaryPtr pCatalogDict = pCatalogDictInObj->GetObj().GetDictPtr();
// 		CHE_PDF_IndirectObject * pPagesDictInObj = CreateInObj_Dict();
// 		CHE_PDF_DictionaryPtr pPagesDict = pPagesDictInObj->GetObj().GetDictPtr();
// 		pPagesDict->SetAtName( "Type", "Pages" );
// 		CHE_PDF_ArrayPtr pArray = CHE_PDF_Array::Create( GetAllocator() );
// 		pPagesDict->SetAtArray( "Kids", pArray );
// 		pPagesDict->SetAtInteger( "Count", 0 );
// 		pCatalogDict->SetAtName( "Type", "Catalog" );
// 		pCatalogDict->SetAtReference( "Pages", pPagesDictInObj->GetObjNum(), pPagesDictInObj->GetGenNum(), this );
// 		pTmpDict->SetAtReference( "Root", pCatalogDictInObj->GetObjNum(), pCatalogDictInObj->GetGenNum(), this );
// 	}	 
}

HE_PDF_RefInfo CHE_PDF_File::CreateNullObject( CHE_PDF_NullPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_NullPtr pObj = CHE_PDF_Null::Create( GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateBooleanObject( CHE_PDF_BooleanPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_BooleanPtr pObj = CHE_PDF_Boolean::Create( false, GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateNumberObject( CHE_PDF_NumberPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_NumberPtr pObj = CHE_PDF_Number::Create( 0, GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateStringObject( CHE_PDF_StringPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_StringPtr pObj = CHE_PDF_String::Create( "", GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateNameObject( CHE_PDF_NamePtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_NamePtr pObj = CHE_PDF_Name::Create( "", GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateArrayObject( CHE_PDF_ArrayPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_ArrayPtr pObj = CHE_PDF_Array::Create( GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateDictObject( CHE_PDF_DictionaryPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	CHE_PDF_DictionaryPtr pObj = CHE_PDF_Dictionary::Create( GetAllocator() );
	if ( pObj )
	{
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}

HE_PDF_RefInfo CHE_PDF_File::CreateStreamObject( CHE_PDF_StreamPtr & ptrRet )
{
	CHE_PDF_XREF_Entry entry;
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	mXRefTable.AddNewEntry( entry );

	CHE_PDF_StreamPtr pObj = CHE_PDF_Stream::Create( entry.ObjNum, 0 );
	if ( pObj )
	{
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo, pObj );
	}
	return refInfo;
}