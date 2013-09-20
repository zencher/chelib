#pragma once

#include <list>
#include <vector>
#include <Windows.h>

#include "../../../../../trunk/include/che_bitmap.h"
#include "../../../../../trunk/include/che_drawer_windows.h"
#include "../../../../../trunk/include/pdf/che_pdf_contents.h"
#include "../../../../../trunk/include/pdf/che_pdf_renderer_windows.h"

struct WORKITEM
{
	int					param;	//page index
	int					workerId;
	float				scale;
	int					rotate;
	CHE_Rect			pageRect;
	CHE_PDF_ContentObjectList * pContents;
	CHE_Bitmap *		pBitmapRet;
};

class CRenderManager;

class CRenderWorker : public CHE_Object
{
public:
	CRenderWorker( CHE_Allocator * pAllocator = NULL );
	~CRenderWorker();

	void				SetBoss( CRenderManager * pBoss );
	CRenderManager *	GetBoss();
	
	void				WorkMessage();

	HANDLE				GetWorkEvent();
	bool				IsOver();

	CHE_GraphicsDrawer*	GetDrawer() { return mpDrawer; }

	void				SetWorkItem( WORKITEM & workItem )
	{
		mWorkItem.param = workItem.param;
		mWorkItem.pageRect = workItem.pageRect;
		mWorkItem.rotate = workItem.rotate;
		mWorkItem.scale = workItem.scale;
		mWorkItem.pBitmapRet = NULL;
		mWorkItem.pContents = NULL;
	}
	WORKITEM &			GetWorkItem() { return mWorkItem; }
	
private:
	bool				mIsOver;
	DWORD				mThreadId;
	HANDLE				mWorkEvent;
	HANDLE				mThread;
	HDC					mDC;
	CRenderManager*		mpBoss;
	CHE_GraphicsDrawer * mpDrawer;

	WORKITEM			mWorkItem;
};


class CRenderManager
{
public:
	CRenderManager( int workerCount = 1 );
	~CRenderManager();

	HANDLE	GetResultEvent();

	bool	NewWork(	int pageIndex, float scale, int rotate, CHE_Rect pageRect, 
						CHE_PDF_ContentObjectList * pContents );

	bool	GetWork( WORKITEM & work );

	bool	SetResult( const WORKITEM & result );

	bool	GetResult( WORKITEM & result );
	
private:
	HANDLE						mResultEvent;
	HANDLE						mWorkMutex;
	HANDLE						mResultMutex;
	std::vector<CRenderWorker*>	mWorkers;
	std::list<WORKITEM>			mWorkList;
	std::list<WORKITEM>			mResultList;
};