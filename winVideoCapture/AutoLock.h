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

	BOOL Lock(){
		BOOL bRet = FALSE;

		bRet = TryEnterCriticalSection(&mCs);

		return bRet>0;
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

