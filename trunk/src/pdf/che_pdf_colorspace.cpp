        #include "../../include/pdf/che_pdf_colorspace.h"
#include "../../include/pdf/che_pdf_contentobjs.h"

CHE_PDF_ColorSpacePtr gDeviceGrayCSPtr;
CHE_PDF_ColorSpacePtr gDeviceRGBCSPtr;
CHE_PDF_ColorSpacePtr gDeviceCMYKCSPtr;
CHE_PDF_ColorSpacePtr gPatternCSPtr;
CHE_PDF_ColorSpacePtr gDefCalGrayCSPtr;
CHE_PDF_ColorSpacePtr gDefCalRGBCSPtr;
CHE_PDF_ColorSpacePtr gDefCalLabCSPtr;

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceGray()
{
	if ( !gDeviceGrayCSPtr )
	{
		gDeviceGrayCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_GRAY, 1) );
	}
	return gDeviceGrayCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceRGB()
{
	if ( !gDeviceRGBCSPtr )
	{
		gDeviceRGBCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_RGB, 3) );
	}
	return gDeviceRGBCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceCMYK()
{
	if ( !gDeviceCMYKCSPtr )
	{
		gDeviceCMYKCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_CMYK, 4) );
	}
	return gDeviceCMYKCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreatePattern()
{
	if ( !gPatternCSPtr )
	{
		gPatternCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_CS_Pattern>() );
	}
	return gPatternCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateCalGray()
{
    if ( !gDefCalGrayCSPtr )
    {
        gDefCalGrayCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_CS_CalGray>() );
    }
    return gDefCalGrayCSPtr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateCalRGB()
{
    if ( !gDefCalRGBCSPtr )
    {
        gDefCalRGBCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_CS_CalRGB>() );
    }
    return gDefCalRGBCSPtr;

}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateCalLab()
{
    if ( !gDefCalLabCSPtr )
    {
        gDefCalLabCSPtr.Reset( GetDefaultAllocator()->New<CHE_PDF_CS_CalLab>() );
    }
    return gDefCalLabCSPtr;

}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create(const CHE_ByteString & name, CHE_Allocator * pAllocator/*= NULL*/)
{
	CHE_PDF_ColorSpacePtr ptr;
	if ( name == "DeviceGray" || name == "G" || name == "Gray" )
	{
		return CreateDeviceGray();
	}else if ( name == "DeviceRGB" || name == "RGB" || name == "CalRGB" )
	{
		return CreateDeviceRGB();
	}else if ( name == "DeviceCMYK" || name == "CMYK" || name == "CalCMYK" ) // CalCMYK define in pdf 1.1, not support now, using DeviceCMYK for Compatibility
	{
		return CreateDeviceCMYK();
	}else if ( name == "Pattern" )
	{
		return CreatePattern();
	}
    /*else if ( name == "CalGray" )
    {
        return CreateCalGray();
    }else if ( name == "CalRGB" )
    {
        return CreateCalRGB();
    }else if ( name == "CalLab" )
    {
        return CreateCalLab();
    }*/
	return ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create(const CHE_PDF_ObjectPtr & obj, CHE_Allocator * pAllocator/*= NULL*/)
{
	CHE_PDF_ColorSpacePtr ptr;
	if ( !obj )
	{
		return ptr;
	}
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
    
    CHE_PDF_ObjectPtr objPtr = obj;
    if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
    {
        CHE_PDF_ObjectPtr tmpObjPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_ARRAY );
        if ( !tmpObjPtr )
        {
            tmpObjPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_NAME );
        }
        objPtr = tmpObjPtr;
    }
    
    if ( objPtr->GetType() == OBJ_TYPE_NAME )
    {
        return CHE_PDF_ColorSpace::Create( objPtr->GetNamePtr()->GetString() );
    }
    
    CHE_PDF_ColorSpace *  pColorSpace = NULL;
    
    if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
    {
        CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        if ( arrayPtr->GetCount() >= 2 )
        {
            CHE_PDF_ObjectPtr obj;
            obj = arrayPtr->GetElement(0, OBJ_TYPE_NAME);
            if ( obj )
            {
                CHE_ByteString name = obj->GetNamePtr()->GetString();
                if ( name == "CalGray" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CHE_PDF_CS_CalGray>(obj->GetDictPtr(), pAllocator);
                    }
                }else if ( name == "CalRGB" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CHE_PDF_CS_CalRGB>(obj->GetDictPtr(), pAllocator);
                    }
                }else if ( name == "Lab" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CHE_PDF_CS_CalLab>(obj->GetDictPtr(), pAllocator);
                    }
                }else if ( name == "ICCBased" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_STREAM);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CHE_PDF_CS_ICCBased>(obj->GetStreamPtr(), pAllocator);
                    }
                }else if ( name == "Indexed" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_Indexed>( arrayPtr, pAllocator);
                }else if ( name == "Separation" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_Separation>(pAllocator);
                }else if ( name == "DeviceN" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_DeviceN>(pAllocator);
                }else if ( name == "Pattern" )
                {
                    //pattern underlying colorspace
                    obj = arrayPtr->GetElement(1);
                    return CHE_PDF_ColorSpace::Create(obj, pAllocator);
                }
            }
        }
    }
	if ( pColorSpace->IsError() )
	{
		pAllocator->Delete(pColorSpace);
		pColorSpace = NULL;
	}else{
		ptr.Reset( pColorSpace );
	}
	return ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Convert( const CHE_PDF_ComponentPtr & componetPtr )
{
    CHE_PDF_ColorSpacePtr ptr;
    if ( componetPtr && componetPtr->GetType() == COMPONENT_TYPE_ColorSpace )
    {
        ptr.Reset( componetPtr.GetPointer() );
    }
    return ptr;
}

