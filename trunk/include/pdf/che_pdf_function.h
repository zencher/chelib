#ifndef _CHE_PDF_FUNCTION_H_
#define _CHE_PDF_FUNCTION_H_

enum PDF_FUNCTION_TYPE
{
	FUNCTION_TYPE_SAMPLE		= 0,
	FUNCTION_TYPE_EXPONENTIAL	= 2,
	FUNCTION_TYPE_STITCHING		= 3,
	FUNCTION_TYPE_POSTSCRIPT	= 4
};

// enum PDF_FUNCTION_POSTSCRIPT_KEYWORD
// {
// 	POSTSCRIPT_abs,
// 	POSTSCRIPT_add
// 	POSTSCRIPT_atan
// 	POSTSCRIPT_ceiling
// 	POSTSCRIPT_cos
// 	POSTSCRIPT_cvi
// 	POSTSCRIPT_cvr
// 	POSTSCRIPT_div
// 	POSTSCRIPT_exp
// 	POSTSCRIPT_floor
// 	POSTSCRIPT_idiv
// 	ln
// 	log
// 	mod
// 	mul
// 	neg
// 	round
// 	sin
// 	sqrt
// 	sub
// 	truncate
// 	and
// 	bitshift
// 	eq
// 	false
// 	ge
// 	gt
// 	le 
// 	lt
// 	ne
// 	not
// 	or
// 	true
// 	xor
// 	if
// 	ifelse
// 	copy
// 	dup
// 	exch
// 	index
// 	pop
// 	roll
// };

class CHE_PDF_Function : public CHE_Object
{
public:
	static CHE_PDF_Function *	Create( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator = NULL );
	static HE_VOID				Destroy( CHE_PDF_Function * pFunction );

	~CHE_PDF_Function();

	PDF_FUNCTION_TYPE			GetType() const { return mType; }
	HE_UINT32					GetInputCount() const { return mInputCount; } 
	HE_UINT32					GetOutputCount() const { return mInputCount; }
	HE_FLOAT					GetDomianMin( HE_UINT32 index ) const;
	HE_FLOAT					GetDomianMax( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMin( HE_UINT32 index ) const;
	HE_FLOAT					GetRangeMax( HE_UINT32 index ) const;

protected:
	CHE_PDF_Function( CHE_PDF_DictionaryPtr dict, CHE_Allocator * pAllocator );

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

private:
	CHE_PDF_Function_Sampled( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator );

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

private:
	CHE_PDF_Function_Exponential( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator );

	HE_INT32	mN;
	HE_FLOAT*	mpC0;
	HE_FLOAT*	mpC1;

	friend class CHE_Allocator;
};

class CHE_PDF_Function_Stitching : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_Stitching();

private:
	CHE_PDF_Function_Stitching( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator );

	HE_UINT32			mK;
	HE_FLOAT *			mpBounds;
	HE_FLOAT *			mpEncode;
	CHE_PDF_Function *	mpFunctions;

	friend class CHE_Allocator;
};

class CHE_PDF_Function_PostScript : public CHE_PDF_Function
{
public:
	~CHE_PDF_Function_PostScript( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator );

private:
	CHE_PDF_Function_PostScript( CHE_PDF_StreamPtr stmPtr, CHE_Allocator * pAllocator );

	friend class CHE_Allocator;
};

#endif