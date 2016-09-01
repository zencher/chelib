#include "../../include/pdf/che_pdf_file.h"
#include "../../include/pdf/che_pdf_creator.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mpParser(nullptr), mVersion(PDF_VERSION_1_7), mObjCollector(pAllocator), mXRefTable(pAllocator) {}

CHE_PDF_File::~CHE_PDF_File()
{
	Close();
}

bool CHE_PDF_File::Open( IHE_Read * pRead )
{
	if ( pRead == nullptr )
	{
		return false;
	}

	mLock.Lock();
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
		if ( mVersion == PDF_VERSION_UNKNOWN )
		{
			mLock.UnLock();
			return false;
		}
		mpParser->GetStartxref( 1024 );
		mpParser->ParseXRef();
		mLock.UnLock();
		return TRUE;
	}

	mLock.UnLock();
	return false;
}

void CHE_PDF_File::Close()
{
	mLock.Lock();
 	mObjCollector.Clear();
 	mXRefTable.Clear();
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mpParser = nullptr;
	}
	mLock.UnLock();
}

bool CHE_PDF_File::Save( IHE_Write * pWrite, bool bCompress )
{
	if ( pWrite == nullptr )
	{
		return false;
	}

	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create(pWrite, GetAllocator());
	if ( pCreator == nullptr )
	{
		return false;
	}
    
    pCreator->SetCompress( bCompress );

	if ( mpParser && mpParser->mpStrEncrypt )
	{
		pCreator->SetEncrypt(mpParser->mpStrEncrypt);
	}

	size_t				objCount = 0;
	size_t				offset = 0;
	PDF_RefInfo				refInfo;
	bool					bEncrypt = TRUE;

	CHE_PDF_ObjectPtr		ObjPtr;
	CHE_PDF_NamePtr			NamePtr;
	CHE_PDF_DictionaryPtr	DictPtr;

	CHE_PDF_XRefEntry		entry;
	CHE_PDF_XRefTable		xref( GetAllocator() );

	//PDF File Head
	pCreator->OutPutFileHead( GetPDFVersion() );

	//XRef Beginning
	entry.ObjNum = 0;
	entry.Field1 = 0;
	entry.Field2 = 65535;
	entry.Type = XREF_ENTRY_TYPE_FREE;
	++objCount;
	xref.Add( entry );

	mLock.Lock();

	//Out put all objects
	mXRefTable.MoveFirst();
	while( !mXRefTable.IsEOF() )
	{
		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			refInfo.objNum = entry.GetObjNum();
			refInfo.genNum = 0;

			if (	mpParser && mpParser->mEncryptRef &&
					mpParser->mEncryptRef->GetRefInfo().objNum == refInfo.objNum &&
					mpParser->mEncryptRef->GetRefInfo().genNum == refInfo.genNum )
			{
				bEncrypt = false;
			}else{
				bEncrypt = TRUE;
			}

			ObjPtr = GetObject( refInfo );
			if ( ObjPtr )
			{
				if ( ObjPtr->GetType() == OBJ_TYPE_STREAM )
				{
					DictPtr = ObjPtr->GetStreamPtr()->GetDictPtr();
					CHE_PDF_ObjectPtr tmpObjPtr = DictPtr->GetElement( "Type", OBJ_TYPE_NAME );
					if ( tmpObjPtr )
					{
						NamePtr = tmpObjPtr->GetNamePtr();
					}
					if ( NamePtr )
					{
						if ( ( NamePtr->GetString() == "ObjStm" ) || ( NamePtr->GetString() == "XRef" ) )
						{
							mXRefTable.MoveNext();
							NamePtr.Reset();
							continue;
						}
					}
				}

				offset = pCreator->OutPutInObject( refInfo, ObjPtr, bEncrypt );

				entry.Type = XREF_ENTRY_TYPE_COMMON;
				entry.ObjNum = refInfo.objNum;
				entry.Field1 = offset;
				entry.Field2 = 0;
				++objCount;

				xref.Add( entry );
			}
		}
		mXRefTable.MoveNext();
	}

	mLock.UnLock();

	offset = pCreator->OutPutXRefTable( xref );

	DictPtr = GetTrailerDict();
	if ( DictPtr )
	{
		CHE_PDF_DictionaryPtr newTriailerDict = CHE_PDF_Dictionary::Create(GetAllocator());

		ObjPtr = DictPtr->GetElement("Root", OBJ_TYPE_REFERENCE);
		if (ObjPtr)
		{
			newTriailerDict->SetObject("Root", ObjPtr);
		}
		ObjPtr = DictPtr->GetElement("Info", OBJ_TYPE_REFERENCE);
		if (ObjPtr)
		{
			newTriailerDict->SetObject("Info", ObjPtr->GetRefPtr());
		}

		if (mpParser && mpParser->mEncryptRef)
		{
			newTriailerDict->SetReference("Encrypt", mpParser->mEncryptRef->GetRefInfo().objNum, mpParser->mEncryptRef->GetRefInfo().genNum, this);
		}

		if (mpParser && mpParser->mIDArrayPtr)
		{
			newTriailerDict->SetArray("ID", mpParser->mIDArrayPtr->Clone());
		}

		newTriailerDict->SetInteger("Size", (int32)objCount);

		pCreator->OutPutTailerDict(newTriailerDict);

		pCreator->OutPutFileTailer(offset);
	}

	pCreator->GetAllocator()->Delete( pCreator );

	return TRUE;
}

