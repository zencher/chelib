#include "../../include/pdf/che_pdf_contentobject.h"
#include "../../include/pdf/che_pdf_gstate.h"

namespace chelib {

CPDF_ContentObject::~CPDF_ContentObject()
{
	if ( mpGState )
	{
		mpGState->GetAllocator()->Delete( mpGState );
	}
}

bool CPDF_ContentObject::SetGState( CPDF_GState * pGSatae )
{
	if ( mpGState )
	{
		mpGState->GetAllocator()->Delete( mpGState );
		mpGState = nullptr;
	}
	if ( pGSatae )
	{
		mpGState = pGSatae;
		return TRUE;
	}
	return false;
}



CPDF_ContentObject * CPDF_Text::Clone() const
{
	CPDF_Text * pTextRet = GetAllocator()->New<CPDF_Text>( GetAllocator() );
	pTextRet->mItems = mItems;
	//pTextRet->mExtMatrixl = mExtMatrixl;
	if ( mpGState )
	{
		pTextRet->mpGState = mpGState->Clone();
	}
	if ( mpObj )
	{
		pTextRet->mpObj = mpObj->Clone();
	}
	return pTextRet;
}

bool CPDF_Text::SetTextObject( const CPDF_ObjectPtr & pObj )
{
	if ( pObj )
	{
		mpObj = pObj;

		//获取字体
		CPDF_Font * pFont = GetGState()->GetTextFont();
		if ( pFont == nullptr )
		{
			return false;
		}

		std::vector<CPDF_ObjectPtr> tmpArray; 
		if ( pObj->GetType() == OBJ_TYPE_STRING )
		{
			tmpArray.push_back( pObj );
		}
		else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CPDF_ArrayPtr pArray = pObj->GetArrayPtr();
			CPDF_ObjectPtr pObj;
			for ( size_t i = 0; i < pArray->GetCount(); ++i )
			{
				pObj = pArray->GetElement( i );
				if ( pObj && ( pObj->GetType() == OBJ_TYPE_STRING || pObj->GetType() == OBJ_TYPE_NUMBER ) )
				{
					tmpArray.push_back( pObj );
				}
			}
		}
		CByteString tmpStr;
		int32 kerning = 0;
		CPDF_TextItem item;
		if ( pFont->GetFontType() != FONT_TYPE0 )
		{
			for ( size_t i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					for ( size_t index = 0; index < tmpStr.GetLength(); ++index )
					{
						item.charCode = BYTE( tmpStr[index] );
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
                        pFont->Decode( item.charCode, item.ucs, item.gid, item.cid );
						item.kerning = kerning;
						item.width = pFont->GetWidth( item );
						item.height = 1; //font height??
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
				}
			}
			if ( kerning != 0 )
			{
				mpLastKerning = kerning;
			}
		}
		else
		{
			CPDF_Type0_Font * pType0Font = (CPDF_Type0_Font *)( pFont );
			for ( size_t i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					item.charCode = 0;
					BYTE byteCount = 0;
					for ( size_t index = 0; index < tmpStr.GetLength(); ++index )
					{
						++byteCount;
						item.charCode = ( item.charCode << 8 ) + BYTE( tmpStr[index] );
						if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							if ( pType0Font->IsCode( item.charCode, byteCount ) == false )
							{
								continue;
							}
						}else if ( index % 2 == 0 )
						{
							continue;
						}
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
                        pFont->Decode( item.charCode, item.ucs, item.gid, item.cid );
						item.kerning = kerning;
						item.width = pFont->GetWidth( item );
						item.height = 1; //font height??
						mItems.push_back( item );
						kerning = 0;

						//very important!
						item.charCode = 0;
						byteCount = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
				}
			}
			if ( kerning != 0 )
			{
				mpLastKerning = kerning;
			}
		}
		return TRUE;
	}
	return false;
}

CMatrix CPDF_Text::GetTextMatrix() const
{
	CPDF_GState * pGState = GetGState();
	if ( pGState )
	{
		CMatrix textMatrix;
		CMatrix ctm;
		CMatrix tmpMatrix;
		FLOAT fontSize = 1;
		FLOAT fontScaling = 100;
		FLOAT fontRise = 0;
		pGState->GetTextMatrix( textMatrix );
		ctm = pGState->GetMatrix();
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextScaling( fontScaling );
		pGState->GetTextRise( fontRise );
		tmpMatrix.a = fontSize * fontScaling / 100;
		tmpMatrix.b = 0;
		tmpMatrix.c = 0;
		tmpMatrix.d = fontSize;
		tmpMatrix.e = 0;
		tmpMatrix.f = fontRise;
		tmpMatrix.Concat( textMatrix );
		tmpMatrix.Concat( ctm );
		return tmpMatrix;
	}
	return CMatrix();
}


