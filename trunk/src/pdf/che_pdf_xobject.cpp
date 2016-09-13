#include "../../include/pdf/che_pdf_xobject.h"
#include "../../include/pdf/che_pdf_contentlistbuilder.h"

namespace chelib {

CPDF_ImageXObjectPtr CPDF_ImageXObject::Create( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator/*= nullptr*/ )
{
	CPDF_ImageXObjectPtr ptr;
	if ( ! refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) )
	{
		return ptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_ImageXObject>( refPtr, pAllocator ) );
	return ptr;
}

CPDF_ImageXObjectPtr CPDF_ImageXObject::Convert( const CPDF_ComponentPtr & componentPtr )
{
	CPDF_ImageXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_ImageXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}


CPDF_ImageXObject::CPDF_ImageXObject( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator/*= nullptr*/ )
	: CPDF_Component( COMPONENT_TYPE_ImageXObject, refPtr, pAllocator ), mWidth(0), mHeight(0), mBpc(0),
mbInterpolate(false), mStmAcc(pAllocator), mbMask(false), mRI(RI_AbsoluteColorimetric)
{
	if ( refPtr )
	{
		CPDF_ObjectPtr objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			mStmPtr = objPtr->GetStreamPtr();
			CPDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
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
					mColorspace = CPDF_ColorSpace::Create( objPtr, GetAllocator() );
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
                    CByteString str = objPtr->GetNamePtr()->GetString();
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
				if ( mbMask == false )
				{
					objPtr = dictPtr->GetElement( "Mask" );
					if ( objPtr )
					{
                        if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
                        {
                            if ( objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM ) )
                            {
                                mMaskImagePtr = CPDF_ImageXObject::Create( objPtr->GetRefPtr() );
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
                            mSoftMaskImagePtr = CPDF_ImageXObject::Create( objPtr->GetRefPtr() );
                        }
					}
				}
			}
		}
	}
}


CPDF_ImageXObject::~CPDF_ImageXObject()
{
}

PBYTE CPDF_ImageXObject::GetData()
{
    PBYTE PBYTE = mStmAcc.GetData();
    if ( PBYTE == nullptr )
    {
        mStmAcc.Attach( mStmPtr );
        PBYTE = mStmAcc.GetData();
    }
    return PBYTE;
}

size_t CPDF_ImageXObject::GetSize()
{
    PBYTE PBYTE = mStmAcc.GetData();
    if ( PBYTE == nullptr )
    {
        mStmAcc.Attach( mStmPtr );
    }
    return mStmAcc.GetSize();
}

CPDF_FormXObjectPtr CPDF_FormXObject::Create( const CPDF_ReferencePtr & refPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*=  nullptr*/ )
{
	CPDF_FormXObjectPtr ptr;
	if ( !refPtr || ! refPtr->GetRefObj( OBJ_TYPE_STREAM ) || pComponentMgr == nullptr )
	{
		return ptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	CPDF_FormXObject * pForm = pAllocator->New<CPDF_FormXObject>( refPtr, pAllocator );
	if ( CPDF_ContentListBuilder::ParseContentStream( refPtr, pForm->GetList(), pComponentMgr, pAllocator ) )
	{
		ptr.Reset( pForm );
	}else{
		pAllocator->Delete( pForm );
	}
	return ptr;
}

CPDF_FormXObjectPtr CPDF_FormXObject::Convert( const CPDF_ComponentPtr & componentPtr )
{
	CPDF_FormXObjectPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_FormXObject )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}

CPDF_FormXObject::CPDF_FormXObject( const CPDF_ReferencePtr & refPtr, CAllocator * pAllocator/*= nullptr*/ )
	: CPDF_Component( COMPONENT_TYPE_FormXObject, refPtr, pAllocator ), mIsGroup(false)
{
    CPDF_ObjectPtr objPtr = refPtr->GetRefObj(OBJ_TYPE_STREAM);
    if (objPtr)
    {
        CPDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
        CPDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
        CPDF_ObjectPtr objPtr = dictPtr->GetElement("Matrix", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
            arrayPtr->GetMatrix(mMatrix);
        }
        
        objPtr = dictPtr->GetElement("Group", OBJ_TYPE_DICTIONARY);
        if ( objPtr )
        {
            mIsGroup = TRUE;
        }
    }
}

}//namespace