bool CHE_PDF_File::SaveCompact( IHE_Write * pWrite )
{
	if ( pWrite == nullptr )
	{
		return false;
	}

// 	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
// 	if ( pCreator == nullptr )
// 	{
// 		return false;
// 	}
// 
// 	CHE_DynBuffer dynBuffer( 1024, 1024 * 4, GetAllocator() );
// 	IHE_Write * pBufWrite = HE_CreateDynBufferWrite( &dynBuffer, GetAllocator() );
// 	if ( pWrite == nullptr )
// 	{
// 		pCreator->GetAllocator()->Delete( pCreator );
// 		return false;
// 	}
// 
// 	size_t objCount = 0;
// 	size_t objInStmCount = 0;
// 	size_t offset = 0;
// 	size_t nextObjNum = mXRefTable.GetMaxObjNum()+1;
// 	CHE_PDF_XRefEntry entry;
// 	CHE_PDF_XRefTable xref( GetAllocator() );
// 	PDF_RefInfo refInfo;
// 	CHE_Queue< size_t > objNumQueue;
// 	CHE_Queue< size_t > objOffset;
// 
// 	CHE_PDF_ObjectPtr ObjPtr;
// 	CHE_PDF_NamePtr NamePtr;
// 	CHE_PDF_DictionaryPtr DictPtr;
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
// 	mLock.Lock();
// 
// 	mXRefTable.MoveFirst();
// 	while( !mXRefTable.IsEOF() )
// 	{
// 		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
// 		{
// 			refInfo.objNum = entry.GetObjNum();
// 			refInfo.genNum = 0;
// 			ObjPtr = GetObject( refInfo );
// 			if ( ObjPtr )
// 			{
// 				//跳过不需要输出的对象流对象和交叉索引流对象
// 				if ( IsPdfStreamPtr( ObjPtr ) )
// 				{
// 					DictPtr = ObjPtr->GetStreamPtr()->GetDictPtr();
// 					
// 					CHE_PDF_ObjectPtr tmpObjPtr = DictPtr->GetElement( "Type", OBJ_TYPE_NAME );
// 					if ( tmpObjPtr )
// 					{
// 						NamePtr = tmpObjPtr->GetNamePtr();
// 						if ( NamePtr )
// 						{
// 							if ( NamePtr->GetString() == "ObjStm" || NamePtr->GetString() == "XRef" )
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
// 	mLock.UnLock();
// 
// 	CHE_PDF_ReferencePtr RefPtr;
// 	CHE_PDF_DictionaryPtr trailerDictPtr = GetTrailerDict();
// 
// // 	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_REFERENCE );
// // 
// // 	if ( ObjPtr )
// // 	{
// // 		RefPtr = ObjPtr->GetRefPtr();
// // 		refInfo = RefPtr->GetRefInfo();
// // 
// // 		offset = pCreator->OutPutInObject( refInfo, RefPtr->GetRefObj() );
// // 
// // 		entry.Type = XREF_ENTRY_TYPE_COMMON;
// // 		entry.ObjNum = refInfo.objNum;
// // 		entry.Field1 = offset;
// // 		entry.Field2 = 0;
// // 		xref.Update( refInfo.objNum, entry );
// // 	}
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
// 				ObjPtr = GetObject( refInfo );
// 				if ( ! ObjPtr )
// 				{
// 					xref.MoveNext();
// 					continue;
// 				}
// 				if ( ObjPtr->GetType() == OBJ_TYPE_STREAM )
// 				{
// 					offset = pCreator->OutPutInObject( refInfo, ObjPtr );
// 					entry.Type = XREF_ENTRY_TYPE_COMMON;
// 					entry.ObjNum = refInfo.objNum;
// 					entry.Field1 = offset;
// 					entry.Field2 = 0;
// 					xref.Update( refInfo.objNum, entry );
// 				}else if ( entry.GetGenNum() == 0  )
// 				{
// 					offset = pBufWrite->GetCurOffset();
// 					CHE_PDF_Creator::OutPutObject( pBufWrite, refInfo, ObjPtr );
// 					pBufWrite->WriteByte( '\n' );
// 					objNumQueue.Push( refInfo.objNum );
// 					objOffset.Push( offset );
// 					++objInStmCount;
// 					
// 					if ( objInStmCount == 30 )
// 					{
// 						size_t objNum = 0;
// 						size_t offset = 0;
// 						size_t index = 0;
// 						CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
// 						char tmpStr[1024];
// 
// 						CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( nextObjNum++, 0, nullptr, GetAllocator() );
// 
// 						CHE_PDF_DictionaryPtr DictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
// 
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
// 							sprintf( tmpStr, "%ld %ld ", objNum, offset );
// 							tmpBuf.Write( (PCBYTE)(&(tmpStr[0])), strlen(tmpStr) );
// 
// 							++index;
// 						}
// 
// 						offset = tmpBuf.GetSize();
// 						DictPtr->SetAtName( "Type", "ObjStm" );
// 						DictPtr->SetAtInteger( "N", (int32)objInStmCount );
// 						DictPtr->SetAtInteger( "First", (int32)offset );
// 						StmPtr->SetDict( DictPtr );
// 
// 						tmpBuf.Write( dynBuffer );
// 						StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_NULL );
// 
// 						refInfo.objNum = nextObjNum - 1;
// 						refInfo.genNum = 0;
// 
// 						offset = pCreator->OutPutInObject( refInfo, StmPtr );
// 						entry.Type = XREF_ENTRY_TYPE_COMMON;
// 						entry.ObjNum = nextObjNum-1;
// 						entry.Field1 = offset;
// 						entry.Field2 = 0;
// 						xref.Add( entry );
// 
// 						dynBuffer.Clear();
// 						objInStmCount = 0;
// 						++objCount;
// 					}
// 				}else{
// 					offset = pCreator->OutPutInObject( refInfo, ObjPtr );
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
// 		size_t objNum = 0;
// 		size_t offset = 0;
// 		size_t index = 0;
// 		CHE_DynBuffer tmpBuf( 1024, 1024 * 8, GetAllocator() );
// 		char tmpStr[1024];
// 		CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( nextObjNum++, 0, nullptr, GetAllocator() );
// 		CHE_PDF_DictionaryPtr DictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
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
// 			sprintf( tmpStr, "%ld %ld ", objNum, offset );
// 			tmpBuf.Write( (PCBYTE)( &(tmpStr[0]) ), strlen(tmpStr) );
// 
// 			++index;
// 		}
// 
// 		offset = tmpBuf.GetSize();
// 		DictPtr->SetAtName( "Type", "ObjStm" );
// 		DictPtr->SetAtInteger( "N", (int32)objInStmCount );
// 		DictPtr->SetAtInteger( "First", (int32)offset );
// 		StmPtr->SetDict( DictPtr );
// 
// 		tmpBuf.Write( dynBuffer );
// 		StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );
// 
// 		refInfo.objNum = nextObjNum - 1;
// 		refInfo.genNum = 0;
// 		offset = pCreator->OutPutInObject( refInfo, StmPtr );
// 		entry.Type = XREF_ENTRY_TYPE_COMMON;
// 		entry.ObjNum = nextObjNum - 1;
// 		entry.Field1 = offset;
// 		entry.Field2 = 0;
// 		xref.Add( entry );
// 
// 		dynBuffer.Clear();
// 		objInStmCount = 0;
// 
// 		++objCount;
// 	}
// 
// 	CHE_PDF_DictionaryPtr StmDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
// 	CHE_PDF_StreamPtr StmPtr = CHE_PDF_Stream::Create( 0, 0, nullptr, GetAllocator() );
// 
// 	refInfo.objNum = xref.GetMaxObjNum() + 1;
// 	refInfo.genNum = 0;
// 	
// 	StmPtr->SetDict( StmDictPtr );
// 
// 	StmDictPtr->SetAtName( "Type", "XRef" );
// 
// 	ObjPtr = trailerDictPtr->GetElement( "Root", OBJ_TYPE_REFERENCE );
// 	if ( ObjPtr )
// 	{
// 		StmDictPtr->SetAtObj( "Root", ObjPtr->Clone() );
// 	}
// 	ObjPtr = trailerDictPtr->GetElement( "Info", OBJ_TYPE_REFERENCE );
// 	if ( ObjPtr )
// 	{
// 		StmDictPtr->SetAtObj( "Info", ObjPtr->Clone() );
// 	}
// 	StmDictPtr->SetAtInteger( "Size", (int32)objCount );
// 	CHE_PDF_ArrayPtr ArrayPtr = CHE_PDF_Array::Create( GetAllocator() );
// 	ArrayPtr->Append( CHE_PDF_Number::Create( (int32)1, GetAllocator() ) );
// 	ArrayPtr->Append( CHE_PDF_Number::Create( (int32)3, GetAllocator() ) );
// 	ArrayPtr->Append( CHE_PDF_Number::Create( (int32)1, GetAllocator() ) );
// 	StmDictPtr->SetAtArray( "W", ArrayPtr );
// 	ArrayPtr  = CHE_PDF_Array::Create( GetAllocator() );
// 	StmDictPtr->SetAtArray( "Index", ArrayPtr );
// 
// 	CHE_DynBuffer tmpBuf( 1024, 1024, GetAllocator() );
// 
// 	size_t lBeginNum = 0;
// 	size_t lNextObjNum = 0;
// 	size_t lCountNum = 0;
// 
// 	CHE_Queue< CHE_PDF_XRefEntry > entryList;
// 
// 	size_t field1 = 0, field2 = 0, field3 = 0;
// 
// 	xref.MoveFirst();
// 	if ( ! xref.GetCurNode( entry ) )
// 	{
// 		return false/*offset*/;
// 	}
// 	lBeginNum = entry.GetObjNum();
// 	lNextObjNum = lBeginNum + 1;
// 	lCountNum = 1;
// 	entryList.Push( entry );
// 
// 	xref.MoveNext();
// 	while( !xref.IsEOF() )
// 	{
// 		if ( xref.GetCurNode( entry ) )
// 		{
// 			if ( entry.GetObjNum() == lNextObjNum )
// 			{
// 				entryList.Push( entry );
// 				++lCountNum;
// 			}else{
// 				CHE_PDF_XRefEntry tmpEntry;
// 				ArrayPtr->Append( CHE_PDF_Number::Create( (int32)lBeginNum, GetAllocator() ) );
// 				ArrayPtr->Append( CHE_PDF_Number::Create( (int32)lCountNum, GetAllocator() ) );
// 				while( entryList.Pop( tmpEntry ) )
// 				{
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
//                     case XREF_ENTRY_TYPE_NEW:
//                         break;
// 					}
// 					BYTE byte = 0;
// 					tmpBuf.Write( PCBYTE( &field1 ), 1 );
// 					byte = (BYTE)( field2 >> 16 );
// 					tmpBuf.Write( PCBYTE( &byte ), 1 );
// 					byte = (BYTE)( field2 >> 8 );
// 					tmpBuf.Write( PCBYTE( &byte ), 1 );
// 					byte = (BYTE)( field2 );
// 					tmpBuf.Write( PCBYTE( &byte ), 1 );
// 					tmpBuf.Write( PCBYTE( &field3 ), 1 );
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
// 		CHE_PDF_XRefEntry tmpEntry;
// 		ArrayPtr->Append( CHE_PDF_Number::Create( (int32)lBeginNum, GetAllocator() ) );
// 		ArrayPtr->Append( CHE_PDF_Number::Create( (int32)lCountNum, GetAllocator() ) );
// 		while( entryList.Pop( tmpEntry ) )
// 		{
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
//             case XREF_ENTRY_TYPE_NEW:
//                 break;
// 			}
// 			BYTE byte = 0;
// 			tmpBuf.Write( PCBYTE( &field1 ), 1 );
// 			byte = (BYTE)( field2 >> 16 );
// 			tmpBuf.Write( PCBYTE( &byte ), 1 );
// 			byte = (BYTE)( field2 >> 8 );
// 			tmpBuf.Write( PCBYTE( &byte ), 1 );
// 			byte = (BYTE)( field2 );
// 			tmpBuf.Write( PCBYTE( &byte ), 1 );
// 			tmpBuf.Write( PCBYTE( &field3 ), 1 );
// 		}
// 	}
// 
// 	StmPtr->SetRawData( tmpBuf.GetData(), tmpBuf.GetSize(), STREAM_FILTER_FLATE );
// 
// 	refInfo.objNum = nextObjNum + 1;
// 	refInfo.genNum= 0;
// 
// 	offset = pCreator->OutPutInObject( refInfo, StmPtr );
// 
// 	pCreator->OutPutFileTailer( offset );
// 
// 	pCreator->GetAllocator()->Delete( pCreator );
 
 	return TRUE;
}

