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
    std::vector<HE_FLOAT> mSample;
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
    
    //HE_INT32 m_nOrigOutputs;
    //HE_FLOAT m_Exponent;
    //HE_FLOAT* m_pBeginValues;
    //HE_FLOAT* m_pEndValues;

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

enum PDF_PSOP {
    PSOP_ADD,
    PSOP_SUB,
    PSOP_MUL,
    PSOP_DIV,
    PSOP_IDIV,
    PSOP_MOD,
    PSOP_NEG,
    PSOP_ABS,
    PSOP_CEILING,
    PSOP_FLOOR,
    PSOP_ROUND,
    PSOP_TRUNCATE,
    PSOP_SQRT,
    PSOP_SIN,
    PSOP_COS,
    PSOP_ATAN,
    PSOP_EXP,
    PSOP_LN,
    PSOP_LOG,
    PSOP_CVI,
    PSOP_CVR,
    PSOP_EQ,
    PSOP_NE,
    PSOP_GT,
    PSOP_GE,
    PSOP_LT,
    PSOP_LE,
    PSOP_AND,
    PSOP_OR,
    PSOP_XOR,
    PSOP_NOT,
    PSOP_BITSHIFT,
    PSOP_TRUE,
    PSOP_FALSE,
    PSOP_IF,
    PSOP_IFELSE,
    PSOP_POP,
    PSOP_EXCH,
    PSOP_DUP,
    PSOP_COPY,
    PSOP_INDEX,
    PSOP_ROLL,
    PSOP_PROC,
    PSOP_CONST
};

const HE_UINT32 PSENGINE_STACKSIZE = 100;
const HE_FLOAT RADIAN = 57.2957795;

class CHE_PDF_PSEngine;
class CHE_PDF_PSProc;

class CHE_PDF_PSOP {
public:
    explicit CHE_PDF_PSOP(PDF_PSOP op) : m_op(op), m_value(0) {}
    explicit CHE_PDF_PSOP(HE_FLOAT value) : m_op(PSOP_CONST), m_value(value) {}
    explicit CHE_PDF_PSOP(std::unique_ptr<CHE_PDF_PSProc> proc)
        : m_op(PSOP_PROC), m_value(0), m_proc(std::move(proc)) {}

    HE_FLOAT GetFloatValue() const {
        if (m_op == PSOP_CONST)
            return m_value;
        return 0;
    }

    CHE_PDF_PSProc* GetProc() const {
        if (m_op == PSOP_PROC)
            return m_proc.get();
        return nullptr;
    }

    PDF_PSOP GetOp() const { return m_op; }

private:
    const PDF_PSOP m_op;
    const HE_FLOAT m_value;
    std::unique_ptr<CHE_PDF_PSProc> m_proc;
};

class CHE_PDF_PSProc {
public:
    CHE_PDF_PSProc() {}
    ~CHE_PDF_PSProc() {}

    HE_BOOL Parse(CHE_PDF_SyntaxParser* parser);
    HE_BOOL Execute(CHE_PDF_PSEngine* pEngine);

private:
    std::vector<std::unique_ptr<CHE_PDF_PSOP>> m_Operators;
};



class CHE_PDF_PSEngine {
public:
    CHE_PDF_PSEngine();
    ~CHE_PDF_PSEngine();

    HE_BOOL Parse(const HE_BYTE* str, HE_ULONG size);
    HE_BOOL Execute() { return m_MainProc.Execute(this); }
    HE_BOOL DoOperator(PDF_PSOP op);
    void Reset() { m_StackCount = 0; }
    void Push(HE_FLOAT value);
    void Push(int value) { Push((HE_FLOAT)value); }
    HE_FLOAT Pop();
    HE_UINT32 GetStackSize() const { return m_StackCount; }

private:
    HE_FLOAT m_Stack[PSENGINE_STACKSIZE];
    HE_UINT32 m_StackCount;
    CHE_PDF_PSProc m_MainProc;
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

	HE_BOOL RunCode(const std::vector<HE_FLOAT> & input, std::vector<HE_FLOAT> & output);

	HE_BOOL						mbParsed;
	CHE_PDF_StreamPtr			mStmPtr;
    CHE_PDF_PSEngine            mPSEngine;

	friend class CHE_Allocator;
};

#endif