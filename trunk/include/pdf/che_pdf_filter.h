#ifndef _CHE_PDF_FILTER_H_
#define _CHE_PDF_FILTER_H_

#include "../che_base.h"
#include "../che_datastructure.h"

class CHE_PDF_Filter : public CHE_Object
{
public:
	CHE_PDF_Filter( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}
	virtual	~CHE_PDF_Filter() {};

	virtual HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer ) = 0;
	virtual HE_VOID	Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer ) = 0;
};

class CHE_PDF_Predictor : public CHE_Object
{
public:
    CHE_PDF_Predictor(	HE_BYTE Predictor = 1, HE_BYTE Colors = 1, HE_BYTE BitsPerComponent = 8, HE_BYTE Columns = 8, HE_BYTE EarlyChange = 1, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
	{
        m_nPredictor   = Predictor;
        m_nColors      = 1;
        m_nBPC         = BitsPerComponent;
        m_nColumns     = Columns;
        m_nEarlyChange = EarlyChange;
        m_nCurPredictor = 255;
        m_nCurRowIndex  = 0;
        m_nBpp  = (m_nBPC * m_nColors) >> 3;
        m_nRows = (m_nColumns * m_nColors * m_nBPC) >> 3;
        m_pPrev = GetAllocator()->NewArray<HE_BYTE>( m_nRows );
        if( !m_pPrev )
        {
			throw 0;
        }
        memset( m_pPrev, 0, m_nRows );
    };

    ~CHE_PDF_Predictor()
    {
        if ( m_pPrev )
        {
			GetAllocator()->DeleteArray<HE_BYTE>( m_pPrev );
        }
    }

    void Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer ) 
    {
        if( m_nPredictor == 1 )
        {
			buffer.Write( pData, length );
            return;
        }
        if( m_nCurPredictor == 255 ) 
        {
            m_nCurPredictor = m_nPredictor >= 10 ? *pData + 10 : *pData;
            m_nCurRowIndex  = 0;
            ++pData;
            --length;
        }
        while( length-- ) 
        {
            if( m_nCurRowIndex >= m_nRows ) 
            {
                m_nCurRowIndex  = 0;
                m_nCurPredictor = m_nPredictor >= 10 ? *pData + 10 : *pData;
				buffer.Write( m_pPrev, m_nRows );
            }else{
                switch( m_nCurPredictor )
                {
                    case 2: // Tiff Predictor
                        throw 0;
                        break;
                    case 10: // png none
                    {
                        m_pPrev[m_nCurRowIndex] = *pData;
                        break;
                    }
                    case 11: // png sub
                    {
                        HE_INT32 prev = ( m_nCurRowIndex - m_nBpp < 0 ? 0 : m_pPrev[m_nCurRowIndex - m_nBpp] );
                        m_pPrev[m_nCurRowIndex] = *pData + prev;
                        break;
                    }
                    case 12: // png up
                    {
                        m_pPrev[m_nCurRowIndex] += *pData;
                        break;
                    }
                    case 13: // png average
                    {
                        int prev = ( m_nCurRowIndex - m_nBpp < 0 ? 0 : m_pPrev[m_nCurRowIndex - m_nBpp] );
                        m_pPrev[m_nCurRowIndex] = ((prev + m_pPrev[m_nCurRowIndex]) >> 1) + *pData;
                        break;
                    }
                    case 14: // png paeth
                    case 15: // png optimum
						throw 0;
                        break;
                        
                    default:
                    {
                        break;
                    }
                }
                ++m_nCurRowIndex;
            }
            ++pData;
        }
        buffer.Write( m_pPrev, m_nRows );
    }
private:
    HE_BYTE		m_nPredictor;
    HE_BYTE		m_nColors;
    HE_BYTE		m_nBPC; //< Bytes per component
    HE_BYTE		m_nColumns;
    HE_BYTE		m_nEarlyChange;
    HE_BYTE		m_nBpp; ///< Bytes per pixel
    HE_BYTE		m_nCurPredictor;
    HE_ULONG	m_nCurRowIndex;
    HE_ULONG	m_nRows;
    HE_LPBYTE	m_pPrev;
};

class CHE_PDF_HexFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_HexFilter( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_HexFilter() {};
	
	HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
	HE_VOID	Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
};

class CHE_PDF_ASCII85Filter : public CHE_PDF_Filter
{
public:
	CHE_PDF_ASCII85Filter( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_ASCII85Filter() {};

	HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );

	HE_VOID Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );

private:
	HE_VOID EncodeTuple( HE_ULONG tuple, HE_ULONG count,  CHE_DynBuffer & buffer );

	HE_VOID WidePut( HE_ULONG tuple, HE_ULONG bytes,  CHE_DynBuffer & buffer );

	HE_ULONG	m_count;
    HE_ULONG	m_tuple;
};

class CHE_PDF_FlateFilter : public CHE_PDF_Filter
{
public:
	CHE_PDF_FlateFilter( CHE_PDF_Predictor * pPredictor = NULL, CHE_Allocator * pAllocator = NULL) : CHE_PDF_Filter( pAllocator )
	{ m_pPredictor = pPredictor; }
	
	~CHE_PDF_FlateFilter() {}

	HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );

private:
	CHE_PDF_Predictor * m_pPredictor;
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
	CHE_PDF_LZWFilter( CHE_PDF_Predictor * pPredictor = NULL, CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Filter( pAllocator )
	{ m_pPredictor = pPredictor; }

	~CHE_PDF_LZWFilter() {};

	HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
	
private:
	HE_VOID InitTable();

    static const unsigned short s_masks[4];
    static const unsigned short s_clear;
    static const unsigned short s_eod;
	
    TLzwTable     m_table;
	
    unsigned int  m_mask;
    unsigned int  m_code_len;
    unsigned char m_character;
	
    bool          m_bFirst;

	CHE_PDF_Predictor * m_pPredictor;
};

class CHE_PDF_RLEFileter : public CHE_PDF_Filter
{
public:
	CHE_PDF_RLEFileter( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Filter( pAllocator ) {}
	~CHE_PDF_RLEFileter() {};
	
	HE_VOID Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer );
};

#endif
