#include "GameTimer.h"
#include <windows.h>

GameTimer::GameTimer() 
	: mSecondsPerCount(0.f),
	mDeltaTime(-1.f),
	mBaseTime(0),
	mPasuedTime(0),
	mStopTime(0),
	mPrevTime(0),
	mCurrTime(0),
	bStop(false)
{
	__int64 countPresec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPresec);
	mSecondsPerCount = 1.f / (double)countPresec;
}
