#ifndef _CHE_PDF_OBJCLONE_H_
#define _CHE_PDF_OBJCLONE_H_

#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_define.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_creator.h"

class IHE_ObjectCloneMgr
{
public:
	virtual HE_BOOL				IsCloned( HE_PDF_RefInfo OriNunbers ) = 0;
	virtual HE_PDF_RefInfo		GetMapObjNum( HE_PDF_RefInfo OriObjNumbers ) = 0;
	virtual HE_VOID				SetMap( HE_PDF_RefInfo OriObjNumbers, HE_PDF_RefInfo NewObjNumbers ) = 0;
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
	HE_PDF_RefInfo mObjNumbers;
	HE_PDF_RefInfo mNewObjNumbers;

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
	HE_BOOL IsCloned( HE_PDF_RefInfo OriObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers.objNum = OriObjNumbers.objNum;
		data.mObjNumbers.genNum = OriObjNumbers.genNum;
		return mDataList.Find( data );
	}

	HE_PDF_RefInfo GetMapObjNum( HE_PDF_RefInfo OriObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers.objNum = OriObjNumbers.objNum;
		data.mObjNumbers.genNum = OriObjNumbers.genNum;
		mDataList.Find( data );
		return data.mNewObjNumbers;
	}

	HE_VOID	SetMap( HE_PDF_RefInfo OriObjNumers, HE_PDF_RefInfo NewObjNumbers )
	{
		ObjectCloneMgrData data;
		data.mObjNumbers = OriObjNumers;
		data.mNewObjNumbers = NewObjNumbers;
		mDataList.Append( data );   
	}

	CHE_SkipList< ObjectCloneMgrData > mDataList;
};

CHE_PDF_Dictionary*		CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr = NULL );
CHE_PDF_Array*			CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr = NULL );
HE_PDF_RefInfo			CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr = NULL );

#endif