#ifndef _CHE_PDF_FUNCTION_H_
#define _CHE_PDF_FUNCTION_H_

#include "che_pdf_component.h"
#include "che_pdf_parser.h"

#include <vector>
#include <stack>


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
	static CHE_PDF_FunctionPtr	Create(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator = NULL);

	virtual ~CHE_PDF_Function() {};

	PDF_FUNCTION_TYPE			GetFunctionType() const { return mFunctionType; }
	HE_UINT32					GetInputCount() const { return mInputCount; } 
	HE_UINT32					GetOutputCount() const { return mOutputCount; }
	HE_FLOAT					GetDomianMin( HE_UINT32 index ) const;
	HE_FLOAT					GetDomianMax( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMin( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMax( HE_UINT32 index ) const;
	HE_BOOL						HasDomian() const { return mpDomain != NULL; }
	HE_BOOL						HasRange() const { return mpRange != NULL; }

	virtual HE_BOOL				Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output) = 0;

protected:
	CHE_PDF_Function(const CHE_PDF_ObjectPtr & root, CHE_Allocator * pAllocator = NULL);

	PDF_FUNCTION_TYPE			mFunctionType;
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

	HE_BOOL Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

private:
	CHE_PDF_Function_Sampled(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

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

	HE_BOOL Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

private:
	CHE_PDF_Function_Exponential(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

	HE_INT32	mN;
	HE_FLOAT*	mpC0;
	HE_FLOAT*	mpC1;

	friend class CHE_Allocator;
};

class CHE_PDF_Function_Stitching : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Stitching();

	HE_BOOL Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

private:
	CHE_PDF_Function_Stitching(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

	HE_FLOAT GetEncodeMin(HE_UINT32 index) const;
	HE_FLOAT GetEncodeMax(HE_UINT32 index) const;

	HE_UINT32							mK;
	std::vector<HE_FLOAT>				mBounds;
	std::vector<HE_FLOAT>				mEncode;
	std::vector<CHE_PDF_FunctionPtr>	mFunctions;

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
	PSFuncStack() {}

	HE_VOID	Init()
	{
		while ( !mStack.empty() )
		{
			mStack.pop();
		}
	}

	HE_VOID Push(const PSFuncItem & item )
	{
		mStack.push(item);
	}

	HE_VOID PushBool(HE_BOOL val)
	{
		PSFuncItem item;
		item.mType = PSITEM_BOOL;
		item.mBoolValue = val;
		Push(item);
	}

	HE_VOID PushInteger(HE_INT32 val)
	{
		PSFuncItem item;
		item.mType = PSITEM_INT;
		item.mIntegerValue = val;
		Push(item);
	}

	HE_VOID PushFloat( HE_FLOAT val )
	{
		PSFuncItem item;
		item.mType = PSITEM_FLOAT;
		item.mFloatValue = val;
		Push( item );
	}

	HE_BOOL PopItem(PSFuncItem & item)
	{
		if (!mStack.empty())
		{
			item = mStack.top();
			mStack.pop();
			return true;
		}
		return false;
	}

	HE_BOOL PopBool( HE_BOOL & valRet )
	{
		if ( !mStack.empty() )
		{
			PSFuncItem item = mStack.top();
			mStack.pop();
			if (item.mType == PSITEM_BOOL)
			{
				valRet = item.mBoolValue;
				return true;
			}
		}
		return false;
	}

	HE_BOOL PopInteger( HE_INT32 & valRet )
	{
		if (!mStack.empty())
		{
			PSFuncItem item = mStack.top();
			mStack.pop();
			if (item.mType == PSITEM_INT)
			{
				valRet = item.mIntegerValue;
				return true;
			}
		}
		return false;
	}

	HE_BOOL PopFloat( HE_FLOAT & valRet )
	{
		if (!mStack.empty())
		{
			PSFuncItem item = mStack.top();
			mStack.pop();
			if (item.mType == PSITEM_FLOAT)
			{
				valRet = item.mFloatValue;
				return true;
			}
		}
		return false;
	}
    
	HE_BOOL Execute( PSFUNCITEM_OPERATOR op );
	
	stack<PSFuncItem>	mStack;
};

class CHE_PDF_Function_PostScript : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_PostScript();

	HE_BOOL Calculate(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

private:
	CHE_PDF_Function_PostScript(const CHE_PDF_StreamPtr & stm, CHE_Allocator * pAllocator);

	HE_BOOL IsParsed() const;
	
	HE_VOID Parse();

	HE_VOID ParseImp(CHE_PDF_SyntaxParser & syntaxParser);

	HE_BOOL RunCode(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

	HE_BOOL RunCodeImp(PSFuncStack & stack, HE_ULONG codeIndex);

	HE_BOOL						mbParsed;
	CHE_PDF_StreamPtr			mStmPtr;
	std::vector<PSFuncItem>		mCodes;

	friend class CHE_Allocator;
};

#endif