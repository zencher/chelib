#include "StdAfx.h"
#include <float.h>
#include "RenderWork.h"


DWORD WINAPI renderThread( LPVOID param )
{
	if ( param == NULL )
	{
		return 1;
	}

	CRenderWorker * p = (CRenderWorker*)(param);
	CRenderManager * pBoss = p->GetBoss();
	CHE_GraphicsDrawer * pDrawer = p->GetDrawer();
	if ( pBoss == NULL || pDrawer == NULL )
	{
		return 2;
	}

	DWORD dwRet = 0;
	WORKITEM item;
	HANDLE workEvent = p->GetWorkEvent();
	while( TRUE )
	{
		if ( p->IsOver() )
		{
			break;
		}
		dwRet = WaitForSingleObject( workEvent, INFINITE );
		while ( pBoss->GetWork( item ) && ! p->IsOver() )
		{
			p->SetWorkItem( item );
			CHE_PDF_Renderer::Render( *(item.pContents), *pDrawer, item.pageRect, item.rotate % 360, item.scale );
			item.pBitmapRet = p->GetAllocator()->New<CHE_Bitmap>( p->GetAllocator() );
			pDrawer->GetBitmap( *(item.pBitmapRet) );
			pBoss->SetResult( item );
		}
		item.param = -1;
		p->SetWorkItem( item );
	}
	return 0;
}


CRenderWorker::CRenderWorker( CHE_Allocator * pAllocator )
	:CHE_Object(pAllocator), mIsOver(false), mThreadId(0), mWorkEvent(NULL), mThread(NULL), mpBoss(NULL)
{
	mWorkEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	mWorkItem.param = -1;

	mDC = GetDC(NULL);
	mpDrawer = GetAllocator()->New<CHE_GraphicsDrawer>( mDC, 1, 1 );
	mThread = CreateThread( NULL, 0, renderThread, this, 0, &mThreadId );
}


CRenderWorker::~CRenderWorker(void)
{
	mIsOver = true;
	if ( mWorkEvent )
	{
		SetEvent( mWorkEvent );
		CloseHandle( mWorkEvent );
	}
	if ( mThread )
	{
		CloseHandle( mThread );
	}
	if ( mpDrawer )
	{
		GetAllocator()->Delete( mpDrawer );
	}
	if ( mDC )
	{
		ReleaseDC( NULL, mDC );
	}
}


void CRenderWorker::SetBoss( CRenderManager * pBoss )
{
	mpBoss = pBoss;
}


CRenderManager * CRenderWorker::GetBoss()
{
	return mpBoss;
}


void CRenderWorker::WorkMessage()
{
	if ( mWorkEvent )
	{
		SetEvent( mWorkEvent );
	}
}


HANDLE CRenderWorker::GetWorkEvent()
{
	return mWorkEvent;
}


bool CRenderWorker::IsOver()
{
	return mIsOver;
}


CRenderManager::CRenderManager( int workerCount /*= 1*/ )
{
	mWorkers.resize( workerCount );
	for ( size_t i = 0; i < mWorkers.size(); ++i )
	{
		mWorkers[i] = new CRenderWorker;
		mWorkers[i]->SetBoss( this );
	}
	mResultEvent	= CreateEvent( NULL, FALSE, FALSE, NULL );
	mWorkMutex		= CreateMutex( NULL, FALSE, NULL ); 
	mResultMutex	= CreateMutex( NULL, FALSE, NULL );
}


CRenderManager::~CRenderManager()
{
	if ( mResultEvent )
	{
		CloseHandle( mResultEvent );
	}
	if ( mWorkMutex )
	{
		CloseHandle( mWorkMutex );
	}
	if ( mResultMutex )
	{
		CloseHandle( mResultMutex );
	}
}


HANDLE CRenderManager::GetResultEvent()
{
	return mResultEvent;
}


bool CRenderManager::NewWork(	int pageIndex, float scale, int rotate, CHE_Rect pageRect,
								CHE_PDF_ContentObjectList * pContents )
{
	if ( pContents == NULL )
	{
		return false;
	}

	bool bExist = false;
	WORKITEM item;
	item.param = pageIndex;
	item.scale = scale;
	item.rotate = rotate;
	item.pageRect = pageRect;
	item.pContents = pContents;
	item.pBitmapRet = NULL;
	item.workerId = 0;

	if ( mWorkMutex )
	{
		WaitForSingleObject( mWorkMutex, INFINITE );
		std::list<WORKITEM>::iterator it;
		for ( it = mWorkList.begin(); it != mWorkList.end(); ++it )
		{
			if (	(it->param == pageIndex) && 
					(it->scale - scale <= FLT_EPSILON) &&
					(rotate % 360 == it->rotate % 360) )
			{
				bExist = true;
				break;
			}
		}
		if ( ! bExist )
		{
			std::vector<CRenderWorker*>::iterator workerIt = mWorkers.begin();
			for ( ; workerIt != mWorkers.end(); ++workerIt )
			{
				WORKITEM & wItem = (*workerIt)->GetWorkItem();
				if (	(wItem.param == pageIndex) && 
						(wItem.scale - scale <= FLT_EPSILON) &&
						(rotate % 360 == wItem.rotate % 360) )
				{
					bExist = true;
					break;
				}
			}
		}
		if ( ! bExist )
		{
			for ( it = mResultList.begin(); it != mResultList.end(); ++it )
			{
				if (	(it->param == pageIndex) && 
						(it->scale - scale <= FLT_EPSILON) &&
						(rotate % 360 == it->rotate % 360) )
				{
					bExist = true;
					break;
				}
			}
		}
		if ( !bExist )
		{
			mWorkList.push_back( item );
		}
		ReleaseMutex( mWorkMutex );
		if ( !bExist )
		{
			for ( size_t i = 0; i < mWorkers.size(); ++i )
			{
				mWorkers[i]->WorkMessage();
			}
		}
		return true;
	}

	return false;
}


bool CRenderManager::GetWork( WORKITEM & work )
{
	bool bRet = false;
	if ( mWorkMutex )
	{
		WaitForSingleObject( mWorkMutex, INFINITE );
		if ( ! mWorkList.empty() )
		{
			work = mWorkList.back();
			mWorkList.pop_back();
			bRet = true;
		}
		ReleaseMutex( mWorkMutex );
	}
	return bRet;
}


bool CRenderManager::SetResult( const WORKITEM & result )
{
	bool bRet = false;
	if ( mResultMutex )
	{
		WaitForSingleObject( mResultMutex, INFINITE );
		mResultList.push_back( result );
		if ( mResultEvent )
		{
			SetEvent( mResultEvent );
		}
		ReleaseMutex( mResultMutex );
	}

	return bRet;
}


bool CRenderManager::GetResult( WORKITEM & result )
{
	bool bRet = false;
	if ( mResultMutex )
	{
		WaitForSingleObject( mResultMutex, INFINITE );
		if ( ! mResultList.empty() )
		{
			result = mResultList.front();
			mResultList.pop_front();
			bRet = true;
		}
		ReleaseMutex( mResultMutex );
	}
	return bRet;
}