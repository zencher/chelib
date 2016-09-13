#include "../../include/pdf/pdf_colorspace.h"
#include "../../include/pdf/pdf_contentobject.h"

#include <cstdlib>

namespace chelib {

CPDF_ColorSpacePtr CPDF_ColorSpace::CreateDeviceGray(CAllocator * pAllocator)
{
    CPDF_ColorSpacePtr ptr;
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CPDF_ColorSpace>(COLORSPACE_DEVICE_GRAY, 1, pAllocator) );
    return  ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::CreateDeviceRGB(CAllocator * pAllocator)
{
    CPDF_ColorSpacePtr ptr;
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CPDF_ColorSpace>(COLORSPACE_DEVICE_RGB, 3, pAllocator) );
    return  ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::CreateDeviceCMYK(CAllocator * pAllocator)
{
    CPDF_ColorSpacePtr ptr;
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CPDF_ColorSpace>(COLORSPACE_DEVICE_CMYK, 4, pAllocator) );
    return  ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::CreatePattern(CAllocator * pAllocator)
{
    CPDF_ColorSpacePtr ptr;
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CPDF_CS_Pattern>(pAllocator) );
    return ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::Create(const CByteString & name, CAllocator * pAllocator/*= nullptr*/)
{
	CPDF_ColorSpacePtr ptr;
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
	if ( name == "DeviceGray" || name == "G" || name == "Gray" )
	{
		return CreateDeviceGray(pAllocator);
	}else if ( name == "DeviceRGB" || name == "RGB" || name == "CalRGB" )
	{
		return CreateDeviceRGB(pAllocator);
	}else if ( name == "DeviceCMYK" || name == "CMYK" || name == "CalCMYK" ) // CalCMYK define in pdf 1.1, not support now, using DeviceCMYK for Compatibility
	{
		return CreateDeviceCMYK(pAllocator);
	}else if ( name == "Pattern" )
	{
        return CreatePattern(pAllocator);
	}
	return ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::Create(const CPDF_ObjectPtr & obj, CAllocator * pAllocator/*= nullptr*/)
{
	CPDF_ColorSpacePtr ptr;
	if ( !obj )
	{
		return ptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
    
    CPDF_ObjectPtr objPtr = obj;
    if ( objPtr->GetType() == OBJ_TYPE_REFERENCE )
    {
        CPDF_ObjectPtr tmpObjPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_ARRAY );
        if ( !tmpObjPtr )
        {
            tmpObjPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_NAME );
        }
        objPtr = tmpObjPtr;
    }
    
    if ( objPtr->GetType() == OBJ_TYPE_NAME )
    {
        return CPDF_ColorSpace::Create( objPtr->GetNamePtr()->GetString() );
    }
    
    CPDF_ColorSpace *  pColorSpace = nullptr;
    if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
    {
        CPDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
        if ( arrayPtr->GetCount() >= 2 )
        {
            CPDF_ObjectPtr obj;
            obj = arrayPtr->GetElement(0, OBJ_TYPE_NAME);
            if ( obj )
            {
                CByteString name = obj->GetNamePtr()->GetString();
                if ( name == "CalGray" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CPDF_CS_CalGray>(obj->GetDictPtr(), pAllocator);
                    }
                }else if ( name == "CalRGB" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CPDF_CS_CalRGB>(obj->GetDictPtr(), pAllocator);
                    }
                }else if ( name == "Lab" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_DICTIONARY);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CPDF_CS_CalLab>(obj->GetDictPtr(), pAllocator);
                    }
                }else if (  name == "ICCBased" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_STREAM);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CPDF_CS_ICCBased>(obj->GetStreamPtr(), pAllocator);
                    }
                }else if ( name == "Indexed" )
                {
                    pColorSpace = pAllocator->New<CPDF_CS_Indexed>(arrayPtr, pAllocator);
                }else if ( name == "Separation" )
                {
                    pColorSpace = pAllocator->New<CPDF_CS_Separation>(arrayPtr, pAllocator);
                }else if ( name == "DeviceN" )
                {
                    pColorSpace = pAllocator->New<CPDF_CS_DeviceN>(arrayPtr, pAllocator);
                }else if ( name == "Pattern" )
                {
                    CPDF_ColorSpacePtr csptr = CreatePattern(pAllocator);
                    obj = arrayPtr->GetElement(1);
                    csptr->GetPatternPtr()->mUnderLyingColorspace = CPDF_ColorSpace::Create(obj, pAllocator);
                    return  csptr;
                    
                    //this is wrong, and wrong strange! But why?
                    //pColorSpace = pAllocator->New<CPDF_CS_Pattern>(pAllocator);
                    //pColorSpace->GetPatternPtr()->mUnderLyingColorspace = CPDF_ColorSpace::Create(obj, pAllocator);
                }
            }
        }else if( arrayPtr->GetCount() == 1)
        {
            CPDF_ObjectPtr obj;
            obj = arrayPtr->GetElement(0, OBJ_TYPE_NAME);
            if ( obj )
            {
                CByteString name = obj->GetNamePtr()->GetString();
                if ( name == "Pattern" )
                {
                    CPDF_ColorSpacePtr csptr = CreatePattern(pAllocator);
                    obj = arrayPtr->GetElement(1);
                    csptr->GetPatternPtr()->mUnderLyingColorspace = CPDF_ColorSpace::Create(obj, pAllocator);
                    return  csptr;
                }
            }
        }
    }
    if ( pColorSpace->IsError() )
    {
		pAllocator->Delete(pColorSpace);
		pColorSpace = nullptr;
	}else{
		ptr.Reset( pColorSpace );
	}
	return ptr;
}

