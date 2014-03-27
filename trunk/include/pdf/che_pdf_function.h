#ifndef _CHE_PDF_FUNCTION_H_
#define _CHE_PDF_FUNCTION_H_

#include <vector>
using namespace std;

#include "che_pdf_component.h"
#include "che_pdf_parser.h"

enum PDF_FUNCTION_TYPE
{
	FUNCTION_TYPE_SAMPLE		= 0,
	FUNCTION_TYPE_EXPONENTIAL	= 2,
	FUNCTION_TYPE_STITCHING		= 3,
	FUNCTION_TYPE_POSTSCRIPT	= 4
};


class CHE_PDF_Function;

class CHE_PDF_FunctionPtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_Function * operator->() const;
};

class CHE_PDF_Function : public CHE_PDF_Component
{
public:
	static CHE_PDF_FunctionPtr	Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator = NULL );

	virtual ~CHE_PDF_Function() {};

	PDF_FUNCTION_TYPE			GetType() const { return mType; }
	HE_UINT32					GetInputCount() const { return mInputCount; } 
	HE_UINT32					GetOutputCount() const { return mOutputCount; }
	HE_FLOAT					GetDomianMin( HE_UINT32 index ) const;
	HE_FLOAT					GetDomianMax( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMin( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMax( HE_UINT32 index ) const;
	HE_BOOL						HasDomian() const { return mpDomain != NULL; }
	HE_BOOL						HasRange() const { return mpRange != NULL; }

	virtual HE_BOOL				Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output ) = 0;

protected:
	CHE_PDF_Function( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator );

	PDF_FUNCTION_TYPE			mType;
	HE_UINT32					mInputCount;
	HE_UINT32					mOutputCount;
	HE_FLOAT*					mpDomain;
	HE_FLOAT*					mpRange;
	HE_BOOL						mbRange;

	friend class CHE_Allocator;
};


class CHE_PDF_Function_Sampled : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Sampled();

	HE_BOOL Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output );

private:
	CHE_PDF_Function_Sampled( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator );

	HE_INT32 GetSize( HE_UINT32 index ) const;
	HE_FLOAT GetEncodeMin( HE_UINT32 index ) const;
	HE_FLOAT GetEncodeMax( HE_UINT32 index ) const;
	HE_FLOAT GetDecodeMin( HE_UINT32 index ) const;
	HE_FLOAT GetDecodeMax( HE_UINT32 index ) const;

	float interpolate_sample(int *scale, int *e0, int *e1, float *efrac, int dim, int idx);

	HE_BYTE		mBps;
	HE_BYTE		mOrder;
	HE_INT32*	mpSize;
	HE_FLOAT*	mpEncode;
	HE_FLOAT*	mpDecode;
	HE_BYTE *	mpSample;

	friend class CHE_Allocator;
};


class CHE_PDF_Function_Exponential : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Exponential();

	HE_BOOL Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output );

private:
	CHE_PDF_Function_Exponential( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator );

	HE_INT32	mN;
	HE_FLOAT*	mpC0;
	HE_FLOAT*	mpC1;

	friend class CHE_Allocator;
};

class CHE_PDF_Function_Stitching : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Stitching();

	HE_BOOL Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output );

private:
	CHE_PDF_Function_Stitching( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator );

	HE_FLOAT GetEncodeMin( HE_UINT32 index ) const;
	HE_FLOAT GetEncodeMax( HE_UINT32 index ) const;

	HE_UINT32					mK;
	vector<HE_FLOAT>			mBounds;
	vector<HE_FLOAT>			mEncode;
	vector<CHE_PDF_FunctionPtr>	mFunctions;

	friend class CHE_Allocator;
};

enum PSFUNCITEM_TYPE
{
	PSITEM_BOOL,
	PSITEM_INT,
	PSITEM_FLOAT,
	PSITEM_OPERATOR,
	PSITEM_BLOCK
};

enum PSFUNCITEM_OPERATOR
{
	PSOPERATOR_ABS, PSOPERATOR_ADD, PSOPERATOR_AND, PSOPERATOR_ATAN, PSOPERATOR_BITSHIFT,
	PSOPERATOR_CEILING, PSOPERATOR_COPY, PSOPERATOR_COS, PSOPERATOR_CVI, PSOPERATOR_CVR,
	PSOPERATOR_DIV, PSOPERATOR_DUP, PSOPERATOR_EQ, PSOPERATOR_EXCH, PSOPERATOR_EXP,
	PSOPERATOR_FALSE, PSOPERATOR_FLOOR, PSOPERATOR_GE, PSOPERATOR_GT, PSOPERATOR_IDIV, PSOPERATOR_IF,
	PSOPERATOR_IFELSE, PSOPERATOR_INDEX, PSOPERATOR_LE, PSOPERATOR_LN, PSOPERATOR_LOG, PSOPERATOR_LT,
	PSOPERATOR_MOD, PSOPERATOR_MUL, PSOPERATOR_NE, PSOPERATOR_NEG, PSOPERATOR_NOT, PSOPERATOR_OR,
	PSOPERATOR_POP, PSOPERATOR_RETURN, PSOPERATOR_ROLL, PSOPERATOR_ROUND, PSOPERATOR_SIN,
	PSOPERATOR_SQRT, PSOPERATOR_SUB, PSOPERATOR_TRUE, PSOPERATOR_TRUNCATE, PSOPERATOR_XOR
};

