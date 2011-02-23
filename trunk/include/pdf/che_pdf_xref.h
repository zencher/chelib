#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#define PDF_XREF_ENTRY_INUSE	'n'
#define PDF_XREF_ENTRY_FREE		'f'

class CHE_PDF_XREF_Entry
{
public:
	CHE_PDF_XREF_Entry();
	CHE_PDF_XREF_Entry( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );

	void SetOffset( unsigned int offset ) { m_iByteOffset = offset; };
	void SetObjNum( unsigned int objNum ) { m_iObjNum = objNum; };
	void SetGenNum( unsigned int genNum ) { m_iGenNum = genNum; };
	void SetFlag( unsigned char flag ) { m_byteFlag = flag; };

	unsigned int GetOffset() { return m_iByteOffset; };
	unsigned int GetObjNum() { return m_iObjNum; };
	unsigned int GetGenNum() { return m_iGenNum; };
	unsigned char	GetFlag() { return m_byteFlag; };

private:
	unsigned int	m_iByteOffset;
	unsigned int	m_iObjNum;
	unsigned short	m_iGenNum;
	unsigned char	m_byteFlag;
};

struct PDF_XREF_ENTRY_NODE
{
	CHE_PDF_XREF_Entry entry;
	PDF_XREF_ENTRY_NODE * pNext;
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

	bool Append( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );
	bool Append( CHE_PDF_XREF_Entry & entry );

	bool Update( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag );
	bool Update( CHE_PDF_XREF_Entry & entry );

	bool Remove( unsigned int objNum );

	bool GetEntry( unsigned int objNum, CHE_PDF_XREF_Entry & entryRet );

	bool IsExist( unsigned int objNum );

private:
	PDF_XREF_ENTRY_NODE * m_pFirstEntry;
	PDF_XREF_ENTRY_NODE * m_pLastEntry;
};

#endif