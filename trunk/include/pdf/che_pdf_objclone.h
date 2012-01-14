#ifndef _CHE_PDF_OBJCLONE_H_
#define _CHE_PDF_OBJCLONE_H_

#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_define.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_creator.h"

class IHE_ObjectCloneMgr
{
public:
	virtual HE_BOOL		IsCloned( HE_DWORD OriObjNum ) = 0;
	virtual HE_DWORD	GetMapObjNum( HE_DWORD OriObjNum ) = 0;
	virtual HE_VOID		SetMap( HE_DWORD OriObjNum, HE_DWORD NewObjNum ) = 0;
};

class ObjectCloneMgrData
{
public:
	ObjectCloneMgrData() { mObjNum = 0; mNewObjNum = 0;}
	HE_DWORD mObjNum;
	HE_DWORD mNewObjNum;

	ObjectCloneMgrData & operator = ( const ObjectCloneMgrData data )
	{
		mObjNum = data.mObjNum;
		mNewObjNum = data.mNewObjNum;
		return *this;
	}
};

bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 );

class CHE_ObjectCloneMgr : public IHE_ObjectCloneMgr
{
public:
	HE_BOOL IsCloned( HE_DWORD OriObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		return mDataList.Find( data );
	}

	HE_DWORD GetMapObjNum( HE_DWORD OriObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		mDataList.Find( data );
		return data.mNewObjNum;
	}

	HE_VOID	SetMap( HE_DWORD OriObjNum, HE_DWORD NewObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		data.mNewObjNum = NewObjNum;
		mDataList.Append( data );   
	}

	CHE_SkipList< ObjectCloneMgrData > mDataList;
};

CHE_PDF_Dictionary*	CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_Creator * creator, /*HE_DWORD objNum, HE_DWORD genNum,*/ IHE_ObjectCloneMgr * pMgr = NULL );
CHE_PDF_Array*		CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_Creator * creator, /*HE_DWORD objNum, HE_DWORD genNum,*/ IHE_ObjectCloneMgr * pMgr = NULL );
HE_DWORD			CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr = NULL );

#endif