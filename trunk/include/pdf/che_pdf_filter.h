#ifndef _CHE_PDF_FILTER_H_
#define _CHE_PDF_FILTER_H_

#include "../che_base.h"
#include "../che_dynBuffer.h"
#include <vector>

class CHE_PDF_Filter
{
public:
	virtual	~CHE_PDF_Filter() {};

	virtual HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer ) = 0;
	virtual HE_VOID	Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer ) = 0;
};

class CHE_PDF_HexFilter : public CHE_PDF_Filter
{
public:
	~CHE_PDF_HexFilter() {};
	
	HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
	HE_VOID	Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
};

class CHE_PDF_ASCII85Filter : public CHE_PDF_Filter
{
public:
	~CHE_PDF_ASCII85Filter() {};

	HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );

	HE_VOID Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );

private:
	HE_VOID EncodeTuple( HE_DWORD tuple, HE_DWORD count,  CHE_DynBuffer & buffer );

	HE_VOID WidePut( HE_DWORD tuple, HE_DWORD bytes,  CHE_DynBuffer & buffer );

	HE_DWORD	m_count;
    HE_DWORD	m_tuple;
};

class CHE_PDF_FlateFilter : public CHE_PDF_Filter
{
public:
	~CHE_PDF_FlateFilter() {};

	HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
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
	~CHE_PDF_LZWFilter() {};

	HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
	
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
};

class CHE_PDF_RLEFileter : public CHE_PDF_Filter
{
public:
	~CHE_PDF_RLEFileter() {};
	
	HE_VOID Encode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
	
	HE_VOID Decode( HE_LPBYTE pData, HE_DWORD length, CHE_DynBuffer & buffer );
};

#endif