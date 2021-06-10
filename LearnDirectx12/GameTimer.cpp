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

float GameTimer::TotalTime() const 
{
	if (bStop)
	{
		return (float)(((mStopTime - mBaseTime) - mPasuedTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mBaseTime) - mPasuedTime) * mSecondsPerCount);
	}
	return 0.f;
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	bStop = false;
}

void GameTimer::Start()
{
	__int64 StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	if (bStop)
	{
		mPasuedTime += (StartTime - mStopTime);
		mPrevTime = StartTime;

		mStopTime = 0;
		bStop = false;
	}
}

void GameTimer::Stop()
{
	if (!bStop)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		bStop = true;
	}
}

void GameTimer::Tick()
{
	if (bStop)
	{
		mDeltaTime = 0.f;
		return;
	}

	//获取本帧开始显示的时刻
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;
	//本帧与前一帧的时间差
	mDeltaTime = (currTime - mPrevTime) * mSecondsPerCount;

	//准备计算本帧与下一帧时间差
	mPrevTime = currTime;
	if (mDeltaTime < 0)
	{
		mDeltaTime = 0.f;
	}
}
