#ifndef _CHE_PDF_FILTER_H_
#define _CHE_PDF_FILTER_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

class CHE_PDF_Filter : public CHE_Object
{
public:
	CHE_PDF_Filter( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator ) {}
	virtual	~CHE_PDF_Filter() {};

	virtual void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer ) = 0;
	virtual void	Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer ) = 0;
};


static inline int getcomponent(unsigned char *line, int x, int bpc)
{
	switch (bpc)
	{
	case 1: return (line[x >> 3] >> ( 7 - (x & 7) ) ) & 1;
	case 2: return (line[x >> 2] >> ( ( 3 - (x & 3) ) << 1 ) ) & 3;
	case 4: return (line[x >> 1] >> ( ( 1 - (x & 1) ) << 2 ) ) & 15;
	case 8: return line[x];
	case 16: return (line[x<<1]<<8)+line[(x<<1)+1];
	}
	return 0;
}

static inline void putcomponent(unsigned char *buf, int x, int bpc, int value)
{
	switch (bpc)
	{
	case 1: buf[x >> 3] |= value << (7 - (x & 7)); break;
	case 2: buf[x >> 2] |= value << ((3 - (x & 3)) << 1); break;
	case 4: buf[x >> 1] |= value << ((1 - (x & 1)) << 2); break;
	case 8: buf[x] = value; break;
	case 16: buf[x<<1] = value>>8; buf[(x<<1)+1] = value; break;
	}
}


static inline int fz_absi(int i)
{
	return (i < 0 ? -i : i);
}

static inline int paeth( int a, int b, int c )
{
	/* The definitions of ac and bc are correct, not a typo. */
	int ac = b - c, bc = a - c, abcc = ac + bc;
	int pa = fz_absi(ac);
	int pb = fz_absi(bc);
	int pc = fz_absi(abcc);
	return pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
}


