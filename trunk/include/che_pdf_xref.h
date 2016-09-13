#ifndef _CHELIB_PDF_XREF_H_
#define _CHELIB_PDF_XREF_H_

#include "che_pdf_object.h"

#include <vector>
#include <map>

namespace chelib {

enum PDF_XREF_ENTRY_TYPE
{
    XREF_ENTRY_TYPE_FREE		= 0x00,
    XREF_ENTRY_TYPE_COMMON		= 0x01,
    XREF_ENTRY_TYPE_COMPRESSED	= 0x02,
    XREF_ENTRY_TYPE_NEW			= 0x03
};


class CPDF_XRefEntry
{
public:
    CPDF_XRefEntry();
    CPDF_XRefEntry( PDF_XREF_ENTRY_TYPE type, size_t objNum, size_t f1, size_t f2 );

	PDF_XREF_ENTRY_TYPE GetType() const { return Type; }
    
    size_t GetObjNum() const { return ObjNum; }
    size_t GetOffset() const { return Field1; }
    size_t GetGenNum() const { return Field2; }
    size_t GetParentObjNum() const { return Field1; }
    size_t GetIndex() const { return Field2; }

    PDF_XREF_ENTRY_TYPE Type;
    size_t ObjNum;
    size_t Field1;
    size_t Field2;
};


class CPDF_XRefTable : public CObject
{
public:
	CPDF_XRefTable( CAllocator * pAllocator = nullptr );
	~CPDF_XRefTable();

	bool Add( const CPDF_XRefEntry & entry );
	void AddNewEntry( CPDF_XRefEntry & entryRet );
	bool AddTrailerDict( const CPDF_DictionaryPtr & pDict );

	void Clear();

	bool Get( size_t objNum, CPDF_XRefEntry & entryRet );
    
	size_t GetTrailerCount() const { return mTrailerDict.size(); }
	CPDF_DictionaryPtr	GetTrailer( size_t index = 0 ) const;
	
    size_t GetMaxObjNum() const { return mMaxObjNum; }

	bool IsEOF() { return mIt == mMap.end(); }
    
	void MoveFirst() { mIt = mMap.begin(); }
	void MoveNext() { ++mIt; }

	bool GetCurNode( CPDF_XRefEntry & entryRet );
	bool Update( size_t objNum, const CPDF_XRefEntry & entryRet );

private:
	size_t                                  mMaxObjNum;
	std::vector<CPDF_DictionaryPtr>         mTrailerDict;
    std::map<size_t,CPDF_XRefEntry>              mMap;
    std::map<size_t,CPDF_XRefEntry>::iterator    mIt;
};

}//namespace

#endif