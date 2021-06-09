#pragma once
class GameTimer
{
public:
	GameTimer();

	float TotalTime()const;// 用秒做单位
	float DeltaTime()const;
	void Reset(); // 在开始消息循环之前调用
	void Start(); // 解除暂停
	void Stop();// 暂停
	void Tick();

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPasuedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool bStop;
};

