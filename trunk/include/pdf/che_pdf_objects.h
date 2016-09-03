#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_encrypt.h"

#include <string>
#include <vector>
#include <cfloat>
//#include <cmath>
#include <unordered_map>

using namespace std;


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


class CHE_PDF_Object;
class CHE_PDF_Null;
class CHE_PDF_Boolean;
class CHE_PDF_Number;
class CHE_PDF_String;
class CHE_PDF_Name;
class CHE_PDF_Reference;
class CHE_PDF_Array;
class CHE_PDF_Dictionary;
class CHE_PDF_Stream;
class CHE_PDF_File;

class CHE_PDF_ObjectPtr;
class CHE_PDF_NullPtr;
class CHE_PDF_BooleanPtr;
class CHE_PDF_NumberPtr;
class CHE_PDF_StringPtr;
class CHE_PDF_NamePtr;
class CHE_PDF_ReferencePtr;
class CHE_PDF_ArrayPtr;
class CHE_PDF_DictionaryPtr;
class CHE_PDF_StreamPtr;


bool IsPdfNull( CHE_PDF_Object * pObj );

bool IsPdfBoolean( CHE_PDF_Object * pObj );

bool IsPdfNumber( CHE_PDF_Object * pObj );

bool IsPdfName( CHE_PDF_Object * pObj );

bool IsPdfString( CHE_PDF_Object * pObj );

bool IsPdfDict( CHE_PDF_Object * pObj );

bool IsPdfArray( CHE_PDF_Object * pObj );

bool IsPdfRef( CHE_PDF_Object * pObj );

bool IsPdfStream( CHE_PDF_Object * pObj );

bool IsPdfNullPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfBooleanPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfNumberPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfNamePtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfStringPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfDictPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfArrayPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfRefPtr( const CHE_PDF_ObjectPtr & objPtr );

bool IsPdfStreamPtr( const CHE_PDF_ObjectPtr & objPtr );


class CHE_PDF_Object : public CHE_Object
{
public:
    
    PDF_OBJ_TYPE            GetType() const;
    
    CHE_PDF_ObjectPtr		Clone();
    
    void					Release();
    
    void					SetModified( bool );
    
    virtual	bool			IsModified();
    
    CHE_PDF_NullPtr         GetNullPtr() const;
    
    CHE_PDF_BooleanPtr      GetBooleanPtr() const;
    
    CHE_PDF_NumberPtr       GetNumberPtr() const;
    
    CHE_PDF_StringPtr       GetStringPtr() const;
    
    CHE_PDF_NamePtr			GetNamePtr() const;
    
    CHE_PDF_ArrayPtr        GetArrayPtr() const;
    
    CHE_PDF_DictionaryPtr	GetDictPtr() const;
    
    CHE_PDF_ReferencePtr    GetRefPtr() const;
    
    CHE_PDF_StreamPtr		GetStreamPtr() const;
    
protected:
    
    CHE_PDF_Object( PDF_OBJ_TYPE type, CHE_Allocator * pAllocator = nullptr );
    
    bool							mModified;
    
    PDF_OBJ_TYPE					mType;
    
    CHE_RefCount					mRefs;
    
    friend class CHE_Allocator;
    
    friend class CHE_PDF_ObjectPtr;
};

class CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_ObjectPtr() : mpObj( nullptr ) {}
    
    CHE_PDF_ObjectPtr( const CHE_PDF_ObjectPtr & ptr );
    
    ~CHE_PDF_ObjectPtr();
    
    CHE_PDF_ObjectPtr operator = ( const CHE_PDF_ObjectPtr & ptr );
    
    bool operator!() const { return mpObj ? false : TRUE ; }
    
    operator bool() const { return mpObj ? TRUE : false ; }
    
    CHE_PDF_Object * operator->() const { return mpObj; }
    
    void	Reset( CHE_PDF_Object * pObj = nullptr );
    
protected:
    
    CHE_PDF_Object * mpObj;
};

class CHE_PDF_NullPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Null * operator->() const { return /*IsPdfNull( mpObj ) ?*/ (CHE_PDF_Null*)( mpObj ) /* : nullptr*/ ; }
};

