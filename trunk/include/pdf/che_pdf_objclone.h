#ifndef _CHE_PDF_OBJCLONE_H_
#define _CHE_PDF_OBJCLONE_H_

#include "../../include/pdf/che_pdf_file.h"

#include <unordered_map>

class IHE_ObjectCloneMgr
{
public:
	virtual bool IsCloned( PDF_RefInfo OriNunbers ) = 0;
	virtual bool GetMapObjNum(PDF_RefInfo OriObjNumbers, PDF_RefInfo & infoRet) = 0;
	virtual void SetMap( PDF_RefInfo OriObjNumbers, PDF_RefInfo NewObjNumbers ) = 0;
	virtual void	Reset() = 0;
};

class CHE_ObjectCloneMgr : public IHE_ObjectCloneMgr
{
public:
	bool IsCloned( PDF_RefInfo OriObjNumbers )
	{
		std::unordered_map<size_t,size_t>::const_iterator it = mObjMap.find(OriObjNumbers.objNum);
		return it != mObjMap.cend();
	}

	bool GetMapObjNum( PDF_RefInfo OriObjNumbers, PDF_RefInfo & infoRet )
	{
		std::unordered_map<size_t, size_t>::const_iterator it = mObjMap.find(OriObjNumbers.objNum);
		if ( it != mObjMap.cend() )
		{
			infoRet.objNum = it->second;
			infoRet.genNum = 0;
			return true;
		}
		return false;
	}

	void	SetMap( PDF_RefInfo OriObjNumers, PDF_RefInfo NewObjNumbers )
	{
		mObjMap.insert(make_pair(OriObjNumers.objNum, NewObjNumbers.objNum));
	}

	void	Reset()
	{
		mObjMap.clear();
	}

	std::unordered_map<size_t,size_t> mObjMap;
};

CHE_PDF_DictionaryPtr	CloneDirectDictObj( const CHE_PDF_DictionaryPtr & DictPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = nullptr );

CHE_PDF_ArrayPtr		CloneDirectArrayObj( const CHE_PDF_ArrayPtr & ArrayPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = nullptr );

PDF_RefInfo				CloneIndirectObj( const CHE_PDF_ReferencePtr & RefPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = nullptr );

#endif