CHE_PDF_ColorSpace::CHE_PDF_ColorSpace(PDF_COLORSPACE_TYPE type, HE_UINT32 componentCount, CHE_Allocator * pAllocator/* = NULL*/)
 : CHE_PDF_Component(COMPONENT_TYPE_ColorSpace, pAllocator), mColorSpaceType(type), mComponentCount(componentCount) {}


CHE_PDF_ColorSpace::~CHE_PDF_ColorSpace() {}

// HE_BOOL CHE_PDF_ColorSpace::IsDeviceColorSpace() const
// {
// 	switch ( GetType() )
// 	{
// 	case COLORSPACE_DEVICE_GRAY:
// 	case COLORSPACE_DEVICE_RGB:
// 	case COLORSPACE_DEVICE_CMYK:
// 		return TRUE;
// 	default:
// 		break;
// 	}
// 
// 	return FALSE;;
// }

// CHE_ByteString CHE_PDF_ColorSpace::GetName() const
// {
// 	switch ( mType )
// 	{
// 	case COLORSPACE_DEVICE_GRAY:
// 		return "DeviceGray";
// 	case COLORSPACE_DEVICE_RGB:
// 		return "DeviceRGB";
// 	case COLORSPACE_DEVICE_CMYK:
// 		return "DeviceCMYK";
// 	case COLORSPACE_CIEBASE_CALGRAY:
// 		return "CalGray";
// 	case COLORSPACE_CIEBASE_CALRGB:
// 		return "CalRGB";
// 	case COLORSPACE_CIEBASE_CALCMYK:
// 		return "CalCMYK";
// 	case COLORSPACE_CIEBASE_CALLAB:
// 		return "Lab";
// 	case COLORSPACE_CIEBASE_ICCBASED:
// 		return "ICCBased";
// 	case COLORSPACE_SPECIAL_PATTERN:
// 		return "Pattern";
// 	case COLORSPACE_SPECIAL_INDEXED:
// 		return "Indexed";
// 	case COLORSPACE_SPECIAL_SEPARATION:
// 		return "Separation";
// 	case COLORSPACE_SPECIAL_DEVICEN:
// 		return "DeviceN";
// 	default:
// 		break;
// 	}
// 	return "";
// }

HE_UINT32 CHE_PDF_ColorSpace::GetComponentCount() const
{
	return mComponentCount;
}

CHE_PDF_CS_CalGrayPtr CHE_PDF_ColorSpace::GetCalGrayPtr() const
{
    CHE_PDF_CS_CalGrayPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALGRAY )
    {
        ptr.Reset( (CHE_PDF_CS_CalGray*)this );
    }
    return ptr;
}

CHE_PDF_CS_CalRGBPtr CHE_PDF_ColorSpace::GetCalRGBPtr() const
{
    CHE_PDF_CS_CalRGBPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALRGB )
    {
        ptr.Reset( (CHE_PDF_CS_CalRGB*)this );
    }
    return ptr;
}

