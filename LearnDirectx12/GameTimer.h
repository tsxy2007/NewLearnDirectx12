#pragma once
class GameTimer
{
public:
	GameTimer();

	float TotalTime()const;// ��������λ
	float DeltaTime()const;
	void Reset(); // �ڿ�ʼ��Ϣѭ��֮ǰ����
	void Start(); // �����ͣ
	void Stop();// ��ͣ
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

