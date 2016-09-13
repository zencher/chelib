#ifndef _CHELIB_PDF_OBJECT_H_
#define _CHELIB_PDF_OBJECT_H_

#include "che_pdf_encrypt.h"

#include <string>
#include <vector>
#include <cfloat>
#include <unordered_map>

namespace chelib {

enum PDF_OBJ_TYPE
{
    OBJ_TYPE_INVALID		= 0x00,
    OBJ_TYPE_NULL			= 0x01,
    OBJ_TYPE_BOOLEAN		= 0x02,
    OBJ_TYPE_NUMBER			= 0x03,
    OBJ_TYPE_STRING			= 0x04,
    OBJ_TYPE_NAME			= 0x05,
    OBJ_TYPE_ARRAY			= 0x06,
    OBJ_TYPE_DICTIONARY		= 0x07,
    OBJ_TYPE_STREAM			= 0x08,
    OBJ_TYPE_REFERENCE		= 0x09
};


enum PDF_STREAM_FILTER
{
    STREAM_FILTER_NULL		= 0x00,
    STREAM_FILTER_HEX		= 0x01,
    STREAM_FILTER_ASCII85	= 0x02,
    STREAM_FILTER_FLATE		= 0x03,
    STREAM_FILTER_LZW		= 0x04,
    STREAM_FILTER_RLE		= 0x05
};


class CPDF_Object;
class CPDF_Null;
class CPDF_Boolean;
class CPDF_Number;
class CPDF_String;
class CPDF_Name;
class CPDF_Reference;
class CPDF_Array;
class CPDF_Dictionary;
class CPDF_Stream;
class CPDF_File;

class CPDF_ObjectPtr;
class CPDF_NullPtr;
class CPDF_BooleanPtr;
class CPDF_NumberPtr;
class CPDF_StringPtr;
class CPDF_NamePtr;
class CPDF_ReferencePtr;
class CPDF_ArrayPtr;
class CPDF_DictionaryPtr;
class CPDF_StreamPtr;


bool IsPdfNull( CPDF_Object * pObj );
bool IsPdfBoolean( CPDF_Object * pObj );
bool IsPdfNumber( CPDF_Object * pObj );
bool IsPdfName( CPDF_Object * pObj );
bool IsPdfString( CPDF_Object * pObj );
bool IsPdfDict( CPDF_Object * pObj );
bool IsPdfArray( CPDF_Object * pObj );
bool IsPdfRef( CPDF_Object * pObj );
bool IsPdfStream( CPDF_Object * pObj );
bool IsPdfNullPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfBooleanPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfNumberPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfNamePtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfStringPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfDictPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfArrayPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfRefPtr( const CPDF_ObjectPtr & objPtr );
bool IsPdfStreamPtr( const CPDF_ObjectPtr & objPtr );


class CPDF_Object : public CObject
{
public:
    PDF_OBJ_TYPE        GetType() const;
    
    CPDF_ObjectPtr		Clone();
    void                Release();
    
    void                SetModified( bool );
    virtual	bool		IsModified();
    
    CPDF_NullPtr        GetNullPtr() const;
    CPDF_BooleanPtr     GetBooleanPtr() const;
    CPDF_NumberPtr      GetNumberPtr() const;
    CPDF_StringPtr      GetStringPtr() const;
    CPDF_NamePtr        GetNamePtr() const;
    CPDF_ArrayPtr       GetArrayPtr() const;
    CPDF_DictionaryPtr  GetDictPtr() const;
    CPDF_ReferencePtr   GetRefPtr() const;
    CPDF_StreamPtr      GetStreamPtr() const;
    
protected:
    CPDF_Object( PDF_OBJ_TYPE type, CAllocator * pAllocator = nullptr );
    
    bool                mModified;
    PDF_OBJ_TYPE        mType;
    CRefCount           mRefs;
    