CHE_PDF_CS_CalLabPtr CHE_PDF_ColorSpace::GetCalLabPtr() const
{
    CHE_PDF_CS_CalLabPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALLAB )
    {
        ptr.Reset( (CHE_PDF_CS_CalLab*)this );
    }
    return ptr;
}

CHE_PDF_CS_ICCBasedPtr CHE_PDF_ColorSpace::GetICCBasedPtr() const
{
    CHE_PDF_CS_ICCBasedPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_ICCBASED )
    {
        ptr.Reset( (CHE_PDF_CS_ICCBased*)this );
    }
    return ptr;
}

CHE_PDF_CS_IndexedPtr CHE_PDF_ColorSpace::GetIndexedPtr() const
{
    CHE_PDF_CS_IndexedPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_INDEXED )
    {
        ptr.Reset( (CHE_PDF_CS_Indexed*)this );
    }
    return ptr;
}

CHE_PDF_CS_PatternPtr CHE_PDF_ColorSpace::GetPatternPtr() const
{
    CHE_PDF_CS_PatternPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_PATTERN )
    {
        ptr.Reset( (CHE_PDF_CS_Pattern*)this );
    }
    return ptr;
}

CHE_PDF_CS_SeparationPtr CHE_PDF_ColorSpace::GetSeparationPtr() const
{
    CHE_PDF_CS_SeparationPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_SEPARATION )
    {
        ptr.Reset( (CHE_PDF_CS_Separation*)this );
    }
    return ptr;
}

CHE_PDF_CS_DeviceNPtr CHE_PDF_ColorSpace::GetDeviceNPtr() const
{
    CHE_PDF_CS_DeviceNPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_DEVICEN )
    {
        ptr.Reset( (CHE_PDF_CS_DeviceN*)this );
    }
    return ptr;
}

CHE_PDF_CS_CalGray::CHE_PDF_CS_CalGray(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALGRAY,1, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma = 1.0f;
}