CMatrix CPDF_Text::GetCharMatrix( size_t index ) const
{
	if ( index >= mItems.size() )
	{
		return CMatrix();
	}
	CPDF_GState * pGState = GetGState();
	if ( pGState )
	{
		CPDF_Font * pFont = pGState->GetTextFont();
		size_t wMode = 0;
		if ( pFont )
		{
			wMode = pFont->GetWMode();
		}

		FLOAT fontCharSpace = 0;
		FLOAT fontWordSpace = 0;
		FLOAT fontScaling = 100;
		FLOAT fontSize = 1;
		FLOAT fontRise = 0;
		
		pGState->GetTextCharSpace( fontCharSpace );
		pGState->GetTextWordSpace( fontWordSpace );
		pGState->GetTextScaling( fontScaling );
		fontScaling /= 100;
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextRise( fontRise );

		FLOAT offset = 0;
		FLOAT kerning = 0;
		size_t i = 0;
		for (; i < index; ++i )
		{
			offset += mItems[i].width * fontSize + fontCharSpace;	//wMode==1的时候，是不是还加width？
			if ( mItems[i].ucs == L' ' || mItems[i].charCode == ' ' )	//如果这里的ucs和charcode处理有误的话就会算错了！
			{
				offset += fontWordSpace;
			}
			kerning -= mItems[i].kerning * fontSize / 1000;
		}
		if ( i <= index )
		{
			kerning -= mItems[i].kerning * fontSize / 1000;
		}

		//CMatrix ctm = pGState->GetMatrix();
		CMatrix tm;
        pGState->GetTextMatrix( tm );

		CMatrix textMatrix;
		textMatrix.a = fontSize * fontScaling;
		textMatrix.b = 0;
		textMatrix.c = 0;
		textMatrix.d = fontSize;
		textMatrix.e = 0;
		textMatrix.f = fontRise;
		
		CMatrix OffsetMatrix;
		if ( wMode == 0 )
		{
			OffsetMatrix.e = offset * fontScaling + kerning;
		}else{
			OffsetMatrix.f = offset + kerning;
		}
		textMatrix.Concat( OffsetMatrix );
		textMatrix.Concat( tm );
		//textMatrix.Concat( ctm );
        
        if (pFont->GetFontType() == FONT_TYPE3) {
            CMatrix fontMatrix = ((CPDF_Type3_Font*)pFont)->GetFontMatrix();
            fontMatrix.Concat( textMatrix );
            textMatrix = fontMatrix;
        }
		return textMatrix;
	}
	return CMatrix();
}


CRect CPDF_Text::GetTextRect() const
{
	CRect rect;
	CRect tmpRect;
	for ( size_t i = 0; i < mItems.size(); ++i )
	{
		tmpRect = GetCharRect( i );
		rect.Union( tmpRect );
	}
	return rect;
}


CRect CPDF_Text::GetCharRect( size_t index ) const
{
	CRect rect;
	if (index < mItems.size())
	{
        CPDF_Font * pfont = nullptr;
		CPDF_GState * pGState = GetGState();
		if (pGState)
		{
            pfont = pGState->GetTextFont();
		}
		CMatrix matrix = GetCharMatrix( index );
		rect.width = mItems[index].width;
		rect.height = mItems[index].height;
		if ( pfont )
		{
            rect.bottom = pfont->GetFontDesender();//face->descender * 1.0f / face->units_per_EM;
            rect.height = pfont->GetFontAscender() - pfont->GetFontDesender();//( face->ascender - face->descender ) * 1.0f / face->units_per_EM;
		}
		rect = matrix.Transform( rect );
	}
	return rect;
}


//获得这个字符串对象中包括kerning和字符宽度，间隔等所有数据说产生的offset
//这个offset还没有被外部的tm、ctm矩阵进行运行。而在使用该offset的地方要进行这个运算。可以直接加在tm里面
FLOAT CPDF_Text::GetOffSet() const
{
	CPDF_GState * pGState = GetGState();
	if ( pGState )
	{
		FLOAT fontCharSpace = 0;
		FLOAT fontWordSpace = 0;
		FLOAT fontScaling = 100;
		FLOAT fontSize = 1;
		FLOAT fontRise = 0;
		pGState->GetTextCharSpace( fontCharSpace );
		pGState->GetTextWordSpace( fontWordSpace );
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextRise( fontRise );
		pGState->GetTextScaling( fontScaling );
		fontScaling /= 100;

		//这里之所以可以忽略fontSize，fontScaling，fontRise等信息的矩阵的原因是
		//这些信息所组成的矩阵在乘以offset的矩阵的时候不会影响offset的值（不受放大系数影响）

		FLOAT offset = 0;
		FLOAT kerning = 0;
		for ( size_t i = 0; i < mItems.size(); ++i )
		{
			offset += mItems[i].width * fontSize + fontCharSpace;
			if ( mItems[i].ucs == L' ' || mItems[i].charCode == ' ' )
			{
				offset += fontWordSpace;
			}
			kerning -= mItems[i].kerning * fontSize / 1000;
		}
		return offset * fontScaling + kerning - mpLastKerning * fontSize / 1000;
	}
	return 0;
}

CPDF_Path * CPDF_Text::GetGraphPath( size_t index )
{
    return nullptr;
}

