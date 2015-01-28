#ifndef _CHE_PDF_OBJCLONE_H_
#define _CHE_PDF_OBJCLONE_H_

#include "../../include/pdf/che_pdf_file.h"

#include <unordered_map>

class IHE_ObjectCloneMgr
{
public:
	virtual HE_BOOL IsCloned( PDF_RefInfo OriNunbers ) = 0;
	virtual HE_BOOL GetMapObjNum(PDF_RefInfo OriObjNumbers, PDF_RefInfo & infoRet) = 0;
	virtual HE_VOID SetMap( PDF_RefInfo OriObjNumbers, PDF_RefInfo NewObjNumbers ) = 0;
	virtual HE_VOID	Reset() = 0;
};

class CHE_ObjectCloneMgr : public IHE_ObjectCloneMgr
{
public:
	HE_BOOL IsCloned( PDF_RefInfo OriObjNumbers )
	{
		std::unordered_map<HE_ULONG,HE_ULONG>::const_iterator it = mObjMap.find(OriObjNumbers.objNum);
		return it != mObjMap.cend();
	}

	HE_BOOL GetMapObjNum( PDF_RefInfo OriObjNumbers, PDF_RefInfo & infoRet )
	{
		std::unordered_map<HE_ULONG, HE_ULONG>::const_iterator it = mObjMap.find(OriObjNumbers.objNum);
		if ( it != mObjMap.cend() )
		{
			infoRet.objNum = it->second;
			infoRet.genNum = 0;
			return true;
		}
		return false;
	}

	HE_VOID	SetMap( PDF_RefInfo OriObjNumers, PDF_RefInfo NewObjNumbers )
	{
		mObjMap.insert(make_pair(OriObjNumers.objNum, NewObjNumbers.objNum));
	}

	HE_VOID	Reset()
	{
		mObjMap.clear();
	}

	std::unordered_map<HE_ULONG,HE_ULONG> mObjMap;
};

CHE_PDF_DictionaryPtr	CloneDirectDictObj( const CHE_PDF_DictionaryPtr & DictPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );

CHE_PDF_ArrayPtr		CloneDirectArrayObj( const CHE_PDF_ArrayPtr & ArrayPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );

PDF_RefInfo				CloneIndirectObj( const CHE_PDF_ReferencePtr & RefPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );

#endif