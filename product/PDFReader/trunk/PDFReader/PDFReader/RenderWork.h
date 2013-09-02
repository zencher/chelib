#pragma once

#include <list>
#include <vector>

#include <Windows.h>

class CRenderManager;

class CRenderWorker
{
public:
	CRenderWorker();
	~CRenderWorker();

	void				SetBoss( CRenderManager * pBoss );
	CRenderManager *	GetBoss();
	
	void				WorkMessage();

	HANDLE				GetWorkEvent();
	bool				IsOver();
	
private:
	bool				mIsOver;
	DWORD				mThreadId;
	HANDLE				mWorkEvent;
	HANDLE				mThread;
	CRenderManager*		mpBoss;
};

struct WORKITEM
{
	int param;
	int result;
	int workerId;
};

class CRenderManager
{
public:
	CRenderManager( int workerCount = 1 );
	~CRenderManager();

	HANDLE	GetResultEvent();

	bool	NewWork( int param );

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