    friend class CAllocator;
    friend class CPDF_ObjectPtr;
};

class CPDF_ObjectPtr
{
public:
    CPDF_ObjectPtr() : mpObj( nullptr ) {}
    CPDF_ObjectPtr( const CPDF_ObjectPtr & ptr );
    ~CPDF_ObjectPtr();
    
    CPDF_ObjectPtr operator=( const CPDF_ObjectPtr & ptr );
    bool operator!() const { return mpObj ? false : TRUE; }
    operator bool() const { return mpObj ? TRUE : false; }
    CPDF_Object * operator->() const { return mpObj; }
    
    void Reset( CPDF_Object * pObj = nullptr );
    
protected:
    CPDF_Object * mpObj;
};

class CPDF_NullPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Null * operator->() const { return (CPDF_Null*)( mpObj ); }
};

class CPDF_BooleanPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Boolean * operator->() const { return (CPDF_Boolean*)( mpObj ); }
};

class CPDF_NumberPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Number * operator->() const { return (CPDF_Number*)( mpObj ); }
};

class CPDF_StringPtr : public CPDF_ObjectPtr
{
public:
    CPDF_String * operator->() const { return (CPDF_String*)( mpObj ); }
};

class CPDF_NamePtr : public CPDF_ObjectPtr
{
public:
    CPDF_Name * operator->() const { return (CPDF_Name*)( mpObj ); }
};

class CPDF_ReferencePtr : public CPDF_ObjectPtr
{
public:
    CPDF_Reference * operator->() const { return (CPDF_Reference*)( mpObj ); }
};

class CPDF_ArrayPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Array * operator->() const { return (CPDF_Array*)( mpObj ); }
};

class CPDF_DictionaryPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Dictionary * operator->() const { return (CPDF_Dictionary*)( mpObj ); }
};

class CPDF_StreamPtr : public CPDF_ObjectPtr
{
public:
    CPDF_Stream * operator->() const { return (CPDF_Stream*)( mpObj ); }
};

class CPDF_Null	: public CPDF_Object
{
public:
    static CPDF_NullPtr Create( CAllocator * pAllocator = nullptr );
    
    CPDF_NullPtr Clone();
    
private:
    CPDF_Null( CAllocator * pAllocator = nullptr ) : CPDF_Object(OBJ_TYPE_NULL, pAllocator) {}
    
    friend class CAllocator;
    friend class CPDF_Object;
};



class CPoint
{
public:
    CPoint() : x(0), y(0) {}
    CPoint( FLOAT vx, FLOAT vy ) : x(vx), y(vy) {}

    FLOAT	x;
    FLOAT	y;
};

class CRect
{
public:
    CRect() : left(0), bottom(0), width(0), height(0) {}
    CRect( FLOAT vleft, FLOAT vbottom, FLOAT vwidth, FLOAT vheight )
      : left(vleft), bottom(vbottom), width(vwidth), height(vheight) {}

    bool IsEmpty() const
    {
        return ( fabs(width) < FLT_EPSILON || fabs(height) < FLT_EPSILON );
    }

    bool IsUnion( const CRect & rect )
    {
        CPoint p1;
        CPoint p2;
        p1.x = left + width / 2;
        p1.y = bottom + height / 2;
        p2.x = rect.left + rect.width / 2;
        p2.y = rect.bottom + rect.height / 2;
        FLOAT xDis = fabsf( p1.x - p2.x );
        FLOAT yDis = fabsf( p1.y - p2.y );
        if ( ( xDis < ( rect.width + width ) / 2 ) && ( yDis < ( rect.height + height ) / 2 ) )
        {
            return true;
        }
        return false;
    }

