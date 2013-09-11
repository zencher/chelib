#include "StdAfx.h"
#include "RenderWork.h"


DWORD WINAPI renderThread( LPVOID param )
{
	if ( param == NULL )
	{
		return 1;
	}

	CRenderWorker * p = (CRenderWorker*)(param);
	CRenderManager * pBoss = p->GetBoss();
	if ( pBoss == NULL )
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
			int param = item.param;
			Sleep(50);
			item.result = param * param;
			pBoss->SetResult( item );
		}
	}
	return 0;
}


CRenderWorker::CRenderWorker()
	: mIsOver(false), mThreadId(0), mWorkEvent(NULL), mThread(NULL), mpBoss(NULL)
{
	mWorkEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
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


bool CRenderManager::NewWork( int param )
{
	bool bExist = false;
	WORKITEM item;
	item.param = param;
	item.result = 0;
	item.workerId = 0;

	if ( mWorkMutex )
	{
		WaitForSingleObject( mWorkMutex, INFINITE );
		std::list<WORKITEM>::iterator it;
		for ( it = mWorkList.begin(); it != mWorkList.end(); ++it )
		{
			if ( it->param == param )
			{
				bExist = true;
				break;
			}
		}
		if ( ! bExist )
		{
			for ( it = mWorkingList.begin(); it != mWorkingList.end(); ++it )
			{
				if ( it->param == param )
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
				if ( it->param == param )
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
			work = mWorkList.front();
			mWorkList.pop_front();
			mWorkingList.push_back( work );
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