bool CHE_PDF_File::Authenticate( const CHE_ByteString & password ) const
{
	if ( mpParser )
	{
		return mpParser->Authenticate( password );
	}
	return false;
}

CHE_PDF_ObjectPtr CHE_PDF_File::GetObject( const PDF_RefInfo & refInfo )
{
	mLock.Lock();
	CHE_PDF_ObjectPtr ObjPtr = mObjCollector.GetObj( refInfo.objNum );
	if ( ObjPtr )
	{
		mLock.UnLock();
		return ObjPtr;
	}
	if ( mpParser == nullptr )
	{
		mLock.UnLock();
		return ObjPtr;
	}
	CHE_PDF_XRefEntry entry;
	if ( mXRefTable.Get( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			ObjPtr = mpParser->GetObject();
			if ( ObjPtr )
			{
				mObjCollector.Add( refInfo.objNum, ObjPtr );
				mLock.UnLock();
				return ObjPtr;
			}
		}else if ( entry.GetType() == XREF_ENTRY_TYPE_COMPRESSED )
		{
			PDF_RefInfo stmRefInfo;
			stmRefInfo.objNum = entry.GetParentObjNum();
			stmRefInfo.genNum = 0;
			ObjPtr = GetObject( stmRefInfo );
			if ( ! ObjPtr )
			{
				mLock.UnLock();
				return ObjPtr;
			}
			CHE_PDF_StreamPtr pStm = ObjPtr->GetStreamPtr();
			if ( ! pStm )
			{
				mLock.UnLock();
				return pStm;
			}
			//mLock.UnLock();
			mpParser->GetAllObjectsInObjStm( pStm, &mObjCollector );
			//mLock.Lock();
			ObjPtr = mObjCollector.GetObj( refInfo.objNum );
// 			CHE_PDF_StreamPtr pStm = ObjPtr->GetStreamPtr();
// 			if ( ! pStm )
// 			{
// 				mLock.UnLock();
// 				return pStm;
// 			}
// 			ObjPtr = mpParser->GetObjectInObjStm( pStm, refInfo, entry.GetIndex() );
// 			if ( ObjPtr )
// 			{
// 				mObjCollector.Add( refInfo, ObjPtr );
// 				mLock.UnLock();
// 				return ObjPtr;
// 			}
		}
	}
	mLock.UnLock();
	return ObjPtr;
}

