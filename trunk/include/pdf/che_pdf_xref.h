 #ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"

#define PDF_XREF_ENTRY_INUSE	'n'
#define PDF_XREF_ENTRY_FREE		'f'

#define OBJTYPE_COMMON			0
#define OBJTYPE_COMPRESSED		1

class CHE_PDF_XREF_Entry
{
public:
	CHE_PDF_XREF_Entry();
	CHE_PDF_XREF_Entry( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );
	CHE_PDF_XREF_Entry( unsigned int objNum, unsigned int parentObjNum, unsigned int index );

	bool operator == ( const CHE_PDF_XREF_Entry & entry );

	void SetType( unsigned char type ) { m_type = type; }
	void SetFlag( unsigned char flag ) { m_byteFlag = flag; }
	void SetGenNum( unsigned int genNum ) { m_iGenNum = genNum; }
	void SetOffset( unsigned int offset ) { m_iByteOffset = offset; }
	void SetObjNum( unsigned int objNum ) { m_iObjNum = objNum; }
	void SetParentObjNum( unsigned int parentObjNum ) { m_iParentObjNum = parentObjNum; }
	void SetIndex( unsigned int index ) { m_iIndex = index; }

	unsigned char	GetType() { return m_type; }
	unsigned char	GetFlag() { return m_byteFlag; }
	unsigned short	GetGenNum() { return m_iGenNum; }
	unsigned int	GetOffset() { return m_iByteOffset; }
	unsigned int	GetObjNum() { return m_iObjNum; }
	unsigned int	GetParentObjNum() { return m_iParentObjNum; }
	unsigned int	GetIndex() { return m_iIndex; }	

private:
	unsigned char	m_type;	// 0 - OBJTYPE_COMMON, 1 - OBJTYPE_COMPRESSED
	unsigned char	m_byteFlag;
	unsigned short	m_iGenNum;
	unsigned int	m_iByteOffset;
	unsigned int	m_iObjNum;
	unsigned int	m_iParentObjNum;
	unsigned int	m_iIndex;
};

struct PDF_XREF_ENTRY_NODE
{
	CHE_PDF_XREF_Entry entry;
	PDF_XREF_ENTRY_NODE * pNext;
};

struct PDF_XREF_TABLE_BLOCK
{
	HE_DWORD lBeginNum;
	PDF_XREF_ENTRY_NODE* pNodePtrArr[4096];
	PDF_XREF_TABLE_BLOCK * pNext;
};


class CHE_PDF_XREF_Table
{
public:
	CHE_PDF_XREF_Table();
	//CHE_PDF_XREF_Table( const CHE_PDF_XREF_Table & table );
	~CHE_PDF_XREF_Table();

	//const CHE_PDF_XREF_Table & operator = ( const CHE_PDF_XREF_Table & table );
	
	void Clear();
                                         
	//void Clone( const CHE_PDF_XREF_Table & table );

	//bool Append( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );
	//bool Append( unsigned int objNum, unsigned int index );
	bool Append( CHE_PDF_XREF_Entry & entry );

	//bool Update( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );
	//bool Update( unsigned int objNum, unsigned int index );
	bool Update( CHE_PDF_XREF_Entry & entry );

	bool Remove( unsigned int objNum );

	bool GetEntry( unsigned int objNum, CHE_PDF_XREF_Entry & entryRet );

	bool IsExist( unsigned int objNum );

private:
	PDF_XREF_ENTRY_NODE *	m_pFirstEntry;
	PDF_XREF_ENTRY_NODE *	m_pLastEntry;
	HE_DWORD				m_lCount;
};

#endif