class CHE_PDF_BooleanPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Boolean * operator->() const { return /*IsPdfBoolean( mpObj ) ?*/ (CHE_PDF_Boolean*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_NumberPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Number * operator->() const { return /*IsPdfNumber( mpObj ) ?*/ (CHE_PDF_Number*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_StringPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_String * operator->() const { return /*IsPdfString( mpObj ) ?*/ (CHE_PDF_String*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_NamePtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Name * operator->() const { return /*IsPdfName( mpObj ) ?*/ (CHE_PDF_Name*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_ReferencePtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Reference * operator->() const { return /*IsPdfRef( mpObj ) ?*/ (CHE_PDF_Reference*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_ArrayPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Array * operator->() const { return /*IsPdfArray( mpObj ) ?*/ (CHE_PDF_Array*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_DictionaryPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Dictionary * operator->() const { return /*IsPdfDict( mpObj ) ?*/ (CHE_PDF_Dictionary*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_StreamPtr : public CHE_PDF_ObjectPtr
{
public:
    CHE_PDF_Stream * operator->() const { return /*IsPdfStream( mpObj ) ?*/ (CHE_PDF_Stream*)( mpObj ) /*: nullptr*/ ; }
};

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:
    
    static CHE_PDF_NullPtr			Create( CHE_Allocator * pAllocator = nullptr );
    
    CHE_PDF_NullPtr					Clone();
    
private:
    
    CHE_PDF_Null( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Object( OBJ_TYPE_NULL, pAllocator ) {}
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};



class CHE_Point
{
public:
    CHE_Point() : x(0), y(0) {}
    CHE_Point( FLOAT vx, FLOAT vy ) : x(vx), y(vy) {}

    FLOAT	x;
    FLOAT	y;
};

class CHE_Rect
{
public:
    CHE_Rect() : left(0), bottom(0), width(0), height(0) {}
    CHE_Rect( FLOAT vleft, FLOAT vbottom, FLOAT vwidth, FLOAT vheight )
    : left(vleft), bottom(vbottom), width(vwidth), height(vheight) {}

    bool	IsEmpty() const
    {
        return ( fabs(width) < FLT_EPSILON || fabs(height) < FLT_EPSILON );
    }

    bool IsUnion( const CHE_Rect & rect )
    {
        CHE_Point p1;
        CHE_Point p2;
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

    void	Union( const CHE_Rect & rect )
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
                CHE_Rect tmpRect;
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

    bool operator == ( const CHE_Rect & rect ) const
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

    bool operator != ( const CHE_Rect & rect ) const
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

class CHE_Matrix
{
public:
    CHE_Matrix() : a(1), b(0), c(0), d(1), e(0), f(0) {}
    CHE_Matrix( float va, float vb, float vc, float vd, float ve, float vf )
    : a(va), b(vb), c(vc), d(vd), e(ve), f(vf) {}

    static CHE_Matrix TranslateMatrix( FLOAT tx, FLOAT ty )
    {
        CHE_Matrix matrixRet;
        matrixRet.e = tx;
        matrixRet.f = ty;
        return matrixRet;
    }

    static CHE_Matrix RotateMatrix( FLOAT theta )
    {
        float s;
        float c;
        CHE_Matrix matrixRet;

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

    static CHE_Matrix ScaleMatrix( FLOAT sx, FLOAT sy )
    {
        CHE_Matrix matrixRet;
        matrixRet.a = sx;
        matrixRet.d = sy;
        return matrixRet;
    }

    static CHE_Matrix SkewMatrix( FLOAT a, FLOAT b )
    {
        CHE_Matrix matrixRet;
        matrixRet.b = tanf(a);
        matrixRet.c = tanf(b);
        return matrixRet;
    }

    void Concat( const CHE_Matrix & matrix )
    {
        CHE_Matrix tmpMatrix = *this;
        a = tmpMatrix.a * matrix.a +tmpMatrix. b * matrix.c;
        b = tmpMatrix.a * matrix.b + tmpMatrix.b * matrix.d;
        c = tmpMatrix.c * matrix.a + tmpMatrix.d * matrix.c;
        d = tmpMatrix.c * matrix.b + tmpMatrix.d * matrix.d;
        e = tmpMatrix.e * matrix.a + tmpMatrix.f * matrix.c + matrix.e;
        f = tmpMatrix.e * matrix.b + tmpMatrix.f * matrix.d + matrix.f;
    }

    void Invert( const CHE_Matrix & matirx )
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

    CHE_Point Transform( const CHE_Point & point ) const
    {
        CHE_Point pointRet;
        pointRet.x = point.x * a + point.y * c + e;
        pointRet.y = point.x * b + point.y * d + f;
        return pointRet;
    }

    CHE_Rect Transform( const CHE_Rect & rect ) const
    {
        CHE_Point s, t, u, v;
        CHE_Rect retRect;

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
    
    bool operator == ( const CHE_Matrix & matrix ) const
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
    
    bool operator != ( const CHE_Matrix & matrix ) const
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



class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_BooleanPtr		Create( bool value, CHE_Allocator * pAllocator = nullptr );
    
    bool							GetValue() { return mbValue; }
    
    void							SetValue( bool value ) { mbValue = value; SetModified( TRUE ); }
    
    CHE_PDF_BooleanPtr				Clone();
    
private:
    
    CHE_PDF_Boolean( CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( false ) {}
    
    CHE_PDF_Boolean( bool value, CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( value ) {}
    
    bool							mbValue;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_NumberPtr		Create( int32 value,  CHE_Allocator * pAllocator = nullptr );
    
    static CHE_PDF_NumberPtr		Create( FLOAT value, CHE_Allocator * pAllocator = nullptr );
    
    bool							IsInteger() const { return mbInteger; }
    
    int32						GetInteger() const { return mbInteger ? mInteger : (int32)mFloat; }
    
    FLOAT						GetFloat() const { return mbInteger ? (FLOAT)mInteger : mFloat; }
    
    void							SetValue( int32 value ) { mbInteger = TRUE; mInteger = value; SetModified( TRUE ); }
    
    void							SetValue( FLOAT value ) { mbInteger = false; mFloat = value; SetModified( TRUE ); }
    
    CHE_PDF_NumberPtr				Clone();
    
private:
    
    CHE_PDF_Number( int32 value, CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( TRUE ), mInteger( value ) {}
    
    CHE_PDF_Number( FLOAT value, CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( false ), mFloat( value ) {}
    
    bool							mbInteger;
    union {
        int32					mInteger;
        FLOAT					mFloat;
    };
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_String : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_StringPtr		Create( const CHE_ByteString & str, CHE_Allocator * pAllocator = nullptr );
    
    CHE_ByteString &				GetString();
    
    void							SetString( CHE_ByteString & name );
    
    CHE_PDF_StringPtr				Clone();
    
private:
    
    CHE_PDF_String( CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( pAllocator ) {};
    
    CHE_PDF_String( const CHE_ByteString & str, CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( str ) {}
    
    CHE_ByteString					mString;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_NamePtr			Create( const CHE_ByteString & str, CHE_Allocator * pAllocator = nullptr );
    
    CHE_ByteString					GetString() const { return mName; }
    
    void							SetString( CHE_ByteString & name ) { mName = name; SetModified( TRUE ); }
    
    CHE_PDF_NamePtr					Clone();
    
private:
    
    CHE_PDF_Name( const CHE_ByteString& str,  CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_NAME, pAllocator ), mName(str) {}
    
    CHE_ByteString					mName;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

struct PDF_RefInfo
{
    size_t objNum;
    size_t genNum;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_ReferencePtr		Create( size_t refNum, size_t genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = nullptr );
    
    size_t						GetRefNum() const { return mRefObjNum; }
    
    void							SetRefNum( size_t objNum ) { mRefObjNum = objNum; SetModified( TRUE ); }
    
    size_t						GetGenNum() const { return mRefGenNum; }
    
    void							SetGenNum( size_t genNum ) { mRefGenNum = genNum; SetModified( TRUE ); }
    
    PDF_RefInfo						GetRefInfo() { PDF_RefInfo refInfo; refInfo.objNum = mRefObjNum; refInfo.genNum = mRefGenNum; return refInfo; }
    
    void							SetRefInfo( PDF_RefInfo refInfo )  { mRefObjNum = refInfo.objNum; mRefGenNum = refInfo.genNum; SetModified( TRUE ); }
    
    CHE_PDF_ObjectPtr				GetRefObj();
    
    CHE_PDF_ObjectPtr				GetRefObj( PDF_OBJ_TYPE Type );
    
    CHE_PDF_File *					GetFile() const { return mpFile; }
    
    CHE_PDF_ReferencePtr			Clone();
    
private:
    
    CHE_PDF_Reference( size_t refNum, size_t genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = nullptr )
    : CHE_PDF_Object( OBJ_TYPE_REFERENCE, pAllocator ), mRefObjNum( refNum ), mRefGenNum( genNum ), mpFile( pFile ) {}
    
    size_t						mRefObjNum;
    
    size_t						mRefGenNum;
    
    CHE_PDF_File *					mpFile;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:
    static CHE_PDF_ArrayPtr			Create( CHE_Allocator * pAllocator = nullptr );
    
    bool							Append( const CHE_PDF_ObjectPtr & pObj );
    
    CHE_PDF_NullPtr                 AppendNull();
    
    CHE_PDF_BooleanPtr              AppendBoolean(bool val = false);
    
    CHE_PDF_NumberPtr               AppendNumber();
    CHE_PDF_NumberPtr               AppendNumber(int32 val);
    CHE_PDF_NumberPtr               AppendNumber(FLOAT val);
    
    CHE_PDF_NamePtr                 AppendName();
    CHE_PDF_NamePtr                 AppendName(CHE_ByteString & str);
    
    CHE_PDF_StringPtr               AppendString();
    CHE_PDF_StringPtr               AppendString(CHE_ByteString & str);
    
    CHE_PDF_ArrayPtr                AppendArray();
    CHE_PDF_DictionaryPtr           AppendDictionary();
    
    CHE_PDF_ReferencePtr            AppendReference(PDF_RefInfo info, CHE_PDF_File * pFile);
    CHE_PDF_ReferencePtr            AppendReference(size_t objNum, size_t genNum, CHE_PDF_File * pFile);
    CHE_PDF_ReferencePtr            AppendReference(CHE_PDF_ReferencePtr & ref);
    CHE_PDF_ReferencePtr            AppendReference(CHE_PDF_File * pFile);
    
    bool                         Replace( size_t index, const CHE_PDF_ObjectPtr & pObj );
    CHE_PDF_NullPtr                 ReplaceNull( size_t index );
    CHE_PDF_BooleanPtr              ReplaceBoolean( size_t index );
    CHE_PDF_NumberPtr               ReplaceNumber( size_t index );
    CHE_PDF_NamePtr                 ReplaceName( size_t index );
    CHE_PDF_StringPtr               ReplaceString( size_t index );
    CHE_PDF_ArrayPtr                ReplaceArray( size_t index );
    CHE_PDF_DictionaryPtr           ReplaceDictionary( size_t index );
    CHE_PDF_ReferencePtr            ReplaceReference( size_t index, CHE_PDF_File * pFile );
    
    size_t						GetCount() const { return mArray.size(); }
    CHE_PDF_ObjectPtr				GetElement( size_t index ) const;
    CHE_PDF_ObjectPtr				GetElement( size_t index, PDF_OBJ_TYPE Type ) const;
    CHE_PDF_ObjectPtr				GetElementByType( PDF_OBJ_TYPE Type );
    
    CHE_PDF_ArrayPtr				Clone();
    void                         Clear();
    
    bool							IsModified();
    
    bool							GetRect( CHE_Rect & rect ) const;
    bool							GetMatrix( CHE_Matrix & maxtrix ) const;
    
private:
    CHE_PDF_Array( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Object( OBJ_TYPE_ARRAY, pAllocator ) {}
    
    std::vector<CHE_PDF_ObjectPtr>	mArray;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:
    
    static CHE_PDF_DictionaryPtr	Create( CHE_Allocator * pAllocator = nullptr );
    
    bool                         SetObject( const CHE_ByteString & key, const CHE_PDF_ObjectPtr & ptr );
    CHE_PDF_NullPtr                 SetNull( const CHE_ByteString & key );
    CHE_PDF_BooleanPtr              SetBoolean( const CHE_ByteString & key, bool value );
    CHE_PDF_NumberPtr               SetInteger( const CHE_ByteString & key, int32 value );
    CHE_PDF_NumberPtr               SetFloatNumber( const CHE_ByteString & key, FLOAT value );
    CHE_PDF_StringPtr               SetString( const CHE_ByteString & key, const CHE_ByteString & string );
    CHE_PDF_NamePtr                 SetName( const CHE_ByteString & key, const CHE_ByteString & name );
    bool                         SetArray( const CHE_ByteString & key, const CHE_PDF_ArrayPtr & pArray );
    CHE_PDF_ArrayPtr                SetArray( const CHE_ByteString & key );
    bool                         SetDictionary( const CHE_ByteString & key, const CHE_PDF_DictionaryPtr & pDict );
    CHE_PDF_DictionaryPtr           SetDictionary( const CHE_ByteString & key );
    CHE_PDF_ReferencePtr            SetReference( const CHE_ByteString & key, size_t objNum, size_t genNum, CHE_PDF_File * pFile );
    
    bool                         Remove( const CHE_ByteString & key );
    
    size_t						GetCount() { return mMap.size(); }
    CHE_PDF_ObjectPtr				GetElement( const CHE_ByteString & key ) const;
    CHE_PDF_ObjectPtr				GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE type );
    
    CHE_PDF_DictionaryPtr			Clone();
    void                         Clear();
    
    bool							IsModified();
    
    void							MoveToFirst();
    bool							GetKeyAndElement( CHE_ByteString & key, CHE_PDF_ObjectPtr & objPtr );
    bool							CheckName( const CHE_ByteString & key, const CHE_ByteString & name, bool bRequired = TRUE );
    
private:
    CHE_PDF_Dictionary( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Object( OBJ_TYPE_DICTIONARY, pAllocator ) {}
    
    unordered_map<string,CHE_PDF_ObjectPtr>::iterator mIt;
    unordered_map<string,CHE_PDF_ObjectPtr> mMap;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
};

class CHE_PDF_Stream : public CHE_PDF_Object
{
public:
    static CHE_PDF_StreamPtr		Create( size_t objNum, size_t genNum,
                                           CHE_PDF_Encrypt * pEncrypt = nullptr,
                                           CHE_Allocator * pAllocator = nullptr );
    
    static CHE_PDF_StreamPtr		Create( PBYTE pData, size_t size,
                                           const CHE_PDF_DictionaryPtr & pDict,
                                           size_t objNum, size_t genNum,
                                           CHE_PDF_Encrypt * pEncrypt = nullptr,
                                           CHE_Allocator * pAllocator = nullptr );
    
    static CHE_PDF_StreamPtr		Create( IHE_Read* pFile, size_t offset, size_t size,
                                           const CHE_PDF_DictionaryPtr pDict,
                                           size_t objNum, size_t genNum,
                                           CHE_PDF_Encrypt * pEncrypt = nullptr,
                                           CHE_Allocator * pAllocator = nullptr );
    
    size_t						GetObjNum() const { return m_dwObjNum; }
    
    size_t						GetGenNum() const { return m_dwGenNum; }
    
    void							SetDictPtr( const CHE_PDF_DictionaryPtr & ptr );
    
    CHE_PDF_DictionaryPtr			GetDictPtr() const { return mDictPtr; }
    
    size_t						GetRawSize() const { return m_dwSize; }
    
    size_t						GetRawData( size_t offset, PBYTE pBuf, size_t buf_size ) const;
    
    bool							SetRawData( PBYTE pData, size_t dwDataSize, BYTE filter = STREAM_FILTER_NULL );
    
    CHE_PDF_StreamPtr				Clone();
    
    bool							IsModified();
    
private:
    CHE_PDF_Stream(	PBYTE pData, size_t size, const CHE_PDF_DictionaryPtr & pDict, size_t objNum, size_t genNum,
                   CHE_PDF_Encrypt * pEncrypt = nullptr, CHE_Allocator * pAllocator = nullptr );
    
    CHE_PDF_Stream( IHE_Read* pFile, size_t offset, size_t size, const CHE_PDF_DictionaryPtr & pDict, size_t objNum, size_t genNum,
                   CHE_PDF_Encrypt * pEncrypt = nullptr, CHE_Allocator * pAllocator = nullptr );
    
    CHE_PDF_Stream(	size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt = nullptr, CHE_Allocator * pAllocator = nullptr );
    
    ~CHE_PDF_Stream();
    
    CHE_PDF_Encrypt *				m_pEncrypt;
    
    CHE_PDF_DictionaryPtr			mDictPtr;
    size_t						m_dwSize;
    bool							m_bMem;
    union {
        PBYTE					m_pDataBuf;
        IHE_Read*					m_pFile;
    };
    size_t						m_FileOffset;
    
    size_t						m_dwObjNum;
    size_t						m_dwGenNum;
    
    friend class CHE_Allocator;
    friend class CHE_PDF_Object;
    friend class CHE_PDF_StreamAcc;
};

enum PDF_STREAM_DECODE_MODE
{
    STREAM_DECODE_NORMAL,
    STREAM_DECODE_NOTLASTFILTER
};

class CHE_PDF_StreamAcc : public CHE_Object
{
public:
    CHE_PDF_StreamAcc( CHE_Allocator * pAllocator = nullptr );
    ~CHE_PDF_StreamAcc();
    
    bool							Attach( const CHE_PDF_StreamPtr & streamPtr, PDF_STREAM_DECODE_MODE mode = STREAM_DECODE_NORMAL );
    
    void							Detach();
    
    CHE_PDF_StreamPtr				GetStream() const { return mStreamPtr; }
    
    PBYTE						GetData() const { return m_pDataBuf; }
    
    size_t						GetSize() const { return m_dwSize; }
    
private:
    PBYTE						m_pDataBuf;
    size_t						m_dwSize;;
    
    CHE_PDF_StreamPtr				mStreamPtr;
};

#endif