size_t CHE_PDF_File::GetFileSize() const
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

void CHE_PDF_File::SetPDFVersion( PDF_VERSION version )
{
	mVersion = version;
}

bool CHE_PDF_File::SetInfo( PDF_DOCUMENT_INFO infoType, const CHE_ByteString & str )
{
	if ( str.GetLength() == 0 )
	{
		return false;
	}
	if ( infoType > 8 || infoType < 0 )
	{
		return false;
	}
	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_DictionaryPtr InfoDictPtr;

	ObjPtr = GetTrailerDict()->GetElement( "Info", OBJ_TYPE_DICTIONARY );
	if ( ObjPtr )
	{
		InfoDictPtr = ObjPtr->GetDictPtr();
	}else{
		PDF_RefInfo refInfo = CreateDictObject( InfoDictPtr );
		if ( InfoDictPtr )
		{
			GetTrailerDict()->SetReference( "Info", refInfo.objNum, refInfo.genNum, this );
		}
	}

	if ( ! InfoDictPtr )
	{
		return false;
	}

	switch( infoType )
	{
	case DOCUMENT_INFO_TITLE:
		InfoDictPtr->SetString( "Title", str );
		break;
	case DOCUMENT_INFO_AUTHOR:
		InfoDictPtr->SetString( "Author", str );
		break;
	case DOCUMENT_INFO_SUBJECT:
		InfoDictPtr->SetString( "Subject", str );
		break;
	case DOCUMENT_INFO_KEYWORDS:
		InfoDictPtr->SetString( "Keywords", str );
		break;
	case DOCUMENT_INFO_CREATOR:
		InfoDictPtr->SetString( "Creator", str );
		break;
	case DOCUMENT_INFO_PRODUCER:
		InfoDictPtr->SetString( "Producer", str );
		break;
	case DOCUMENT_INFO_CREATIONDATE:
		InfoDictPtr->SetString( "CreationDate", str );
		break;
	case DOCUMENT_INFO_MODDATE:
		InfoDictPtr->SetString( "ModDate", str );
		break;
	case DOCUMENT_INFO_TRAPPED:
		InfoDictPtr->SetString( "Trapped", str );
		break;
	default:
		return false;
	}
	return TRUE;
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
		return dictPtr->GetElement( "Root", OBJ_TYPE_DICTIONARY )->GetDictPtr();
	}
	return CHE_PDF_DictionaryPtr();
}