CPDF_ColorSpacePtr CPDF_ColorSpace::Convert( const CPDF_ComponentPtr & componetPtr )
{
    CPDF_ColorSpacePtr ptr;
    if ( componetPtr && componetPtr->GetType() == COMPONENT_TYPE_ColorSpace )
    {
        ptr.Reset( componetPtr.GetPointer() );
    }
    return ptr;
}

CPDF_ColorSpace::CPDF_ColorSpace(PDF_COLORSPACE_TYPE type, uint32 componentCount, CAllocator * pAllocator/* = nullptr*/)
 : CPDF_Component(COMPONENT_TYPE_ColorSpace, pAllocator), mColorSpaceType(type), mComponentCount(componentCount) {}


CPDF_ColorSpace::~CPDF_ColorSpace() {}

// bool CPDF_ColorSpace::IsDeviceColorSpace() const
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
// 	return false;;
// }

// CByteString CPDF_ColorSpace::GetName() const
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

uint32 CPDF_ColorSpace::GetComponentCount() const
{
	return mComponentCount;
}

CPDF_CS_CalGrayPtr CPDF_ColorSpace::GetCalGrayPtr() const
{
    CPDF_CS_CalGrayPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALGRAY )
    {
        ptr.Reset( (CPDF_CS_CalGray*)this );
    }
    return ptr;
}

CPDF_CS_CalRGBPtr CPDF_ColorSpace::GetCalRGBPtr() const
{
    CPDF_CS_CalRGBPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALRGB )
    {
        ptr.Reset( (CPDF_CS_CalRGB*)this );
    }
    return ptr;
}

CPDF_CS_CalLabPtr CPDF_ColorSpace::GetCalLabPtr() const
{
    CPDF_CS_CalLabPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_CALLAB )
    {
        ptr.Reset( (CPDF_CS_CalLab*)this );
    }
    return ptr;
}

CPDF_CS_ICCBasedPtr CPDF_ColorSpace::GetICCBasedPtr() const
{
    CPDF_CS_ICCBasedPtr ptr;
    if ( mColorSpaceType == COLORSPACE_CIEBASE_ICCBASED )
    {
        ptr.Reset( (CPDF_CS_ICCBased*)this );
    }
    return ptr;
}

CPDF_CS_IndexedPtr CPDF_ColorSpace::GetIndexedPtr() const
{
    CPDF_CS_IndexedPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_INDEXED )
    {
        ptr.Reset( (CPDF_CS_Indexed*)this );
    }
    return ptr;
}

CPDF_CS_PatternPtr CPDF_ColorSpace::GetPatternPtr() const
{
    CPDF_CS_PatternPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_PATTERN )
    {
        ptr.Reset( (CPDF_CS_Pattern*)this );
    }
    return ptr;
}

CPDF_CS_SeparationPtr CPDF_ColorSpace::GetSeparationPtr() const
{
    CPDF_CS_SeparationPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_SEPARATION )
    {
        ptr.Reset( (CPDF_CS_Separation*)this );
    }
    return ptr;
}

