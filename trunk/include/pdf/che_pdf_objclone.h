#ifndef _CHE_PDF_OBJCLONE_H_
#define _CHE_PDF_OBJCLONE_H_

#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_define.h"
#include "../../include/pdf/che_pdf_file.h"

class IHE_ObjectCloneMgr
{
public:
	virtual HE_BOOL				IsCloned( PDF_RefInfo OriNunbers ) = 0;
	virtual PDF_RefInfo		GetMapObjNum( PDF_RefInfo OriObjNumbers ) = 0;
	virtual HE_VOID				SetMap( PDF_RefInfo OriObjNumbers, PDF_RefInfo NewObjNumbers ) = 0;
};

class ObjectCloneMgrData
{
public:
	ObjectCloneMgrData()
	{
		mObjNumbers.objNum = 0;
		mObjNumbers.genNum = 0;
		mNewObjNumbers.objNum = 0;
		mNewObjNumbers.genNum = 0;
	}
	PDF_RefInfo mObjNumbers;
	PDF_RefInfo mNewObjNumbers;

	ObjectCloneMgrData & operator = ( const ObjectCloneMgrData data )
	{
		mObjNumbers.objNum = data.mObjNumbers.objNum;
		mObjNumbers.genNum = data.mObjNumbers.genNum;
		mNewObjNumbers.objNum = data.mNewObjNumbers.objNum;
		mNewObjNumbers.objNum = data.mNewObjNumbers.genNum;
		return *this;
	}
};

bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

class CHE_ObjectCloneMgr : public IHE_ObjectCloneMgr
{
public:
	HE_BOOL IsCloned( PDF_RefInfo OriObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers.objNum = OriObjNumbers.objNum;
		data.mObjNumbers.genNum = OriObjNumbers.genNum;
		return mDataList.Find( data );
	}

	PDF_RefInfo GetMapObjNum( PDF_RefInfo OriObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers.objNum = OriObjNumbers.objNum;
		data.mObjNumbers.genNum = OriObjNumbers.genNum;
		mDataList.Find( data );
		return data.mNewObjNumbers;
	}

	HE_VOID	SetMap( PDF_RefInfo OriObjNumers, PDF_RefInfo NewObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers = OriObjNumers;
		data.mNewObjNumbers = NewObjNumbers;
		mDataList.Append( data );   
	}

	CHE_SkipList< ObjectCloneMgrData > mDataList;
};

CHE_PDF_DictionaryPtr	CloneDirectDictObj( const CHE_PDF_DictionaryPtr & pDict, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );
CHE_PDF_ArrayPtr		CloneDirectArrayObj( const CHE_PDF_ArrayPtr & pArray, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );
PDF_RefInfo			CloneIndirectObject( const CHE_PDF_ReferencePtr & pRefObj, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr = NULL );

#endif