CHE_PDF_DictionaryPtr CHE_PDF_File::GetInfoDict()
{
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "Info", OBJ_TYPE_DICTIONARY )->GetDictPtr();
	}
	return CHE_PDF_DictionaryPtr();
}

CHE_PDF_ArrayPtr CHE_PDF_File::GetIDArray()
{
	CHE_PDF_ArrayPtr arrayPtr;
	CHE_PDF_DictionaryPtr dictPtr = mXRefTable.GetTrailer();
	if ( dictPtr )
	{
		return dictPtr->GetElement( "ID", OBJ_TYPE_ARRAY )->GetArrayPtr();
	}
	return arrayPtr;
}

void CHE_PDF_File::CreateTrailerDict()
{
	CHE_PDF_DictionaryPtr pTmpDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	mLock.Lock();
	mXRefTable.AddTrailerDict( pTmpDict );
	mLock.UnLock();
}

void CHE_PDF_File::CreateCatalogDict()
{
	CHE_PDF_DictionaryPtr trailerDict = mXRefTable.GetTrailer();
	if ( ! trailerDict )
	{
		CreateTrailerDict();
		trailerDict = mXRefTable.GetTrailer();
	}
	if ( trailerDict )
	{
		CHE_PDF_DictionaryPtr pagesDictPtr;
		PDF_RefInfo pagesDictRef = CreateDictObject( pagesDictPtr );
		if ( pagesDictPtr )
		{
			pagesDictPtr->SetName( "Type", "Pages" );
			CHE_PDF_ArrayPtr pArray = CHE_PDF_Array::Create( GetAllocator() );
			pagesDictPtr->SetArray( "Kids", pArray );
			pagesDictPtr->SetInteger( "Count", 0 );

			CHE_PDF_DictionaryPtr catalogDictPtr;
			PDF_RefInfo catalogDictRef = CreateDictObject( catalogDictPtr );
			if ( catalogDictPtr )
			{
				catalogDictPtr->SetName( "Type", "Catalog" );
				catalogDictPtr->SetReference( "Pages", pagesDictRef.objNum, pagesDictRef.genNum, this );
				trailerDict->SetReference( "Root", catalogDictRef.objNum, catalogDictRef.genNum, this );
			}
		}
	}	 
}

