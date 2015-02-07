#include "../../include/pdf/che_pdf_xobject.h"
#include "../../include/pdf/che_pdf_contentlistbuilder.h"

CHE_PDF_ImageXObjectPtr CHE_PDF_ImageXObject::Create( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
{
	CHE_PDF_ImageXObjectPtr ptr;
	if ( ! refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_ImageXObject>( refPtr, pAllocator ) );
	return ptr;
}

CHE_PDF_ImageXObjectPtr CHE_PDF_ImageXObject::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
	CHE_PDF_ImageXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_ImageXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}


CHE_PDF_ImageXObject::CHE_PDF_ImageXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component( COMPONENT_TYPE_ImageXObject, refPtr, pAllocator ), mWidth(0), mHeight(0), mBpc(0),
mbInterpolate(FALSE), mStmAcc(pAllocator), mbMask(FALSE), mRI(RI_AbsoluteColorimetric)
{
	if ( refPtr )
	{
		CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			mStmPtr = objPtr->GetStreamPtr();
			CHE_PDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
			if ( dictPtr )
			{
				objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mWidth = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mHeight = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mBpc = objPtr->GetNumberPtr()->GetInteger();
				}
				objPtr = dictPtr->GetElement( "ColorSpace" );
				if ( objPtr )
				{
                    if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
                    {
                        objPtr = objPtr->GetRefPtr()->GetRefObj();
                    }
					mColorspace = CHE_PDF_ColorSpace::Create( objPtr, GetAllocator() );
				}
				objPtr = dictPtr->GetElement( "Interpolate", OBJ_TYPE_BOOLEAN );
				if ( objPtr )
				{
					mbInterpolate = objPtr->GetBooleanPtr()->GetValue();
				}
				objPtr = dictPtr->GetElement( "ImageMask", OBJ_TYPE_BOOLEAN );
				if ( objPtr )
				{
					if ( objPtr->GetBooleanPtr()->GetValue() == TRUE )
					{
						mbMask = TRUE;
					}
				}
				objPtr = dictPtr->GetElement( "Decode", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
                    mDecodeArray = objPtr->GetArrayPtr();
				}
                objPtr = dictPtr->GetElement( "Intent", OBJ_TYPE_NAME );
                if ( objPtr )
                {
                    CHE_ByteString str = objPtr->GetNamePtr()->GetString();
                    if ( str == "AbsoluteColorimetric" )
                    {
                        mRI = RI_AbsoluteColorimetric;
                    }else if ( str == "RelativeColorimetric" )
                    {
                        mRI = RI_RelativeColorimetric;
                    }else if ( str == "Saturation" )
                    {
                        mRI = RI_Saturation;
                    }else{
                        mRI = RI_Perceptual;
                    }
                }
				if ( mbMask == FALSE )
				{
					objPtr = dictPtr->GetElement( "Mask" );
					if ( objPtr )
					{
                        if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
                        {
                            if ( objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM ) )
                            {
                                mMaskImagePtr = CHE_PDF_ImageXObject::Create( objPtr->GetRefPtr() );
                            }
                        }else if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
                        {
                            mColorKeyMaskPtr = objPtr->GetArrayPtr();
                        }
					}
                    else
                    {
						objPtr = dictPtr->GetElement( "SMask" );
                        if ( objPtr && objPtr->GetType() == OBJ_TYPE_REFERENCE )
                        {
                            mSoftMaskImagePtr = CHE_PDF_ImageXObject::Create( objPtr->GetRefPtr() );
                        }
					}
				}
			}
		}
	}
}


CHE_PDF_ImageXObject::~CHE_PDF_ImageXObject()
{
}

HE_LPBYTE CHE_PDF_ImageXObject::GetData()
{
    HE_LPBYTE lpByte = mStmAcc.GetData();
    if ( lpByte == NULL )
    {
        mStmAcc.Attach( mStmPtr );
        lpByte = mStmAcc.GetData();
    }
    return lpByte;
}

HE_ULONG CHE_PDF_ImageXObject::GetSize()
{
    HE_LPBYTE lpByte = mStmAcc.GetData();
    if ( lpByte == NULL )
    {
        mStmAcc.Attach( mStmPtr );
    }
    return mStmAcc.GetSize();
}

CHE_PDF_FormXObjectPtr CHE_PDF_FormXObject::Create( const CHE_PDF_ReferencePtr & refPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*=  NULL*/ )
{
	CHE_PDF_FormXObjectPtr ptr;
	if ( !refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) || pComponentMgr == NULL )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	CHE_PDF_FormXObject * pForm = pAllocator->New<CHE_PDF_FormXObject>( refPtr, pAllocator );
	if ( CHE_PDF_ContentListBuilder::ParseContentStream( refPtr, pForm->GetList(), pComponentMgr, pAllocator ) )
	{
		ptr.Reset( pForm );
	}else{
		pAllocator->Delete( pForm );
	}
	return ptr;
}

CHE_PDF_FormXObjectPtr CHE_PDF_FormXObject::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
	CHE_PDF_FormXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_FormXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}

CHE_PDF_FormXObject::CHE_PDF_FormXObject( const CHE_PDF_ReferencePtr & refPtr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component( COMPONENT_TYPE_FormXObject, refPtr, pAllocator ) {}