#ifndef _CHE_PDF_FUNCTION_H_
#define _CHE_PDF_FUNCTION_H_

#include "che_pdf_component.h"
#include "che_pdf_parser.h"

#include <vector>
#include <stack>
#include <memory>


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
	static CHE_PDF_FunctionPtr	Create(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator = nullptr);

	virtual ~CHE_PDF_Function() {};

	PDF_FUNCTION_TYPE			GetFunctionType() const { return mFunctionType; }
	uint32					GetInputCount() const { return mInputCount; } 
	uint32					GetOutputCount() const { return mOutputCount; }
	FLOAT					GetDomianMin( uint32 index ) const;
	FLOAT					GetDomianMax( uint32 index ) const;
	FLOAT					GetRangeMin( uint32 index ) const;
	FLOAT					GetRangeMax( uint32 index ) const;
	bool						HasDomian() const { return mpDomain != nullptr; }
	bool						HasRange() const { return mpRange != nullptr; }

	virtual bool				Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output) = 0;

protected:
	CHE_PDF_Function(const CHE_PDF_ObjectPtr & root, CHE_Allocator * pAllocator = nullptr);

	PDF_FUNCTION_TYPE			mFunctionType;
	uint32					mInputCount;
	uint32					mOutputCount;
	FLOAT*					mpDomain;
	FLOAT*					mpRange;
	friend class CHE_Allocator;
};


class CHE_PDF_Function_Sampled : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Sampled();

	bool Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output);

private:
	CHE_PDF_Function_Sampled(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

	int32 GetSize( uint32 index ) const;
	FLOAT GetEncodeMin( uint32 index ) const;
	FLOAT GetEncodeMax( uint32 index ) const;
	FLOAT GetDecodeMin( uint32 index ) const;
	FLOAT GetDecodeMax( uint32 index ) const;

	float interpolate_sample(int *scale, int *e0, int *e1, float *efrac, int dim, int idx);

	BYTE		mBps;
	BYTE		mOrder;
	int32*	mpSize;
	FLOAT*	mpEncode;
	FLOAT*	mpDecode;
    std::vector<FLOAT> mSample;
	friend class CHE_Allocator;
};


class CHE_PDF_Function_Exponential : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Exponential();

	bool Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output);

private:
	CHE_PDF_Function_Exponential(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

	int32	mN;
	FLOAT*	mpC0;
	FLOAT*	mpC1;
    
    //int32 m_nOrigOutputs;
    //FLOAT m_Exponent;
    //FLOAT* m_pBeginValues;
    //FLOAT* m_pEndValues;

	friend class CHE_Allocator;
};

class CHE_PDF_Function_Stitching : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Stitching();

	bool Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output);

private:
	CHE_PDF_Function_Stitching(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator);

	FLOAT GetEncodeMin(uint32 index) const;
	FLOAT GetEncodeMax(uint32 index) const;

	uint32							mK;
	std::vector<FLOAT>				mBounds;
	std::vector<FLOAT>				mEncode;
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

const uint32 PSENGINE_STACKSIZE = 100;
const FLOAT RADIAN = 57.2957795;

class CHE_PDF_PSEngine;
class CHE_PDF_PSProc;

class CHE_PDF_PSOP {
public:
    explicit CHE_PDF_PSOP(PDF_PSOP op) : m_op(op), m_value(0) {}
    explicit CHE_PDF_PSOP(FLOAT value) : m_op(PSOP_CONST), m_value(value) {}
    explicit CHE_PDF_PSOP(std::unique_ptr<CHE_PDF_PSProc> proc)
        : m_op(PSOP_PROC), m_value(0), m_proc(std::move(proc)) {}

    FLOAT GetFloatValue() const {
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
    const FLOAT m_value;
    std::unique_ptr<CHE_PDF_PSProc> m_proc;
};

class CHE_PDF_PSProc {
public:
    CHE_PDF_PSProc() {}
    ~CHE_PDF_PSProc() {}

    bool Parse(CHE_PDF_SyntaxParser* parser);
    bool Execute(CHE_PDF_PSEngine* pEngine);

private:
    std::vector<std::unique_ptr<CHE_PDF_PSOP>> m_Operators;
};



class CHE_PDF_PSEngine {
public:
    CHE_PDF_PSEngine();
    ~CHE_PDF_PSEngine();

    bool Parse(const BYTE* str, size_t size);
    bool Execute() { return m_MainProc.Execute(this); }
    bool DoOperator(PDF_PSOP op);
    void Reset() { m_StackCount = 0; }
    void Push(FLOAT value);
    void Push(int value) { Push((FLOAT)value); }
    FLOAT Pop();
    uint32 GetStackSize() const { return m_StackCount; }

private:
    FLOAT m_Stack[PSENGINE_STACKSIZE];
    uint32 m_StackCount;
    CHE_PDF_PSProc m_MainProc;
};



class CHE_PDF_Function_PostScript : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_PostScript();

	bool Calculate(const std::vector<FLOAT> & input, std::vector<FLOAT> & output);

private:
	CHE_PDF_Function_PostScript(const CHE_PDF_StreamPtr & stm, CHE_Allocator * pAllocator);

	bool IsParsed() const;
	
	void Parse();

	bool RunCode(const std::vector<FLOAT> & input, std::vector<FLOAT> & output);

	bool						mbParsed;
	CHE_PDF_StreamPtr			mStmPtr;
    CHE_PDF_PSEngine            mPSEngine;

	friend class CHE_Allocator;
};

#endif