PDF_RefInfo CHE_PDF_File::CreateNullObject( CHE_PDF_NullPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Null::Create( GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateBooleanObject( CHE_PDF_BooleanPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Boolean::Create( false, GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateNumberObject( CHE_PDF_NumberPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Number::Create( 0, GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateStringObject( CHE_PDF_StringPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_String::Create( "", GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateNameObject( CHE_PDF_NamePtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Name::Create( "", GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateArrayObject( CHE_PDF_ArrayPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Array::Create( GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateDictObject( CHE_PDF_DictionaryPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	ptrRet = CHE_PDF_Dictionary::Create( GetAllocator() );
	if ( ptrRet )
	{
		mLock.Lock();
		mXRefTable.AddNewEntry( entry );
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
		mLock.UnLock();
	}
	return refInfo;
}

PDF_RefInfo CHE_PDF_File::CreateStreamObject( CHE_PDF_StreamPtr & ptrRet )
{
	CHE_PDF_XRefEntry entry;
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	mLock.Lock();

	mXRefTable.AddNewEntry( entry );

	ptrRet = CHE_PDF_Stream::Create( entry.ObjNum, 0 );
	if ( ptrRet )
	{
		refInfo.objNum = entry.GetObjNum();
		refInfo.genNum = entry.GetGenNum();
		mObjCollector.Add( refInfo.objNum, ptrRet );
	}

	mLock.UnLock();

	return refInfo;
}