CPDF_ContentObject * CPDF_Path::Clone() const
{
	CPDF_Path * pTmpPath = GetAllocator()->New<CPDF_Path>( GetAllocator() );
	pTmpPath->mType = mType;
	pTmpPath->mFillMode = mFillMode;
	pTmpPath->mItems = mItems;
	pTmpPath->mpGState = nullptr;
	if ( mpGState )
	{
		pTmpPath->mpGState = mpGState->Clone();
	}
	return pTmpPath;
}

CPDF_Path::~CPDF_Path()
{
	mItems.clear();
}

CPDF_InlineImage::CPDF_InlineImage( bool bMask, uint32 width, uint32 hight, uint32 bpc,
    PBYTE pBytes, size_t size, CPDF_ObjectPtr objPtr, CPDF_ColorSpacePtr colorspace,
    GRAPHICS_STATE_RENDERINTENTS ri, CAllocator * pAllocator/*= nullptr*/ )
	: CPDF_ContentObject( ContentType_InlineImage, pAllocator ), mbMask( bMask ), mWidth( width ), mHeight( hight ), 
	mBpc( bpc ), mpData( nullptr), mDataSize( 0 ), mDecodeObjPtr( objPtr ), mColorspace(colorspace), mRI(ri)
{
	if ( pBytes )
	{
		mDataSize = size;
		mpData = GetAllocator()->NewArray<BYTE>( mDataSize );
		memcpy( mpData, pBytes, mDataSize );
	}
}

CPDF_InlineImage::~CPDF_InlineImage()
{
	if ( mpData )
	{
		GetAllocator()->DeleteArray( mpData );
	}
}

CPDF_ContentObject * CPDF_InlineImage::Clone() const
{
	// 		CPDF_ColorSpace * pTmpColorSpace = nullptr;
	// 		if ( mpColorspace )
	// 		{
	// 			pTmpColorSpace = mpColorspace->Clone();
	// 		}
	CPDF_ObjectPtr objPtr;
	if ( mDecodeObjPtr )
	{
		objPtr = mDecodeObjPtr->Clone();
	}
	return GetAllocator()->New<CPDF_InlineImage>( mbMask, mWidth, mHeight, mBpc, mpData, mDataSize, objPtr, mColorspace, mRI, GetAllocator() );
}

#ifdef WIN32

CBitmap * CPDF_InlineImage::GetBitmap()
{
	CBitmap * pBitmapRet = nullptr;
	if ( !mColorspace )
	{
		return pBitmapRet;
	}
	PBYTE pData = mpData;
	PBYTE pTmpByte = nullptr;
	BYTE	tmpByte = 0;
	ARGB colorARGB = 0xFF000000;
	CPDF_Color color;
	ARGB * pColors = GetAllocator()->NewArray<ARGB>( mWidth );
	size_t colorsIndex = 0;
	
	size_t component = mColorspace->GetComponentCount();
	if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
	{
		component = 1;
	}
	size_t stride = (mWidth * component * mBpc + 7)/8;

	pBitmapRet = GetAllocator()->New<CBitmap>( GetAllocator() );
	pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );

	if ( mBpc == 8 )
	{
		color.Clear();
		for ( size_t y = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			for ( size_t x = 0 ; x < mWidth; ++x )
			{
				pTmpByte = pData + ( ( y * mWidth + x ) * component );
				for ( size_t i = 0; i < component; ++i )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( *( pTmpByte ) );
					}else{
						color.Push( *( pTmpByte ) / 255.0f );
					}
					pTmpByte++;
				}
				colorARGB = mColorspace->GetARGBValue( color );
				color.Clear();
				*(pColors+colorsIndex++) = colorARGB;
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 4 )
	{
		color.Clear();
		for ( size_t y = 0, x = 0; y < mHeight; ++y )
		{
			pTmpByte = pData + ( y * stride );
			colorsIndex = 0;
			x = 0;
			for ( size_t i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( size_t j = 0; j < 2; ++j )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( (tmpByte>>(1-j)*4)&0x0F );
					}else{
						color.Push( ((tmpByte>>(1-j)*4)&0x0F)/16.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 2 )
	{
		color.Clear();
		for ( size_t y = 0, x = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			pTmpByte = pData + ( y * stride );
			x = 0;
			for ( size_t i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( size_t j = 0; j < 4; ++j )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( tmpByte>>((3-j)*2) & 0x03 );
					}else{
						color.Push( ((tmpByte>>((3-j)*2)) & 0x03) /4.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 1 )
	{
		color.Clear();
		for ( size_t y = 0, x = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			pTmpByte = pData + ( y * stride );
			x = 0;
			for ( size_t i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( size_t j = 0; j < 8; ++j )
				{
					color.Push( ( tmpByte >> (7-j) ) & 0x01  );
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}
	return pBitmapRet;
}

#endif


CPDF_ContentObject * CPDF_ComponentRef::Clone() const
{
	CPDF_ComponentRef * pCmpRef = GetAllocator()->New<CPDF_ComponentRef>( mName, mComponentPtr, GetAllocator() );
	if ( mpGState )
	{
		pCmpRef->mpGState = mpGState->Clone();
	}
	return pCmpRef;
}

}//namespace