class CHE_PDF_Predictor : public CHE_Object
{
public:
	CHE_PDF_Predictor( CHE_PDF_DictionaryPtr & dictPtr, CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mPredictor(1), mBpc(8), mBpp((8+7)/8), mEarlyChange(1),
		mColors(1), mColumns(1), mStride((8+7)/8), mpOutput(nullptr), mpRef(nullptr)
	{
		if ( dictPtr )
		{
			CHE_PDF_ObjectPtr pObj;
			pObj = dictPtr->GetElement( "Predictor" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{	
				mPredictor = pObj->GetNumberPtr()->GetInteger();
			}
			pObj = dictPtr->GetElement( "Colors" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				mColors = pObj->GetNumberPtr()->GetInteger();
			}
			pObj = dictPtr->GetElement( "BitsPerComponent" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				mBpc = pObj->GetNumberPtr()->GetInteger();
			}
			pObj = dictPtr->GetElement( "Columns" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				mColumns = pObj->GetNumberPtr()->GetInteger();
			}
			pObj = dictPtr->GetElement( "EarlyChange" );
			if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
			{
				mEarlyChange = pObj->GetNumberPtr()->GetInteger();
			}
			mBpp = ((mBpc * mColors + 7) / 8);
			mStride = ((mBpc * mColors * mColumns + 7) / 8);
			mpOutput = GetAllocator()->NewArray<BYTE>( mStride + 1 );
			memset( mpOutput, 0, mStride + 1 );
			mpRef = GetAllocator()->NewArray<BYTE>( mStride + 1 );
			memset( mpRef, 0, mStride + 1 );
		}
	}

    CHE_PDF_Predictor(	BYTE Predictor = 1, BYTE Colors = 1, BYTE BitsPerComponent = 8,
						uint32 Columns = 1, BYTE EarlyChange = 1, CHE_Allocator * pAllocator = nullptr )
		: CHE_Object( pAllocator ), mPredictor(Predictor), mBpc(BitsPerComponent), mBpp((BitsPerComponent * Colors + 7) / 8), 
		mEarlyChange(EarlyChange), mColors(Colors), mColumns(Columns), mStride((BitsPerComponent * Colors * Columns + 7) / 8), 
		mpOutput(nullptr), mpRef(nullptr)
	{
		mpOutput = GetAllocator()->NewArray<BYTE>( mStride + 1 );
		mpRef = GetAllocator()->NewArray<BYTE>( mStride + 1 );
	}

    ~CHE_PDF_Predictor()
    {
		if ( mpOutput )
		{
			GetAllocator()->DeleteArray( mpOutput );
		}
		if ( mpRef )
		{
			GetAllocator()->DeleteArray( mpRef );
		}
    }

	void PredirectTiff( PBYTE pData, CHE_DynBuffer & buffer )
	{
		int left[32];
		int i, k;
		const int mask = (1<<mBpc)-1;

		for (k = 0; k < mColors; k++)
		{
			left[k] = 0;
		}
		memset( mpOutput, 0, mStride );

		for (i = 0; i < mColumns; i++)
		{
			for (k = 0; k < mColors; k++)
			{
				int a = getcomponent( pData, i * mColors + k, mBpc );
				int b = a + left[k];
				int c = b & mask;
				putcomponent( mpOutput, i * mColors + k, mBpc, c );
				left[k] = c;
			}
		}
	}

	void PredirectPng( PBYTE pData, CHE_DynBuffer & buffer, BYTE predictor )
	{
		size_t i = 0;
		PBYTE p = pData;
		PBYTE pOut = mpOutput;
		PBYTE pRef = mpRef;
		switch ( predictor )
		{
		case 0:
			{
				memcpy( mpOutput, p, mStride );
				buffer.Write( mpOutput, mStride );
				break;
			}
		case 1:
			{
				for ( i = mBpp; i > 0; i-- )
				{
					*pOut = *p;
					++pOut;
					++p;
				}
				for ( i = mStride - mBpp; i > 0; i-- )
				{
					*pOut = *p + pOut[-mBpp];
					++pOut;
					++p;
				}
				buffer.Write( mpOutput, mStride );
				break;
			}
		case 2:
			{
				for ( i = mBpp; i > 0; i-- )
				{
					*pOut = *p + *pRef;
					++p;
					++pOut;
					++pRef;
				}
				for ( i = mStride - mBpp; i > 0; i-- )
				{
					*pOut = *p + *pRef;
					++p;
					++pOut;
					++pRef;
				}
				buffer.Write( mpOutput, mStride );
				break;
			}
		case 3:
			{
				for ( i = mBpp; i > 0; i-- )
				{
					*pOut = *p + *pRef/2;
					++p;
					++pOut;
					++pRef;
				}
				for ( i = mStride - mBpp; i > 0; i-- )
				{
					*pOut = *p + (pOut[-mBpp] + *pRef)/2;
					++p;
					++pOut;
					++pRef;
				}
				buffer.Write( mpOutput, mStride );
				break;
			}
		case 4:
			{
				for ( i = mBpp; i > 0; i-- )
				{
					*pOut = *p + paeth(0, *pRef, 0);
					++p;
					++pOut;
					++pRef;
				}
				for ( i = mStride - mBpp; i > 0; i -- )
				{
					*pOut = *p + paeth(pOut[-mBpp], *pRef, pRef[-mBpp]);
					++p;
					++pOut;
					++pRef;
				}
				buffer.Write( mpOutput, mStride );
				break;
			}
		}

		memcpy( mpRef, mpOutput, mStride );
	}

    void Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer ) 
    {
		if ( pData == nullptr || length == 0 )
		{
			return;
		}
		if ( mPredictor != 1 && mPredictor != 2 &&
			 mPredictor != 10 && mPredictor != 11 &&
			 mPredictor != 12 && mPredictor != 13 &&
			 mPredictor != 14 && mPredictor != 15 )
		{
			return;
		}
		PBYTE p	 = pData;
		PBYTE ep = pData + length;
		while ( p < ep )
		{
			if ( mPredictor == 1 )
			{
				buffer.Write( p, mStride );
				p += mStride;
			}
			else if ( mPredictor == 2)
			{
				PredirectTiff( p, buffer );
				p += mStride;
			}
			else
			{
				PredirectPng( p+1, buffer, *p );
				p += mStride + 1;
			}
		}
    }

private:
    BYTE mPredictor;
    BYTE mBpc;
	BYTE mBpp;
	BYTE mEarlyChange;
	uint32 mColors;
    uint32 mColumns;
	uint32 mStride;
	PBYTE mpOutput;
	PBYTE mpRef;
};

class CHE_PDF_HexFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_HexFilter( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_HexFilter() {};
	
	void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
	void	Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
};

class CHE_PDF_ASCII85Filter : public CHE_PDF_Filter
{
public:
	CHE_PDF_ASCII85Filter( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_ASCII85Filter() {};

	void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

	void Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

private:
	void EncodeTuple( size_t tuple, size_t count,  CHE_DynBuffer & buffer );

	void WidePut( size_t tuple, size_t bytes,  CHE_DynBuffer & buffer );

	size_t	m_count;
    size_t	m_tuple;
};

class CHE_PDF_RLEFileter : public CHE_PDF_Filter
{
public:
	CHE_PDF_RLEFileter( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_RLEFileter() {};

	void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

	void Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
};

class CHE_PDF_FlateFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_FlateFilter( CHE_Allocator * pAllocator = nullptr) : CHE_PDF_Filter( pAllocator ) {}
	