CHE_PDF_CS_CalGray::CHE_PDF_CS_CalGray(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALGRAY,1, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma = 1.0f;
    
    HE_ULONG count;
    CHE_PDF_ObjectPtr objPtr;
    CHE_PDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for ( HE_ULONG i = 0; i < count && i < 3; ++i )
        {
            objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
            if ( objPtr )
            {
                mWhitePoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("BlackPoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for ( HE_ULONG i = 0; i < count && i < 3; ++i )
        {
            objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
            if ( objPtr )
            {
                mBlackPoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("Gamma", OBJ_TYPE_NUMBER);
    if (objPtr)
    {
        mGamma = objPtr->GetNumberPtr()->GetFloat();
    }
}

CHE_PDF_CS_CalRGB::CHE_PDF_CS_CalRGB(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALRGB, 3, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma[0] = 1.0f;
    mGamma[1] = 1.0f;
    mGamma[2] = 1.0f;
    mMatrix[0] = 1.0f;
    mMatrix[1] = 0.0f;
    mMatrix[2] = 0.0f;
    mMatrix[3] = 0.0f;
    mMatrix[4] = 1.0f;
    mMatrix[5] = 0.0f;
    mMatrix[6] = 0.0f;
    mMatrix[7] = 0.0f;
    mMatrix[8] = 1.0f;
}

CHE_PDF_CS_CalRGB::CHE_PDF_CS_CalRGB(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALRGB, 3, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma[0] = 1.0f;
    mGamma[1] = 1.0f;
    mGamma[2] = 1.0f;
    mMatrix[0] = 1.0f;
    mMatrix[1] = 0.0f;
    mMatrix[2] = 0.0f;
    mMatrix[3] = 0.0f;
    mMatrix[4] = 1.0f;
    mMatrix[5] = 0.0f;
    mMatrix[6] = 0.0f;
    mMatrix[7] = 0.0f;
    mMatrix[8] = 1.0f;
    
    HE_ULONG count;
    CHE_PDF_ObjectPtr objPtr;
    CHE_PDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 3; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mWhitePoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("BlackPoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 3; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mBlackPoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("Gamma", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 3; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mGamma[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("Matrix", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 9; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mMatrix[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
}

CHE_PDF_CS_CalLab::CHE_PDF_CS_CalLab(CHE_Allocator * pAllocator)
: CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALLAB, 3, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mRange[0] = -100;
    mRange[1] = 100;
    mRange[2] = -100;
    mRange[3] = 100;
}

CHE_PDF_CS_CalLab::CHE_PDF_CS_CalLab(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_CALLAB, 3, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mRange[0] = -100;
    mRange[1] = 100;
    mRange[2] = -100;
    mRange[3] = 100;
    
    HE_ULONG count;
    CHE_PDF_ObjectPtr objPtr;
    CHE_PDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 3; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mWhitePoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("BlackPoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 3; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mBlackPoint[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    objPtr = dict->GetElement("Range", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (HE_ULONG i = 0; i < count && i < 4; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mRange[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
}

CHE_PDF_CS_ICCBased::CHE_PDF_CS_ICCBased(const CHE_PDF_StreamPtr & stream, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_CIEBASE_ICCBASED, 0, pAllocator)
{
    HE_ULONG count;
    CHE_PDF_ObjectPtr obj;
    CHE_PDF_ArrayPtr array;
    CHE_PDF_DictionaryPtr dict;
    
    obj = stream->GetDictPtr();
    if (obj) {
        dict = obj->GetDictPtr();
    }
    
    obj = dict->GetElement("N", OBJ_TYPE_NUMBER);
    if (obj) {
        mComponentCount = obj->GetNumberPtr()->GetInteger();
    }
    
    obj = dict->GetElement("Range", OBJ_TYPE_ARRAY);
    if (obj)
    {
        array = obj->GetArrayPtr();
        count = array->GetCount();
        for (HE_ULONG i = 0; i < count && i < 8; ++i)
        {
            obj = array->GetElement(i, OBJ_TYPE_NUMBER);
            if (obj)
            {
                mRange[i] = obj->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    obj = dict->GetElement("Alternate");
    mAlternate = CHE_PDF_ColorSpace::Create(obj);
    
    mStmAcc.Attach(stream);
}

CHE_PDF_CS_Indexed::CHE_PDF_CS_Indexed(const CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_INDEXED, 1, pAllocator), mIndexCount(0), mIndexTableSize(0), mpIndexTable(NULL)
{
    if (array->GetCount() >= 4)
    {
        CHE_PDF_ObjectPtr obj = array->GetElement(1, OBJ_TYPE_NAME);
        if (obj)
        {
            mBaseColorSpace = CHE_PDF_ColorSpace::Create(obj, GetAllocator());
        }
        obj = array->GetElement(2, OBJ_TYPE_NUMBER);
        if (obj)
        {
            mIndexCount = obj->GetNumberPtr()->GetInteger();
        }
        obj = array->GetElement(3, OBJ_TYPE_STREAM);
        if (obj)
        {
            CHE_PDF_StreamAcc stmAcc(GetAllocator());
            if ( stmAcc.Attach( obj->GetStreamPtr() ) )
            {
                mpIndexTable = GetAllocator()->NewArray<HE_BYTE>(stmAcc.GetSize());
                memcpy(mpIndexTable, stmAcc.GetData(), stmAcc.GetSize());
                mIndexTableSize = stmAcc.GetSize();
                stmAcc.Detach();
            }
        }else{
            obj = array->GetElement( 3, OBJ_TYPE_STRING);
            if (obj)
            {
                CHE_ByteString str = obj->GetStringPtr()->GetString();
                mpIndexTable = GetAllocator()->NewArray<HE_BYTE>(str.GetLength());
                memcpy(mpIndexTable, str.GetData(), str.GetLength());
                mIndexTableSize = str.GetLength();
            }
        }
    }
}

CHE_PDF_CS_Pattern::CHE_PDF_CS_Pattern(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_PATTERN, 0, pAllocator) {}



CHE_PDF_CS_Separation::CHE_PDF_CS_Separation(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_SEPARATION, 1, pAllocator) {}


CHE_PDF_CS_DeviceN::CHE_PDF_CS_DeviceN(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_DEVICEN, 1, pAllocator) {}



CHE_PDF_ColorSpace * CHE_PDF_ColorSpacePtr::operator->() const
{
    return (CHE_PDF_ColorSpace*)mpCom;
}


CHE_PDF_CS_CalGray * CHE_PDF_CS_CalGrayPtr::operator->() const
{
    return (CHE_PDF_CS_CalGray*)mpCom;
}

CHE_PDF_CS_CalRGB * CHE_PDF_CS_CalRGBPtr::operator->() const
{
    return (CHE_PDF_CS_CalRGB*)mpCom;
}

CHE_PDF_CS_CalLab * CHE_PDF_CS_CalLabPtr::operator->() const
{
    return (CHE_PDF_CS_CalLab*)mpCom;
}

CHE_PDF_CS_ICCBased * CHE_PDF_CS_ICCBasedPtr::operator->() const
{
    return (CHE_PDF_CS_ICCBased*)mpCom;
}

CHE_PDF_CS_Pattern * CHE_PDF_CS_PatternPtr::operator->() const
{
    return (CHE_PDF_CS_Pattern*)mpCom;
}

CHE_PDF_CS_Indexed * CHE_PDF_CS_IndexedPtr::operator->() const
{
    return (CHE_PDF_CS_Indexed*)mpCom;
}

CHE_PDF_CS_Separation * CHE_PDF_CS_SeparationPtr::operator->() const
{
    return (CHE_PDF_CS_Separation*)mpCom;
}

CHE_PDF_CS_DeviceN * CHE_PDF_CS_DeviceNPtr::operator->() const
{
    return (CHE_PDF_CS_DeviceN*)mpCom;
}




#ifdef WIN32

static inline float fung(float x)
{
	if (x >= 6.0f / 29.0f)
		return x * x * x;
	return (108.0f / 841.0f) * (x - (4.0f / 29.0f));
}

static inline float fz_clamp(float f, float min, float max)
{
	return (f > min ? (f < max ? f : max) : min);
}

HE_ARGB	CHE_PDF_ColorSpace::GetARGBValue(const CHE_PDF_Color & color) const
{
	if (color.GetComponentCount() == 0)
	{
		return 0xFF00000;
	}
	switch (mColorSpaceType)
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			HE_ARGB valRet = 0xFF000000;
			HE_BYTE tmpByte = 0x00;
			if (color.GetComponentCount() >= 1)
			{
				tmpByte = color.GetComponent(0) * 255;
				valRet = 0xFF000000 + (tmpByte << 16) + (tmpByte << 8) + tmpByte;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_BYTE tmpByte1 = 0x00;
			HE_BYTE tmpByte2 = 0x00;
			HE_BYTE tmpByte3 = 0x00;
			if (color.GetComponentCount() >= 3)
			{
				tmpByte1 = color.GetComponent(0) * 255;
				tmpByte2 = color.GetComponent(1) * 255;
				tmpByte3 = color.GetComponent(2) * 255;
				valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
			}
			return valRet;
		}
	case COLORSPACE_DEVICE_CMYK:
		{
			if (color.GetComponentCount() >= 4)
			{
				HE_FLOAT c = color.GetComponent(0), m = color.GetComponent(1), y = color.GetComponent(2), k = color.GetComponent(3);
				HE_FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
				HE_FLOAT r, g, b, x;
				HE_FLOAT rgb[3];
				HE_BYTE br = 0, bg = 0, bb = 0;
				HE_ARGB valRet = 0;

				/* this is a matrix multiplication, unrolled for performance */
				x = c1 * m1 * y1 * k1;	/* 0 0 0 0 */
				r = g = b = x;
				x = c1 * m1 * y1 * k;	/* 0 0 0 1 */
				r += 0.1373 * x;
				g += 0.1216 * x;
				b += 0.1255 * x;
				x = c1 * m1 * y * k1;	/* 0 0 1 0 */
				r += x;
				g += 0.9490 * x;
				x = c1 * m1 * y * k;	/* 0 0 1 1 */
				r += 0.1098 * x;
				g += 0.1020 * x;
				x = c1 * m * y1 * k1;	/* 0 1 0 0 */
				r += 0.9255 * x;
				b += 0.5490 * x;
				x = c1 * m * y1 * k;	/* 0 1 0 1 */
				r += 0.1412 * x;
				x = c1 * m * y * k1;	/* 0 1 1 0 */
				r += 0.9294 * x;
				g += 0.1098 * x;
				b += 0.1412 * x;
				x = c1 * m * y * k;	/* 0 1 1 1 */
				r += 0.1333 * x;
				x = c * m1 * y1 * k1;	/* 1 0 0 0 */
				g += 0.6784 * x;
				b += 0.9373 * x;
				x = c * m1 * y1 * k;	/* 1 0 0 1 */
				g += 0.0588 * x;
				b += 0.1412 * x;
				x = c * m1 * y * k1;	/* 1 0 1 0 */
				g += 0.6510 * x;
				b += 0.3137 * x;
				x = c * m1 * y * k;	/* 1 0 1 1 */
				g += 0.0745 * x;
				x = c * m * y1 * k1;	/* 1 1 0 0 */
				r += 0.1804 * x;
				g += 0.1922 * x;
				b += 0.5725 * x;
				x = c * m * y1 * k;	/* 1 1 0 1 */
				b += 0.0078 * x;
				x = c * m * y * k1;	/* 1 1 1 0 */
				r += 0.2118 * x;
				g += 0.2119 * x;
				b += 0.2235 * x;

				rgb[0] = fz_clamp(r, 0, 1);
				rgb[1] = fz_clamp(g, 0, 1);
				rgb[2] = fz_clamp(b, 0, 1);

				br = rgb[0] * 255;
				bg = rgb[1] * 255;
				bb = rgb[2] * 255;

				valRet = 0xFF000000 + (br << 16) + (bg << 8) + (bb);
				return valRet;
			}
		}
	default:break;
	}

	return 0xFF00000;
}

HE_ARGB	CHE_PDF_CS_CalLab::GetARGBValue(const CHE_PDF_Color & color) const
{
	HE_ARGB valRet = 0xFF000000;
	if (color.GetComponentCount() >= 3)
	{
		HE_FLOAT lstar, astar, bstar, l, m, n, x, y, z, r, g, b;

		lstar = color.GetComponent(0);
		astar = color.GetComponent(1);
		bstar = color.GetComponent(2);

		lstar *= 300;
		astar *= (mRange[1] - mRange[0]) + fabs(mRange[0]);
		bstar *= (mRange[3] - mRange[2]) + fabs(mRange[2]);

		m = (lstar + 16) / 116;
		l = m + astar / 500;
		n = m - bstar / 200;
		x = fung(l);
		y = fung(m);
		z = fung(n);
		r = (3.240449f * x + -1.537136f * y + -0.498531f * z) * 0.830026f;
		g = (-0.969265f * x + 1.876011f * y + 0.041556f * z) * 1.05452f;
		b = (0.055643f * x + -0.204026f * y + 1.057229f * z) * 1.1003f;


		HE_BYTE tmpByte1 = sqrtf(fz_clamp(r, 0, 1)) * 255;
		HE_BYTE tmpByte2 = sqrtf(fz_clamp(g, 0, 1)) * 255;
		HE_BYTE tmpByte3 = sqrtf(fz_clamp(b, 0, 1)) * 255;
		valRet = valRet + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
	}
	return valRet;
}

HE_ARGB	CHE_PDF_CS_ICCBased::GetARGBValue(const CHE_PDF_Color & color) const
{
	switch ( mComponentCount )
	{
	case 1:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_BYTE tmpByte = 0x00;
			if (color.GetComponentCount() >= 1)
			{
				tmpByte = color.GetComponent(0) * 255;
				valRet = 0xFF000000 + (tmpByte << 16) + (tmpByte << 8) + tmpByte;
			}
			return valRet;
		}
	case 3:
		{
			HE_UINT32 valRet = 0xFF000000;
			HE_BYTE tmpByte1 = 0x00;
			HE_BYTE tmpByte2 = 0x00;
			HE_BYTE tmpByte3 = 0x00;
			if (color.GetComponentCount() >= 3)
			{
				tmpByte1 = color.GetComponent(0) * 255;
				tmpByte2 = color.GetComponent(1) * 255;
				tmpByte3 = color.GetComponent(2) * 255;
				valRet = 0xFF000000 + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
			}
			return valRet;
		}
	case 4:
		{
			if (color.GetComponentCount() >= 4)
			{
				HE_FLOAT c = color.GetComponent(0), m = color.GetComponent(1), y = color.GetComponent(2), k = color.GetComponent(3);
				HE_FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
				HE_FLOAT r, g, b, x;
				HE_FLOAT rgb[3];
				HE_BYTE br = 0, bg = 0, bb = 0;
				HE_ARGB valRet = 0;

				/* this is a matrix multiplication, unrolled for performance */
				x = c1 * m1 * y1 * k1;	/* 0 0 0 0 */
				r = g = b = x;
				x = c1 * m1 * y1 * k;	/* 0 0 0 1 */
				r += 0.1373 * x;
				g += 0.1216 * x;
				b += 0.1255 * x;
				x = c1 * m1 * y * k1;	/* 0 0 1 0 */
				r += x;
				g += 0.9490 * x;
				x = c1 * m1 * y * k;	/* 0 0 1 1 */
				r += 0.1098 * x;
				g += 0.1020 * x;
				x = c1 * m * y1 * k1;	/* 0 1 0 0 */
				r += 0.9255 * x;
				b += 0.5490 * x;
				x = c1 * m * y1 * k;	/* 0 1 0 1 */
				r += 0.1412 * x;
				x = c1 * m * y * k1;	/* 0 1 1 0 */
				r += 0.9294 * x;
				g += 0.1098 * x;
				b += 0.1412 * x;
				x = c1 * m * y * k;	/* 0 1 1 1 */
				r += 0.1333 * x;
				x = c * m1 * y1 * k1;	/* 1 0 0 0 */
				g += 0.6784 * x;
				b += 0.9373 * x;
				x = c * m1 * y1 * k;	/* 1 0 0 1 */
				g += 0.0588 * x;
				b += 0.1412 * x;
				x = c * m1 * y * k1;	/* 1 0 1 0 */
				g += 0.6510 * x;
				b += 0.3137 * x;
				x = c * m1 * y * k;	/* 1 0 1 1 */
				g += 0.0745 * x;
				x = c * m * y1 * k1;	/* 1 1 0 0 */
				r += 0.1804 * x;
				g += 0.1922 * x;
				b += 0.5725 * x;
				x = c * m * y1 * k;	/* 1 1 0 1 */
				b += 0.0078 * x;
				x = c * m * y * k1;	/* 1 1 1 0 */
				r += 0.2118 * x;
				g += 0.2119 * x;
				b += 0.2235 * x;

				rgb[0] = fz_clamp(r, 0, 1);
				rgb[1] = fz_clamp(g, 0, 1);
				rgb[2] = fz_clamp(b, 0, 1);

				br = rgb[0] * 255;
				bg = rgb[1] * 255;
				bb = rgb[2] * 255;

				valRet = 0xFF000000 + (br << 16) + (bg << 8) + (bb);
				return valRet;
			}
		}
		break;
	default:
		break;
	}
	return 0xFF000000;
}

HE_ARGB	CHE_PDF_CS_Indexed::GetARGBValue(const CHE_PDF_Color & color) const
{
	if (mBaseColorSpace)
	{
		HE_BYTE index = (HE_BYTE)(color.GetComponent(0));
		HE_ULONG componentCount = mBaseColorSpace->GetComponentCount();
		if (mpIndexTable && (index * componentCount < mIndexTableSize))
		{
			HE_BYTE component = 0;
			CHE_PDF_Color newColor;
			HE_LPBYTE p = mpIndexTable + (index * componentCount);
			for (HE_ULONG i = 0; i < componentCount; ++i)
			{
				component = *(p + i);
				newColor.Push(component / 255.0f);
			}
			return mBaseColorSpace->GetARGBValue(newColor);
		}
	}
	return 0xFF000000;
}

HE_ARGB	CHE_PDF_CS_Separation::GetARGBValue(const CHE_PDF_Color & color) const
{
	if (mBaseColorSpace && mFunction)
	{
		CHE_PDF_Color newColor;
		std::vector<HE_FLOAT> input;
		std::vector<HE_FLOAT> output;
		for (HE_ULONG i = 0; i < color.GetComponentCount(); ++i)
		{
			input.push_back(color.GetComponent(i));
		}
		if (mFunction->Calculate(input, output))
		{
			for (HE_ULONG j = 0; j < output.size(); ++j)
			{
				newColor.Push(output[j]);
			}
			return mBaseColorSpace->GetARGBValue(newColor);
		}
	}
	return 0xFF000000;
}

HE_ARGB	CHE_PDF_CS_DeviceN::GetARGBValue(const CHE_PDF_Color & color) const
{
	return 0xFF000000;
}

#endif