CPDF_CS_DeviceNPtr CPDF_ColorSpace::GetDeviceNPtr() const
{
    CPDF_CS_DeviceNPtr ptr;
    if ( mColorSpaceType == COLORSPACE_SPECIAL_DEVICEN )
    {
        ptr.Reset( (CPDF_CS_DeviceN*)this );
    }
    return ptr;
}

CPDF_CS_CalGray::CPDF_CS_CalGray(CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_CALGRAY,1, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma = 1.0f;
}

CPDF_CS_CalGray::CPDF_CS_CalGray(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_CALGRAY,1, pAllocator)
{
    mWhitePoint[0] = 1.0f;
    mWhitePoint[1] = 1.0f;
    mWhitePoint[2] = 1.0f;
    mBlackPoint[0] = 0.0f;
    mBlackPoint[1] = 0.0f;
    mBlackPoint[2] = 0.0f;
    mGamma = 1.0f;
    
    size_t count;
    CPDF_ObjectPtr objPtr;
    CPDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for ( size_t i = 0; i < count && i < 3; ++i )
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
        for ( size_t i = 0; i < count && i < 3; ++i )
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

CPDF_CS_CalRGB::CPDF_CS_CalRGB(CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_CALRGB, 3, pAllocator)
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

CPDF_CS_CalRGB::CPDF_CS_CalRGB(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_CALRGB, 3, pAllocator)
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
    
    size_t count;
    CPDF_ObjectPtr objPtr;
    CPDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (size_t i = 0; i < count && i < 3; ++i)
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
        for (size_t i = 0; i < count && i < 3; ++i)
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
        for (size_t i = 0; i < count && i < 3; ++i)
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
        for (size_t i = 0; i < count && i < 9; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mMatrix[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
}

CPDF_CS_CalLab::CPDF_CS_CalLab(CAllocator * pAllocator)
: CPDF_ColorSpace(COLORSPACE_CIEBASE_CALLAB, 3, pAllocator)
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

CPDF_CS_CalLab::CPDF_CS_CalLab(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_CALLAB, 3, pAllocator)
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
    
    size_t count;
    CPDF_ObjectPtr objPtr;
    CPDF_ArrayPtr arrayPtr;
    objPtr = dict->GetElement("WhitePoint", OBJ_TYPE_ARRAY);
    if (objPtr)
    {
        arrayPtr = objPtr->GetArrayPtr();
        count = arrayPtr->GetCount();
        for (size_t i = 0; i < count && i < 3; ++i)
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
        for (size_t i = 0; i < count && i < 3; ++i)
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
        for (size_t i = 0; i < count && i < 4; ++i)
        {
            objPtr = arrayPtr->GetElement(i, OBJ_TYPE_NUMBER);
            if ( objPtr )
            {
                mRange[i] = objPtr->GetNumberPtr()->GetFloat();
            }
        }
    }
}

CPDF_CS_ICCBased::CPDF_CS_ICCBased(const CPDF_StreamPtr & stream, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_CIEBASE_ICCBASED, 0, pAllocator)
{
    size_t count;
    CPDF_ObjectPtr obj;
    CPDF_ArrayPtr array;
    CPDF_DictionaryPtr dict;
    
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
        for (size_t i = 0; i < count && i < 8; ++i)
        {
            obj = array->GetElement(i, OBJ_TYPE_NUMBER);
            if (obj)
            {
                mRange[i] = obj->GetNumberPtr()->GetFloat();
            }
        }
    }
    
    obj = dict->GetElement("Alternate");
    if (obj)
    {
        mAlternate = CPDF_ColorSpace::Create(obj);
    }else{
        if (mComponentCount == 4) {
            mAlternate = CPDF_ColorSpace::CreateDeviceCMYK();
        }else if (mComponentCount == 3)
        {
            mAlternate = CPDF_ColorSpace::CreateDeviceRGB();
        }else if (mComponentCount == 1)
        {
            mAlternate = CPDF_ColorSpace::CreateDeviceGray();
        }
    }
    mStmAcc.Attach(stream);
}