	~CHE_PDF_FlateFilter() {}

	void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
	
	void Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
};

struct TLzwItem {
	std::vector<unsigned char> value;
};
typedef std::vector<TLzwItem>     TLzwTable;
typedef TLzwTable::iterator       TILzwTable;
typedef TLzwTable::const_iterator TCILzwTable;

class CHE_PDF_LZWFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_LZWFilter( CHE_Allocator * pAllocator = nullptr ) : CHE_PDF_Filter( pAllocator ) {}

	~CHE_PDF_LZWFilter() {};

	void Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
	
	void Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
	
private:
	void InitTable();

    static const unsigned short s_masks[4];
    static const unsigned short s_clear;
    static const unsigned short s_eod;
	
    TLzwTable     m_table;
	
    unsigned int  m_mask;
    unsigned int  m_code_len;
    unsigned char m_character;
	
    bool          m_bFirst;
};

class CHE_PDF_FaxDecodeParams /*: public CHE_Object*/
{
public:
	CHE_PDF_FaxDecodeParams( CHE_PDF_DictionaryPtr dictPtr/*, CHE_Allocator * pAllocator = nullptr*/ );
	//~CHE_PDF_FaxDecodeParams();

	int32		k;
	int32		columns;
	int32		rows;
	bool			eol;
	bool			eba;
	bool			eob;
	bool			bi1;
};

class CHE_PDF_FaxFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_FaxFilter( CHE_PDF_FaxDecodeParams * pParams = nullptr, CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_FaxFilter();

	void		Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

	void		Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

private:
	CHE_PDF_FaxDecodeParams * mpParams;
};

class CHE_PDF_DCTDFilter : public CHE_PDF_Filter
{
public:
    CHE_PDF_DCTDFilter( CHE_Allocator * pAllocator = nullptr )
        : CHE_PDF_Filter(pAllocator) {};
    
    ~CHE_PDF_DCTDFilter() {};
    
    void     Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer ) {};
    
    void     Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
};

class CHE_PDF_JPXFilter : public CHE_PDF_Filter
{
public:
    CHE_PDF_JPXFilter( CHE_Allocator * pAllocator = nullptr );
    
    ~CHE_PDF_JPXFilter();
    
    void		Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
    
	void		Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );
};

class CHE_PDF_JBig2Filter : public CHE_PDF_Filter
{
public:
	CHE_PDF_JBig2Filter( CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_JBig2Filter();

	void		SetGlobals( PBYTE pData, size_t length );

	void		Encode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

	void		Decode( PBYTE pData, size_t length, CHE_DynBuffer & buffer );

private:
	PBYTE	mGlobalsParam;
	size_t	mGlobalsParamLength;
};

#endif