class PSFuncItem
{
public:
	PSFUNCITEM_TYPE				mType;
	union{
		PSFUNCITEM_OPERATOR		mOperator;
		HE_BOOL					mBoolValue;
		HE_FLOAT				mFloatValue;
		HE_INT32				mIntegerValue;
		HE_INT32				mBlockIndex;
	};
};

class PSFuncStack
{
public:
	PSFuncStack() : mStackTopIndex(0) {}

	HE_VOID	Init()
	{
		mStackTopIndex = 0;
	}

	HE_BOOL IsOverflow( HE_INT32 n = 1 )
	{
		if ( mStackTopIndex + n > 100 )
		{
			return TRUE;
		}
		return FALSE;
	}

	HE_BOOL IsUnderflow( HE_INT32 n = 1 )
	{
		if ( mStackTopIndex - n < 0 )
		{
			return TRUE;
		}
		return FALSE;
	}

	HE_BOOL IsType( PSFUNCITEM_TYPE type )
	{
		if ( ! IsUnderflow() )
		{
			if ( mStack[mStackTopIndex-1].mType == type )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	HE_BOOL IsType2( PSFUNCITEM_TYPE type )
	{
		if ( ! IsUnderflow( 2 ) )
		{
			if (	mStack[mStackTopIndex-1].mType == type &&
					mStack[mStackTopIndex-2].mType == type )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	HE_BOOL Push( PSFuncItem & item )
	{
		if ( ! IsOverflow() )
		{
			mStack[mStackTopIndex++] = item;
			return TRUE; 
		}
		return FALSE;
	}

	HE_BOOL PushBool( HE_BOOL val )
	{
		PSFuncItem item;
		item.mType = PSITEM_BOOL;
		item.mBoolValue = val;
		return Push( item );
	}

	HE_BOOL PushInteger( HE_INT32 val )
	{
		PSFuncItem item;
		item.mType = PSITEM_INT;
		item.mIntegerValue = val;
		return Push( item );
	}

	HE_BOOL PushFloat( HE_FLOAT val )
	{
		PSFuncItem item;
		item.mType = PSITEM_FLOAT;
		item.mFloatValue = val;
		return Push( item );
	}

	HE_BOOL PopBool( HE_BOOL & valRet )
	{
		if ( !IsUnderflow() )
		{
			if ( IsType(PSITEM_BOOL) )
			{
				valRet = mStack[--mStackTopIndex].mBoolValue;
				return TRUE;
			}
		}
		return FALSE;
	}

	HE_BOOL PopInteger( HE_INT32 & valRet )
	{
		if ( ! IsUnderflow() )
		{
			if ( IsType( PSITEM_INT ) )
			{
				valRet = mStack[--mStackTopIndex].mIntegerValue;
				return TRUE;
			}else if ( IsType( PSITEM_FLOAT ) )
			{
				valRet = mStack[--mStackTopIndex].mFloatValue;
				return TRUE;
			}
		}
		return FALSE;
	}

	HE_BOOL PopFloat( HE_FLOAT & valRet )
	{
		if ( ! IsUnderflow() )
		{
			if ( IsType( PSITEM_INT ) )
			{
				valRet = mStack[--mStackTopIndex].mIntegerValue;
				return TRUE;
			}else if ( IsType( PSITEM_FLOAT ) )
			{
				valRet = mStack[--mStackTopIndex].mFloatValue;
				return TRUE;
			}
		}
		return FALSE;
	}

	HE_BOOL Copy( HE_INT32 n )
	{
		if ( !IsUnderflow( n ) && !IsOverflow( n ) )
		{
			memcpy( mStack + mStackTopIndex, mStack + mStackTopIndex - n, n * sizeof(PSFuncItem) );
			mStackTopIndex += n;
			return TRUE;
		}
		return FALSE;
	}

	HE_BOOL Roll( HE_INT32 n, HE_INT32 j )
 	{
		PSFuncItem item;
		HE_INT32 i;

		if ( IsOverflow( n ) || j == 0 || n == 0 )
		{
			return FALSE;
		}

		if ( j > 0 )
		{
			j %= n;
		}else{
			j = -j % n;
			if ( j != 0 )
			{
				j = n - j;
			}
		}

		for ( i = 0; i < j; i++ )
		{
			item = mStack[mStackTopIndex-1];
			memmove( mStack + mStackTopIndex - n + 1, mStack + mStackTopIndex - n, n * sizeof(PSFuncItem) );
			mStack[mStackTopIndex-n] = item;
		}

		return TRUE;
	}

	HE_BOOL Index( HE_INT32 n )
	{
		if (!IsOverflow() && !IsUnderflow(n) )
		{
			mStack[mStackTopIndex] = mStack[mStackTopIndex - n - 1];
			mStackTopIndex++;
			return TRUE;
		}
		return FALSE;
	}

	PSFuncItem		mStack[100];
	HE_INT32		mStackTopIndex;
};

class CHE_PDF_Function_PostScript : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_PostScript();

	HE_BOOL Calculate( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output );

private:
	CHE_PDF_Function_PostScript( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator );

	HE_BOOL IsParsed() const;
	
	HE_VOID Parse();

	HE_VOID ParseImp( CHE_PDF_SyntaxParser & syntaxParser );

	HE_BOOL RunCode( std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output );

	HE_BOOL RunCodeImp( PSFuncStack & stack, HE_ULONG codeIndex );

	HE_BOOL						mbParsed;
	CHE_PDF_StreamPtr			mStmPtr;
	std::vector<PSFuncItem>		mCodes;

	friend class CHE_Allocator;
};

#endif