CPDF_CS_Indexed::CPDF_CS_Indexed(const CPDF_ArrayPtr & array, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_SPECIAL_INDEXED, 1, pAllocator), mIndexCount(0),
 mIndexTableSize(0), mpIndexTable(nullptr), mNewTableSize(0), mpNewTable(nullptr)
{
    if (array->GetCount() >= 4)
    {
        CPDF_ObjectPtr obj = array->GetElement(1);
        if (obj)
        {
            mBaseColorSpace = CPDF_ColorSpace::Create(obj, GetAllocator());
        }
        obj = array->GetElement(2, OBJ_TYPE_NUMBER);
        if (obj)
        {
            mIndexCount = obj->GetNumberPtr()->GetInteger();
        }
        obj = array->GetElement(3, OBJ_TYPE_STREAM);
        if (obj)
        {
            CPDF_StreamAcc stmAcc(GetAllocator());
            if ( stmAcc.Attach( obj->GetStreamPtr() ) )
            {
                mpIndexTable = GetAllocator()->NewArray<BYTE>(stmAcc.GetSize());
                memcpy(mpIndexTable, stmAcc.GetData(), stmAcc.GetSize());
                mIndexTableSize = stmAcc.GetSize();
                stmAcc.Detach();
            }
        }else{
            obj = array->GetElement( 3, OBJ_TYPE_STRING);
            if (obj)
            {
                CByteString str = obj->GetStringPtr()->GetString();
                mpIndexTable = GetAllocator()->NewArray<BYTE>(str.GetLength());
                memcpy(mpIndexTable, str.GetData(), str.GetLength());
                mIndexTableSize = str.GetLength();
            }
        }
    }
    if (!mBaseColorSpace)
    {
        abort();
    }
}

CPDF_CS_Pattern::CPDF_CS_Pattern(CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_SPECIAL_PATTERN, 0, pAllocator) {}



CPDF_CS_Separation::CPDF_CS_Separation(CPDF_ArrayPtr & array, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_SPECIAL_SEPARATION, 1, pAllocator)
{
    CPDF_ObjectPtr objPtr;
    if (array && array->GetCount() >= 4)
    {
        objPtr = array->GetElement(0, OBJ_TYPE_NAME);
        if (objPtr)
        {
            if (objPtr->GetNamePtr()->GetString() == "Separation")
            {
                objPtr = array->GetElement(2);
                if (objPtr)
                {
                    mBaseColorSpace = CPDF_ColorSpace::Create(objPtr, pAllocator);
                }
                objPtr = array->GetElement(3);
                if (objPtr)
                {
                    mFunction = CPDF_Function::Create(objPtr);
                }
            }
        }
    }
}


CPDF_CS_DeviceN::CPDF_CS_DeviceN(CPDF_ArrayPtr & array, CAllocator * pAllocator)
 : CPDF_ColorSpace(COLORSPACE_SPECIAL_DEVICEN, 1, pAllocator)
{
    CPDF_ObjectPtr objPtr;
    if (array && array->GetCount() >= 4)
    {
        objPtr = array->GetElement(0, OBJ_TYPE_NAME);
        if (objPtr)
        {
            if (objPtr->GetNamePtr()->GetString() == "DeviceN")
            {
                objPtr = array->GetElement(1, OBJ_TYPE_ARRAY);
                if (objPtr)
                {
                    mComponentCount = (uint32)objPtr->GetArrayPtr()->GetCount();
                }
                objPtr = array->GetElement(2);
                if (objPtr)
                {
                    mBaseColorSpace = CPDF_ColorSpace::Create(objPtr, pAllocator);
                }
                objPtr = array->GetElement(3);
                if (objPtr)
                {
                    mFunction = CPDF_Function::Create(objPtr);
                }
            }
        }
    }
}



CPDF_ColorSpace * CPDF_ColorSpacePtr::operator->() const
{
    return (CPDF_ColorSpace*)mpCom;
}


CPDF_CS_CalGray * CPDF_CS_CalGrayPtr::operator->() const
{
    return (CPDF_CS_CalGray*)mpCom;
}

CPDF_CS_CalRGB * CPDF_CS_CalRGBPtr::operator->() const
{
    return (CPDF_CS_CalRGB*)mpCom;
}

CPDF_CS_CalLab * CPDF_CS_CalLabPtr::operator->() const
{
    return (CPDF_CS_CalLab*)mpCom;
}

CPDF_CS_ICCBased * CPDF_CS_ICCBasedPtr::operator->() const
{
    return (CPDF_CS_ICCBased*)mpCom;
}

CPDF_CS_Pattern * CPDF_CS_PatternPtr::operator->() const
{
    return (CPDF_CS_Pattern*)mpCom;
}

CPDF_CS_Indexed * CPDF_CS_IndexedPtr::operator->() const
{
    return (CPDF_CS_Indexed*)mpCom;
}