    void Union( const CRect & rect )
    {
        if ( &rect != this )
        {
            if ( IsEmpty() )
            {
                *this = rect;
                return;
            }
            if ( !rect.IsEmpty() )
            {
                CRect tmpRect;
                tmpRect.left = ( left <= rect.left ) ? left : rect.left;
                tmpRect.bottom = ( bottom <= rect.bottom ) ? bottom : rect.bottom;
                tmpRect.width = ( left + width >= rect.left + rect.width ) ? left + width : rect.left + rect.width;
                tmpRect.width -= tmpRect.left;
                tmpRect.height = ( bottom + height >= rect.bottom + rect.height ) ? bottom + height : rect.bottom + rect.height;
                tmpRect.height -= tmpRect.bottom;
                *this = tmpRect;
            }
        }
    }

    bool operator==( const CRect & rect ) const
    {
        if (	fabsf( left - rect.left ) <= FLT_EPSILON &&
            fabsf( bottom - rect.bottom ) <= FLT_EPSILON &&
            fabsf( width - rect.width ) <= FLT_EPSILON &&
            fabsf( height - rect.height ) <= FLT_EPSILON )
        {
            return true;
        }
        return false;
    }

    bool operator!=( const CRect & rect ) const
    {
        return ! operator==( rect );
    }

    FLOAT	left;
    FLOAT	bottom;
    FLOAT	width;
    FLOAT	height;
};


#define HE_MAX4(a,b,c,d) HE_max(HE_max(a,b), HE_max(c,d))
#define HE_MIN4(a,b,c,d) HE_min(HE_min(a,b), HE_min(c,d))
static inline float HE_min(float a, float b)
{
    return (a < b ? a : b);
}
static inline float HE_max(float a, float b)
{
    return (a > b ? a : b);
}

class CMatrix
{
public:
    CMatrix() : a(1), b(0), c(0), d(1), e(0), f(0) {}
    CMatrix( float va, float vb, float vc, float vd, float ve, float vf )
    : a(va), b(vb), c(vc), d(vd), e(ve), f(vf) {}

    static CMatrix TranslateMatrix( FLOAT tx, FLOAT ty )
    {
        CMatrix matrixRet;
        matrixRet.e = tx;
        matrixRet.f = ty;
        return matrixRet;
    }

    static CMatrix RotateMatrix( FLOAT theta )
    {
        float s;
        float c;
        CMatrix matrixRet;

        while ( theta < 0 )
            theta += 360;
        while ( theta >= 360 )
            theta -= 360;

        if ( fabsf(0 - theta) < FLT_EPSILON )
        {
            s = 0;
            c = 1;
        }
        else if ( fabsf(90.0f - theta) < FLT_EPSILON )
        {
            s = 1;
            c = 0;
        }
        else if ( fabsf(180.0f - theta) < FLT_EPSILON )
        {
            s = 0;
            c = -1;
        }
        else if ( fabsf(270.0f - theta) < FLT_EPSILON )
        {
            s = -1;
            c = 0;
        }
        else
        {
            s = sinf(theta * (float)M_PI / 180);
            c = cosf(theta * (float)M_PI / 180);
        }

        matrixRet.a = c; matrixRet.b = s;
        matrixRet.c = -s; matrixRet.d = c;
        matrixRet.e = 0; matrixRet.f = 0;
        return matrixRet;
    }

    static CMatrix ScaleMatrix( FLOAT sx, FLOAT sy )
    {
        CMatrix matrixRet;
        matrixRet.a = sx;
        matrixRet.d = sy;
        return matrixRet;
    }

    static CMatrix SkewMatrix( FLOAT a, FLOAT b )
    {
        CMatrix matrixRet;
        matrixRet.b = tanf(a);
        matrixRet.c = tanf(b);
        return matrixRet;
    }

    void Concat( const CMatrix & matrix )
    {
        CMatrix tmpMatrix = *this;
        a = tmpMatrix.a * matrix.a +tmpMatrix. b * matrix.c;
        b = tmpMatrix.a * matrix.b + tmpMatrix.b * matrix.d;
        c = tmpMatrix.c * matrix.a + tmpMatrix.d * matrix.c;
        d = tmpMatrix.c * matrix.b + tmpMatrix.d * matrix.d;
        e = tmpMatrix.e * matrix.a + tmpMatrix.f * matrix.c + matrix.e;
        f = tmpMatrix.e * matrix.b + tmpMatrix.f * matrix.d + matrix.f;
    }

