        #include "../../include/pdf/che_pdf_colorspace.h"
#include "../../include/pdf/che_pdf_contentobjs.h"

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceGray(CHE_Allocator * pAllocator)
{
    CHE_PDF_ColorSpacePtr ptr;
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_GRAY, 1, pAllocator) );
    return  ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceRGB(CHE_Allocator * pAllocator)
{
    CHE_PDF_ColorSpacePtr ptr;
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_RGB, 3, pAllocator) );
    return  ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreateDeviceCMYK(CHE_Allocator * pAllocator)
{
    CHE_PDF_ColorSpacePtr ptr;
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CHE_PDF_ColorSpace>(COLORSPACE_DEVICE_CMYK, 4, pAllocator) );
    return  ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::CreatePattern(CHE_Allocator * pAllocator)
{
    CHE_PDF_ColorSpacePtr ptr;
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    ptr.Reset( pAllocator->New<CHE_PDF_CS_Pattern>(pAllocator) );
    return ptr;
}

CHE_PDF_ColorSpacePtr CHE_PDF_ColorSpace::Create(const CHE_ByteString & name, CHE_Allocator * pAllocator/*= NULL*/)
{
	CHE_PDF_ColorSpacePtr ptr;
    if ( pAllocator == NULL )
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
                }else if (  name == "ICCBased" )
                {
                    obj = arrayPtr->GetElement(1, OBJ_TYPE_STREAM);
                    if ( obj )
                    {
                        pColorSpace = pAllocator->New<CHE_PDF_CS_ICCBased>(obj->GetStreamPtr(), pAllocator);
                    }
                }else if ( name == "Indexed" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_Indexed>(arrayPtr, pAllocator);
                }else if ( name == "Separation" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_Separation>(arrayPtr, pAllocator);
                }else if ( name == "DeviceN" )
                {
                    pColorSpace = pAllocator->New<CHE_PDF_CS_DeviceN>(arrayPtr, pAllocator);
                }else if ( name == "Pattern" )
                {
                    CHE_PDF_ColorSpacePtr csptr = CreatePattern(pAllocator);
                    obj = arrayPtr->GetElement(1);
                    csptr->GetPatternPtr()->mUnderLyingColorspace = CHE_PDF_ColorSpace::Create(obj, pAllocator);
                    return  csptr;
                    
                    //this is wrong, and wrong strange! But why?
                    //pColorSpace = pAllocator->New<CHE_PDF_CS_Pattern>(pAllocator);
                    //pColorSpace->GetPatternPtr()->mUnderLyingColorspace = CHE_PDF_ColorSpace::Create(obj, pAllocator);
                }
            }
        }else if( arrayPtr->GetCount() == 1)
        {
            CHE_PDF_ObjectPtr obj;
            obj = arrayPtr->GetElement(0, OBJ_TYPE_NAME);
            if ( obj )
            {
                CHE_ByteString name = obj->GetNamePtr()->GetString();
                if ( name == "Pattern" )
                {
                    CHE_PDF_ColorSpacePtr csptr = CreatePattern(pAllocator);
                    obj = arrayPtr->GetElement(1);
                    csptr->GetPatternPtr()->mUnderLyingColorspace = CHE_PDF_ColorSpace::Create(obj, pAllocator);
                    return  csptr;
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

CHE_PDF_Color CHE_PDF_ColorSpace::GetRGB(const CHE_PDF_Color & color)
{
    CHE_PDF_Color ret;
    return ret;
}


class CFX_Vector_3by1 {
public:
    CFX_Vector_3by1() : a(0.0f), b(0.0f), c(0.0f) {}
    
    CFX_Vector_3by1(HE_FLOAT a1, HE_FLOAT b1, HE_FLOAT c1)
    : a(a1), b(b1), c(c1) {}
    
    HE_FLOAT a;
    HE_FLOAT b;
    HE_FLOAT c;
};
class CFX_Matrix_3by3 {
public:
    CFX_Matrix_3by3()
    : a(0.0f),
    b(0.0f),
    c(0.0f),
    d(0.0f),
    e(0.0f),
    f(0.0f),
    g(0.0f),
    h(0.0f),
    i(0.0f) {}
    
    CFX_Matrix_3by3(HE_FLOAT a1,
                    HE_FLOAT b1,
                    HE_FLOAT c1,
                    HE_FLOAT d1,
                    HE_FLOAT e1,
                    HE_FLOAT f1,
                    HE_FLOAT g1,
                    HE_FLOAT h1,
                    HE_FLOAT i1)
    : a(a1), b(b1), c(c1), d(d1), e(e1), f(f1), g(g1), h(h1), i(i1) {}
    
    CFX_Matrix_3by3 Inverse();
    
    CFX_Matrix_3by3 Multiply(const CFX_Matrix_3by3& m);
    
    CFX_Vector_3by1 TransformVector(const CFX_Vector_3by1& v);
    
    HE_FLOAT a;
    HE_FLOAT b;
    HE_FLOAT c;
    HE_FLOAT d;
    HE_FLOAT e;
    HE_FLOAT f;
    HE_FLOAT g;
    HE_FLOAT h;
    HE_FLOAT i;
};

CFX_Matrix_3by3 CFX_Matrix_3by3::Inverse() {
    HE_FLOAT det =
    a * (e * i - f * h) - b * (i * d - f * g) + c * (d * h - e * g);
    if (fabs(det) < 0.0000001)
        return CFX_Matrix_3by3();
    
    return CFX_Matrix_3by3(
                           (e * i - f * h) / det, -(b * i - c * h) / det, (b * f - c * e) / det,
                           -(d * i - f * g) / det, (a * i - c * g) / det, -(a * f - c * d) / det,
                           (d * h - e * g) / det, -(a * h - b * g) / det, (a * e - b * d) / det);
}

CFX_Matrix_3by3 CFX_Matrix_3by3::Multiply(const CFX_Matrix_3by3& m) {
    return CFX_Matrix_3by3(
                           a * m.a + b * m.d + c * m.g, a * m.b + b * m.e + c * m.h,
                           a * m.c + b * m.f + c * m.i, d * m.a + e * m.d + f * m.g,
                           d * m.b + e * m.e + f * m.h, d * m.c + e * m.f + f * m.i,
                           g * m.a + h * m.d + i * m.g, g * m.b + h * m.e + i * m.h,
                           g * m.c + h * m.f + i * m.i);
}

CFX_Vector_3by1 CFX_Matrix_3by3::TransformVector(const CFX_Vector_3by1& v) {
    return CFX_Vector_3by1(a * v.a + b * v.b + c * v.c,
                           d * v.a + e * v.b + f * v.c,
                           g * v.a + h * v.b + i * v.c);
}

const uint8_t g_sRGBSamples1[] = {
    0,   3,   6,   10,  13,  15,  18,  20,  22,  23,  25,  27,  28,  30,  31,
    32,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  49,  50,  51,  52,  53,  53,  54,  55,  56,  56,  57,  58,  58,
    59,  60,  61,  61,  62,  62,  63,  64,  64,  65,  66,  66,  67,  67,  68,
    68,  69,  70,  70,  71,  71,  72,  72,  73,  73,  74,  74,  75,  76,  76,
    77,  77,  78,  78,  79,  79,  79,  80,  80,  81,  81,  82,  82,  83,  83,
    84,  84,  85,  85,  85,  86,  86,  87,  87,  88,  88,  88,  89,  89,  90,
    90,  91,  91,  91,  92,  92,  93,  93,  93,  94,  94,  95,  95,  95,  96,
    96,  97,  97,  97,  98,  98,  98,  99,  99,  99,  100, 100, 101, 101, 101,
    102, 102, 102, 103, 103, 103, 104, 104, 104, 105, 105, 106, 106, 106, 107,
    107, 107, 108, 108, 108, 109, 109, 109, 110, 110, 110, 110, 111, 111, 111,
    112, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 115, 116, 116,
    116, 117, 117, 117, 118, 118, 118, 118, 119, 119, 119, 120,
};

const uint8_t g_sRGBSamples2[] = {
    120, 121, 122, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149,
    150, 151, 152, 153, 154, 155, 155, 156, 157, 158, 159, 159, 160, 161, 162,
    163, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173,
    174, 175, 175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184,
    185, 185, 186, 187, 187, 188, 189, 189, 190, 190, 191, 192, 192, 193, 194,
    194, 195, 196, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
    203, 204, 205, 205, 206, 206, 207, 208, 208, 209, 209, 210, 210, 211, 212,
    212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 218, 218, 219, 219, 220,
    220, 221, 221, 222, 222, 223, 223, 224, 224, 225, 226, 226, 227, 227, 228,
    228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235,
    236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242,
    243, 243, 244, 244, 245, 245, 246, 246, 246, 247, 247, 248, 248, 249, 249,
    250, 250, 251, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255,
};

HE_FLOAT RGB_Conversion(HE_FLOAT colorComponent) {
    if (colorComponent > 1)
        colorComponent = 1;
    if (colorComponent < 0)
        colorComponent = 0;
    
    int scale = (int)(colorComponent * 1023);
    if (scale < 0)
        scale = 0;
    if (scale < 192)
        colorComponent = (g_sRGBSamples1[scale] / 255.0f);
    else
        colorComponent = (g_sRGBSamples2[scale / 4 - 48] / 255.0f);
    return colorComponent;
}

void XYZ_to_sRGB(HE_FLOAT X,
                 HE_FLOAT Y,
                 HE_FLOAT Z,
                 HE_FLOAT& R,
                 HE_FLOAT& G,
                 HE_FLOAT& B) {
    HE_FLOAT R1 = 3.2410f * X - 1.5374f * Y - 0.4986f * Z;
    HE_FLOAT G1 = -0.9692f * X + 1.8760f * Y + 0.0416f * Z;
    HE_FLOAT B1 = 0.0556f * X - 0.2040f * Y + 1.0570f * Z;
    
    R = RGB_Conversion(R1);
    G = RGB_Conversion(G1);
    B = RGB_Conversion(B1);
}

void XYZ_to_sRGB_WhitePoint(HE_FLOAT X,
                            HE_FLOAT Y,
                            HE_FLOAT Z,
                            HE_FLOAT& R,
                            HE_FLOAT& G,
                            HE_FLOAT& B,
                            HE_FLOAT Xw,
                            HE_FLOAT Yw,
                            HE_FLOAT Zw) {
    // The following RGB_xyz is based on
    // sRGB value {Rx,Ry}={0.64, 0.33}, {Gx,Gy}={0.30, 0.60}, {Bx,By}={0.15, 0.06}
    
    HE_FLOAT Rx = 0.64f, Ry = 0.33f;
    HE_FLOAT Gx = 0.30f, Gy = 0.60f;
    HE_FLOAT Bx = 0.15f, By = 0.06f;
    CFX_Matrix_3by3 RGB_xyz(Rx, Gx, Bx, Ry, Gy, By, 1 - Rx - Ry, 1 - Gx - Gy,
                            1 - Bx - By);
    CFX_Vector_3by1 whitePoint(Xw, Yw, Zw);
    CFX_Vector_3by1 XYZ(X, Y, Z);
    
    CFX_Vector_3by1 RGB_Sum_XYZ = RGB_xyz.Inverse().TransformVector(whitePoint);
    CFX_Matrix_3by3 RGB_SUM_XYZ_DIAG(RGB_Sum_XYZ.a, 0, 0, 0, RGB_Sum_XYZ.b, 0, 0,
                                     0, RGB_Sum_XYZ.c);
    CFX_Matrix_3by3 M = RGB_xyz.Multiply(RGB_SUM_XYZ_DIAG);
    CFX_Vector_3by1 RGB = M.Inverse().TransformVector(XYZ);
    
    R = RGB_Conversion(RGB.a);
    G = RGB_Conversion(RGB.b);
    B = RGB_Conversion(RGB.c);
}



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

CHE_PDF_Color CHE_PDF_CS_CalGray::GetRGB(const CHE_PDF_Color & color)
{
    CHE_PDF_Color ret;
    ret.Push( color.GetComponent(0) );
    ret.Push( color.GetComponent(0) );
    ret.Push( color.GetComponent(0) );
    return  ret;
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

CHE_PDF_Color CHE_PDF_CS_CalRGB::GetRGB(const CHE_PDF_Color & color)
{
    CHE_PDF_Color ret;
    
    HE_FLOAT A_ = color.GetComponent(0);
    HE_FLOAT B_ = color.GetComponent(1);
    HE_FLOAT C_ = color.GetComponent(2);
    if (mGamma) {
        A_ = (HE_FLOAT)powf(A_, mGamma[0]);
        B_ = (HE_FLOAT)powf(B_, mGamma[1]);
        C_ = (HE_FLOAT)powf(C_, mGamma[2]);
    }
    
    HE_FLOAT X;
    HE_FLOAT Y;
    HE_FLOAT Z;
    if (mMatrix) {
        X = mMatrix[0] * A_ + mMatrix[3] * B_ + mMatrix[6] * C_;
        Y = mMatrix[1] * A_ + mMatrix[4] * B_ + mMatrix[7] * C_;
        Z = mMatrix[2] * A_ + mMatrix[5] * B_ + mMatrix[8] * C_;
    } else {
        X = A_;
        Y = B_;
        Z = C_;
    }
    
    HE_FLOAT R, G, B;
    XYZ_to_sRGB_WhitePoint(X, Y, Z, R, G, B, mWhitePoint[0], mWhitePoint[1],
                           mWhitePoint[2]);
    ret.Push(R);
    ret.Push(G);
    ret.Push(B);
    return ret;
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

CHE_PDF_Color CHE_PDF_CS_CalLab::GetRGB(const CHE_PDF_Color & color)
{
    CHE_PDF_Color ret;
    HE_FLOAT Lstar = color.GetComponent(0);
    HE_FLOAT astar = color.GetComponent(1);
    HE_FLOAT bstar = color.GetComponent(2);
    HE_FLOAT M = (Lstar + 16.0f) / 116.0f;
    HE_FLOAT L = M + astar / 500.0f;
    HE_FLOAT N = M - bstar / 200.0f;
    HE_FLOAT X, Y, Z;
    HE_FLOAT R, G, B;
    if (L < 0.2069f)
        X = 0.957f * 0.12842f * (L - 0.1379f);
    else
        X = 0.957f * L * L * L;
    
    if (M < 0.2069f)
        Y = 0.12842f * (M - 0.1379f);
    else
        Y = M * M * M;
    
    if (N < 0.2069f)
        Z = 1.0889f * 0.12842f * (N - 0.1379f);
    else
        Z = 1.0889f * N * N * N;
    
    XYZ_to_sRGB(X, Y, Z, R, G, B);
    ret.Push(R);
    ret.Push(G);
    ret.Push(B);
    return ret;
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
    if (obj)
    {
        mAlternate = CHE_PDF_ColorSpace::Create(obj);
    }else{
        if (mComponentCount == 4) {
            mAlternate = CHE_PDF_ColorSpace::CreateDeviceCMYK();
        }else if (mComponentCount == 3)
        {
            mAlternate = CHE_PDF_ColorSpace::CreateDeviceRGB();
        }else if (mComponentCount == 1)
        {
            mAlternate = CHE_PDF_ColorSpace::CreateDeviceGray();
        }
    }
    mStmAcc.Attach(stream);
}

CHE_PDF_CS_Indexed::CHE_PDF_CS_Indexed(const CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_INDEXED, 1, pAllocator), mIndexCount(0),
 mIndexTableSize(0), mpIndexTable(NULL), mNewTableSize(0), mpNewTable(NULL)
{
    if (array->GetCount() >= 4)
    {
        CHE_PDF_ObjectPtr obj = array->GetElement(1);
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
    if (!mBaseColorSpace)
    {
        abort();
    }
}

CHE_PDF_CS_Pattern::CHE_PDF_CS_Pattern(CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_PATTERN, 0, pAllocator) {}



CHE_PDF_CS_Separation::CHE_PDF_CS_Separation(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_SEPARATION, 1, pAllocator)
{
    CHE_PDF_ObjectPtr objPtr;
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
                    mBaseColorSpace = CHE_PDF_ColorSpace::Create(objPtr, pAllocator);
                }
                objPtr = array->GetElement(3);
                if (objPtr)
                {
                    mFunction = CHE_PDF_Function::Create(objPtr);
                }
            }
        }
    }
}


CHE_PDF_CS_DeviceN::CHE_PDF_CS_DeviceN(CHE_PDF_ArrayPtr & array, CHE_Allocator * pAllocator)
 : CHE_PDF_ColorSpace(COLORSPACE_SPECIAL_DEVICEN, 1, pAllocator)
{
    CHE_PDF_ObjectPtr objPtr;
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
                    mComponentCount = (HE_UINT32)objPtr->GetArrayPtr()->GetCount();
                }
                objPtr = array->GetElement(2);
                if (objPtr)
                {
                    mBaseColorSpace = CHE_PDF_ColorSpace::Create(objPtr, pAllocator);
                }
                objPtr = array->GetElement(3);
                if (objPtr)
                {
                    mFunction = CHE_PDF_Function::Create(objPtr);
                }
            }
        }
    }
}



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