CPDF_CS_Separation * CPDF_CS_SeparationPtr::operator->() const
{
    return (CPDF_CS_Separation*)mpCom;
}

CPDF_CS_DeviceN * CPDF_CS_DeviceNPtr::operator->() const
{
    return (CPDF_CS_DeviceN*)mpCom;
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

ARGB	CPDF_ColorSpace::GetARGBValue(const CPDF_Color & color) const
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
			ARGB valRet = 0xFF000000;
			BYTE tmpByte = 0x00;
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
			uint32 valRet = 0xFF000000;
			BYTE tmpByte1 = 0x00;
			BYTE tmpByte2 = 0x00;
			BYTE tmpByte3 = 0x00;
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
				FLOAT c = color.GetComponent(0), m = color.GetComponent(1), y = color.GetComponent(2), k = color.GetComponent(3);
				FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
				FLOAT r, g, b, x;
				FLOAT rgb[3];
				BYTE br = 0, bg = 0, bb = 0;
				ARGB valRet = 0;

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

ARGB	CPDF_CS_CalLab::GetARGBValue(const CPDF_Color & color) const
{
	ARGB valRet = 0xFF000000;
	if (color.GetComponentCount() >= 3)
	{
		FLOAT lstar, astar, bstar, l, m, n, x, y, z, r, g, b;

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


		BYTE tmpByte1 = sqrtf(fz_clamp(r, 0, 1)) * 255;
		BYTE tmpByte2 = sqrtf(fz_clamp(g, 0, 1)) * 255;
		BYTE tmpByte3 = sqrtf(fz_clamp(b, 0, 1)) * 255;
		valRet = valRet + (tmpByte1 << 16) + (tmpByte2 << 8) + tmpByte3;
	}
	return valRet;
}

ARGB	CPDF_CS_ICCBased::GetARGBValue(const CPDF_Color & color) const
{
	switch ( mComponentCount )
	{
	case 1:
		{
			uint32 valRet = 0xFF000000;
			BYTE tmpByte = 0x00;
			if (color.GetComponentCount() >= 1)
			{
				tmpByte = color.GetComponent(0) * 255;
				valRet = 0xFF000000 + (tmpByte << 16) + (tmpByte << 8) + tmpByte;
			}
			return valRet;
		}
	case 3:
		{
			uint32 valRet = 0xFF000000;
			BYTE tmpByte1 = 0x00;
			BYTE tmpByte2 = 0x00;
			BYTE tmpByte3 = 0x00;
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
				FLOAT c = color.GetComponent(0), m = color.GetComponent(1), y = color.GetComponent(2), k = color.GetComponent(3);
				FLOAT c1 = 1 - c, m1 = 1 - m, y1 = 1 - y, k1 = 1 - k;
				FLOAT r, g, b, x;
				FLOAT rgb[3];
				BYTE br = 0, bg = 0, bb = 0;
				ARGB valRet = 0;

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

ARGB	CPDF_CS_Indexed::GetARGBValue(const CPDF_Color & color) const
{
	if (mBaseColorSpace)
	{
		BYTE index = (BYTE)(color.GetComponent(0));
		size_t componentCount = mBaseColorSpace->GetComponentCount();
		if (mpIndexTable && (index * componentCount < mIndexTableSize))
		{
			BYTE component = 0;
			CPDF_Color newColor;
			PBYTE p = mpIndexTable + (index * componentCount);
			for (size_t i = 0; i < componentCount; ++i)
			{
				component = *(p + i);
				newColor.Push(component / 255.0f);
			}
			return mBaseColorSpace->GetARGBValue(newColor);
		}
	}
	return 0xFF000000;
}

ARGB	CPDF_CS_Separation::GetARGBValue(const CPDF_Color & color) const
{
	if (mBaseColorSpace && mFunction)
	{
		CPDF_Color newColor;
		std::vector<FLOAT> input;
		std::vector<FLOAT> output;
		for (size_t i = 0; i < color.GetComponentCount(); ++i)
		{
			input.push_back(color.GetComponent(i));
		}
		if (mFunction->Calculate(input, output))
		{
			for (size_t j = 0; j < output.size(); ++j)
			{
				newColor.Push(output[j]);
			}
			return mBaseColorSpace->GetARGBValue(newColor);
		}
	}
	return 0xFF000000;
}

ARGB	CPDF_CS_DeviceN::GetARGBValue(const CPDF_Color & color) const
{
	return 0xFF000000;
}
    
#endif

}//namespace