    void Invert( const CMatrix & matirx )
    {
        float rdet = matirx.AbsValue();
        a = matirx.d * rdet;
        b = - matirx.b * rdet;
        c = - matirx.c * rdet;
        d = matirx.a * rdet;
        e = - matirx.e * a - matirx.f * c;
        f = - matirx.e * b - matirx.f * d;
    }

    FLOAT AbsValue() const
    {
        FLOAT val = 1 / ( a * d - b * c );
        return val;
    }

    CPoint Transform( const CPoint & point ) const
    {
        CPoint pointRet;
        pointRet.x = point.x * a + point.y * c + e;
        pointRet.y = point.x * b + point.y * d + f;
        return pointRet;
    }

    CRect Transform( const CRect & rect ) const
    {
        CPoint s, t, u, v;
        CRect retRect;

        s.x = rect.left;
        s.y = rect.bottom;
        t.x = rect.left;
        t.y = rect.bottom + rect.height;
        u.x = rect.left + rect.width;
        u.y = rect.bottom + rect.height;
        v.x = rect.left + rect.width;
        v.y = rect.bottom;
        s = Transform( s );
        t = Transform( t );
        u = Transform( u );
        v = Transform( v );

        retRect.left = HE_MIN4(s.x, t.x, u.x, v.x);
        retRect.bottom = HE_MIN4(s.y, t.y, u.y, v.y);
        retRect.width = HE_MAX4(s.x, t.x, u.x, v.x) - HE_MIN4(s.x, t.x, u.x, v.x);
        retRect.height = HE_MAX4(s.y, t.y, u.y, v.y) - HE_MIN4(s.y, t.y, u.y, v.y);
        return retRect;
    }

    bool operator == ( const CMatrix & matrix ) const
    {
        if ( fabsf( a - matrix.a ) <= FLT_EPSILON &&
            fabsf( b - matrix.b ) <= FLT_EPSILON &&
            fabsf( c - matrix.c ) <= FLT_EPSILON &&
            fabsf( d - matrix.d ) <= FLT_EPSILON && 
            fabsf( e - matrix.e ) <= FLT_EPSILON &&
            fabsf( f - matrix.f ) <= FLT_EPSILON )
        {
            return true;
        }
        return false;
    }

    bool operator != ( const CMatrix & matrix ) const
    {
        return ! operator==( matrix );
    }

    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
};


class CPDF_Boolean : public CPDF_Object
{
public:
    static CPDF_BooleanPtr Create( bool value, CAllocator * pAllocator = nullptr );

    CPDF_BooleanPtr Clone();

