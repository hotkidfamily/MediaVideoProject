#pragma once

class CLock
{
public:
	CLock(){
		InitializeCriticalSection(&mCs);
	};

	~CLock(){
		DeleteCriticalSection(&mCs);
	};

	void Lock(){
		EnterCriticalSection(&mCs);
	}

	void Unlock(){
		LeaveCriticalSection(&mCs);
	}

private:
	CRITICAL_SECTION mCs;
};


class CAutoLock{
public:
	CAutoLock() {
		mLock.Lock();
	}

	~CAutoLock(){
		mLock.Unlock();
	}

private:
	CLock mLock;
};