    bool GetValue() { return mbValue; }
    void SetValue( bool value ) { mbValue = value; SetModified( TRUE ); }

private:
    CPDF_Boolean( CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( false ) {}

    CPDF_Boolean( bool value, CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( value ) {}

    bool mbValue;

    friend class CAllocator;
    friend class CPDF_Object;
};

class CPDF_Number : public CPDF_Object
{
public:
    static CPDF_NumberPtr Create( int32 value,  CAllocator * pAllocator = nullptr );
    static CPDF_NumberPtr Create( FLOAT value, CAllocator * pAllocator = nullptr );

    CPDF_NumberPtr Clone();

    bool IsInteger() const { return mbInteger; }

    int32 GetInteger() const { return mbInteger ? mInteger : (int32)mFloat; }
    FLOAT GetFloat() const { return mbInteger ? (FLOAT)mInteger : mFloat; }

    void SetValue( int32 value ) { mbInteger = TRUE; mInteger = value; SetModified( TRUE ); }
    void SetValue( FLOAT value ) { mbInteger = false; mFloat = value; SetModified( TRUE ); }

private:
    CPDF_Number( int32 value, CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( TRUE ), mInteger( value ) {}

    CPDF_Number( FLOAT value, CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( false ), mFloat( value ) {}

    bool mbInteger;
    union {
        int32 mInteger;
        FLOAT mFloat;
    };

    friend class CAllocator;
    friend class CPDF_Object;
};
    
class CPDF_String : public CPDF_Object
{
public:
    static CPDF_StringPtr Create( const CByteString & str, CAllocator * pAllocator = nullptr );

    CPDF_StringPtr Clone();

    CByteString & GetString();
    void SetString( CByteString & name );

private:
    CPDF_String( CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( pAllocator ) {};

    CPDF_String( const CByteString & str, CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( str ) {}

    CByteString mString;

    friend class CAllocator;
    friend class CPDF_Object;
};

class CPDF_Name : public CPDF_Object
{
public:
    static CPDF_NamePtr Create( const CByteString & str, CAllocator * pAllocator = nullptr );

    CPDF_NamePtr Clone();

    CByteString GetString() const { return mName; }
    void SetString( CByteString & name ) { mName = name; SetModified( TRUE ); }

private:
    CPDF_Name( const CByteString& str,  CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_NAME, pAllocator ), mName(str) {}

    CByteString mName;

    friend class CAllocator;
    friend class CPDF_Object;
};

struct PDF_RefInfo
{
    size_t objNum;
    size_t genNum;
};

class CPDF_Reference : public CPDF_Object
{
public:
    static CPDF_ReferencePtr Create( size_t refNum, size_t genNum, CPDF_File * pFile, CAllocator * pAllocator = nullptr );

    CPDF_ReferencePtr Clone();

    size_t GetRefNum() const { return mRefObjNum; }
    void SetRefNum( size_t objNum ) { mRefObjNum = objNum; SetModified( TRUE ); }
    size_t GetGenNum() const { return mRefGenNum; }
    void SetGenNum( size_t genNum ) { mRefGenNum = genNum; SetModified( TRUE ); }
    PDF_RefInfo GetRefInfo() { PDF_RefInfo refInfo; refInfo.objNum = mRefObjNum; refInfo.genNum = mRefGenNum; return refInfo; }
    void SetRefInfo( PDF_RefInfo refInfo )  { mRefObjNum = refInfo.objNum; mRefGenNum = refInfo.genNum; SetModified( TRUE ); }

    CPDF_ObjectPtr GetRefObj();
    CPDF_ObjectPtr GetRefObj( PDF_OBJ_TYPE Type );

    CPDF_File * GetFile() const { return mpFile; }

private:
    CPDF_Reference( size_t refNum, size_t genNum, CPDF_File * pFile, CAllocator * pAllocator = nullptr )
      : CPDF_Object(OBJ_TYPE_REFERENCE, pAllocator), mRefObjNum(refNum), mRefGenNum(genNum), mpFile(pFile) {}
    
    size_t mRefObjNum;
    size_t mRefGenNum;
    CPDF_File * mpFile;
    
    friend class CAllocator;
    friend class CPDF_Object;
};

class CPDF_Array : public CPDF_Object
{
public:
    static CPDF_ArrayPtr Create( CAllocator * pAllocator = nullptr );
    
    CPDF_ArrayPtr Clone();
    
    bool IsModified();
    
    bool Append( const CPDF_ObjectPtr & pObj );
    bool Replace( size_t index, const CPDF_ObjectPtr & pObj );
    void Clear();
    
    CPDF_NullPtr        AppendNull();
    CPDF_BooleanPtr     AppendBoolean(bool val = false);
    CPDF_NumberPtr      AppendNumber();
    CPDF_NumberPtr      AppendNumber(int32 val);
    CPDF_NumberPtr      AppendNumber(FLOAT val);
    CPDF_NamePtr        AppendName();
    CPDF_NamePtr        AppendName(CByteString & str);
    CPDF_StringPtr      AppendString();
    CPDF_StringPtr      AppendString(CByteString & str);
    CPDF_ArrayPtr       AppendArray();
    CPDF_DictionaryPtr  AppendDictionary();
    CPDF_ReferencePtr   AppendReference(PDF_RefInfo info, CPDF_File * pFile);
    CPDF_ReferencePtr   AppendReference(size_t objNum, size_t genNum, CPDF_File * pFile);
    CPDF_ReferencePtr   AppendReference(CPDF_ReferencePtr & ref);
    CPDF_ReferencePtr   AppendReference(CPDF_File * pFile);
    
    CPDF_NullPtr        ReplaceNull( size_t index );
    CPDF_BooleanPtr     ReplaceBoolean( size_t index );
    CPDF_NumberPtr      ReplaceNumber( size_t index );
    CPDF_NamePtr        ReplaceName( size_t index );
    CPDF_StringPtr      ReplaceString( size_t index );
    CPDF_ArrayPtr       ReplaceArray( size_t index );
    CPDF_DictionaryPtr  ReplaceDictionary( size_t index );
    CPDF_ReferencePtr   ReplaceReference( size_t index, CPDF_File * pFile );
    
    size_t GetCount() const { return mArray.size(); }
    CPDF_ObjectPtr GetElement( size_t index ) const;
    CPDF_ObjectPtr GetElement( size_t index, PDF_OBJ_TYPE Type ) const;
    CPDF_ObjectPtr GetElementByType( PDF_OBJ_TYPE Type );
    
    bool GetRect( CRect & rect ) const;
    bool GetMatrix( CMatrix & maxtrix ) const;
    
private:
    CPDF_Array( CAllocator * pAllocator = nullptr )
      : CPDF_Object( OBJ_TYPE_ARRAY, pAllocator ) {}
    
    std::vector<CPDF_ObjectPtr>	mArray;
    
    friend class CAllocator;
    friend class CPDF_Object;
};

class CPDF_Dictionary : public CPDF_Object
{
public:
    static CPDF_DictionaryPtr Create( CAllocator * pAllocator = nullptr );
    
    CPDF_DictionaryPtr Clone();
    void Clear();
    
    bool IsModified();
    
    bool                SetObject( const CByteString & key, const CPDF_ObjectPtr & ptr );
    CPDF_NullPtr        SetNull( const CByteString & key );
    CPDF_BooleanPtr     SetBoolean( const CByteString & key, bool value );
    CPDF_NumberPtr      SetInteger( const CByteString & key, int32 value );
    CPDF_NumberPtr      SetFloatNumber( const CByteString & key, FLOAT value );
    CPDF_StringPtr      SetString( const CByteString & key, const CByteString & string );
    CPDF_NamePtr        SetName( const CByteString & key, const CByteString & name );
    bool                SetArray( const CByteString & key, const CPDF_ArrayPtr & pArray );
    CPDF_ArrayPtr       SetArray( const CByteString & key );
    bool                SetDictionary( const CByteString & key, const CPDF_DictionaryPtr & pDict );
    CPDF_DictionaryPtr  SetDictionary( const CByteString & key );
    CPDF_ReferencePtr   SetReference( const CByteString & key, size_t objNum, size_t genNum, CPDF_File * pFile );
    
    size_t GetCount() { return mMap.size(); }
    CPDF_ObjectPtr GetElement( const CByteString & key ) const;
    CPDF_ObjectPtr GetElement( const CByteString & key, PDF_OBJ_TYPE type );
    
    bool Remove( const CByteString & key );
    
    void MoveToFirst();
    
    bool GetKeyAndElement( CByteString & key, CPDF_ObjectPtr & objPtr );
    
    bool CheckName( const CByteString & key, const CByteString & name, bool bRequired = TRUE );
    
private:
    CPDF_Dictionary( CAllocator * pAllocator = nullptr ) : CPDF_Object( OBJ_TYPE_DICTIONARY, pAllocator ) {}
    
    std::unordered_map<std::string,CPDF_ObjectPtr>::iterator mIt;
    std::unordered_map<std::string,CPDF_ObjectPtr> mMap;
    
    friend class CAllocator;
    friend class CPDF_Object;
};

class CPDF_Stream : public CPDF_Object
{
public:
    static CPDF_StreamPtr Create( size_t objNum, size_t genNum,
                                  CPDF_Encrypt * pEncrypt = nullptr,
                                  CAllocator * pAllocator = nullptr );
    
    static CPDF_StreamPtr Create( PBYTE pData, size_t size,
                                  const CPDF_DictionaryPtr & pDict,
                                  size_t objNum, size_t genNum,
                                  CPDF_Encrypt * pEncrypt = nullptr,
                                  CAllocator * pAllocator = nullptr );
    
    static CPDF_StreamPtr Create( IRead* pFile, size_t offset, size_t size,
                                  const CPDF_DictionaryPtr pDict,
                                  size_t objNum, size_t genNum,
                                  CPDF_Encrypt * pEncrypt = nullptr,
                                  CAllocator * pAllocator = nullptr );
    
    CPDF_StreamPtr Clone();
    
    bool IsModified();
    
    size_t GetObjNum() const { return m_dwObjNum; }
    size_t GetGenNum() const { return m_dwGenNum; }
    
    void SetDictPtr( const CPDF_DictionaryPtr & ptr );
    CPDF_DictionaryPtr GetDictPtr() const { return mDictPtr; }
    
    size_t GetRawSize() const { return m_dwSize; }
    size_t GetRawData( size_t offset, PBYTE pBuf, size_t buf_size ) const;
    bool SetRawData( PBYTE pData, size_t dwDataSize, BYTE filter = STREAM_FILTER_NULL );

private:
    CPDF_Stream( PBYTE pData, size_t size, const CPDF_DictionaryPtr & pDict,
                 size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt = nullptr,
                 CAllocator * pAllocator = nullptr );
    
    CPDF_Stream( IRead* pFile, size_t offset, size_t size, const CPDF_DictionaryPtr & pDict,
                 size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt = nullptr,
                  CAllocator * pAllocator = nullptr );
    
    CPDF_Stream( size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt = nullptr,
                 CAllocator * pAllocator = nullptr );
    
    ~CPDF_Stream();
    
    CPDF_Encrypt * m_pEncrypt;
    CPDF_DictionaryPtr mDictPtr;
    
    bool m_bMem;
    union {
        PBYTE m_pDataBuf;
        IRead* m_pFile;
    };
    size_t m_dwSize;
    size_t m_FileOffset;
    size_t m_dwObjNum;
    size_t m_dwGenNum;
    
    friend class CAllocator;
    friend class CPDF_Object;
    friend class CPDF_StreamAcc;
};

enum PDF_STREAM_DECODE_MODE
{
    STREAM_DECODE_NORMAL,
    STREAM_DECODE_NOTLASTFILTER
};

class CPDF_StreamAcc : public CObject
{
public:
    CPDF_StreamAcc( CAllocator * pAllocator = nullptr );
    ~CPDF_StreamAcc();
    
    bool Attach( const CPDF_StreamPtr & streamPtr, PDF_STREAM_DECODE_MODE mode = STREAM_DECODE_NORMAL );
    void Detach();
    
    CPDF_StreamPtr				GetStream() const { return mStreamPtr; }
    
    PBYTE						GetData() const { return m_pDataBuf; }
    size_t						GetSize() const { return m_dwSize; }
    
private:
    PBYTE m_pDataBuf;
    size_t m_dwSize;;
    CPDF_StreamPtr mStreamPtr;
};

}//namespace

#endif