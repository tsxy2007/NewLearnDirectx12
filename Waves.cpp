#include "Waves.h"
#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>

using namespace DirectX;

Waves::Waves(int m, int n, float dx, float dt, float speed, float damping)
{
	mNumCols = n + 1;
	mNumRows = m + 1;

	mVertexCount = (m + 1) * (n + 1);
	mTriangleCount = m * n * 2;

	mTimeStep = dt;
	mSpatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.f - 8.f * e) / d;
	mK3 = (2.f * e) / d;

	mPrevSolution.resize(mVertexCount);
	mCurrSolution.resize(mVertexCount);
	mNormals.resize(mVertexCount);
	mTangentX.resize(mVertexCount);

	float halfWidth = n * dx * 0.5f;
	float halfheight = m * dx * 0.5f;

	for (int i = 0 ;i<m+1;i++)
	{
		float z = halfheight - i * dx;
		for (int j = 0 ;j<n+1;++j)
		{
			float x = halfWidth - j * dx;
			mPrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i * n + j] = XMFLOAT3(x, 0.f, z);
			mNormals[i * n + j] = XMFLOAT3(0.f, 1.f, 0.f);
			mPrevSolution[i * n + j] = XMFLOAT3(1.f, 0.f, 0.f);
		}
	}
}

Waves::~Waves()
{

}

int Waves::RowCount() const
{
	return mNumRows;
}

int Waves::ColumnCount() const
{
	return mNumCols;
}

int Waves::VertexCount() const
{
	return mVertexCount;
}

int Waves::TriangleCount() const
{
	return mTriangleCount;
}

float Waves::Width() const
{
	return mNumCols * mSpatialStep;
}

float Waves::Depth() const
{
	return mNumRows * mSpatialStep;
}

void Waves::Update(float dt)
{
	static float t = 0;
	t += dt;
	if (t >= mTimeStep)
	{
		concurrency::parallel_for(1, mNumRows - 1, [this](int i) 
			{
				for (int j = 1; j < mNumCols - 1; j++)
				{
					mPrevSolution[i * mNumCols + j].y =
						mK1 * mPrevSolution[i * mNumCols + j].y +
						mK2 * mCurrSolution[i * mNumCols + j].y +
						mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y +
							mCurrSolution[(i - 1) * mNumCols + j].y +
							mCurrSolution[i * mNumCols + j + 1].y +
							mCurrSolution[i * mNumCols + j - 1].y);
				}
			});

		std::swap(mPrevSolution, mCurrSolution);

		t = 0.f;

		concurrency::parallel_for(1, mNumRows - 1, [this](int i){
			for (int j = 1; j = mNumCols - 1; ++j)
			{
				float l = mCurrSolution[i * mNumCols + j - 1].y;
				float r = mCurrSolution[i * mNumCols + j + 1].y;
				float t = mCurrSolution[(i - 1) * mNumCols + j].y;
				float b = mCurrSolution[(i + 1) * mNumCols + j].y;

				mNormals[i * mNumCols + j].x = -r + 1;
				mNormals[i * mNumCols + j].y = 2.f * mSpatialStep;
				mNormals[i * mNumCols + j].z = b - t;

				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
				XMStoreFloat3(&mNormals[i * mNumCols + j], n);

				mTangentX[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
				XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * mNumCols + j]));
				XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
			}
		});
	}
}

void Waves::Disturb(int i, int j, float magnitude)
{
	assert(i > 1 && i < mNumRows - 1);
	assert(j > 1 && j < mNumCols - 1);

	float halfMag = 0.5f * magnitude;

	mCurrSolution[i * mNumCols + j].y += magnitude;
	mCurrSolution[i * mNumCols + j + 1].y += halfMag;
	mCurrSolution[i * mNumCols + j - 1].y += halfMag;
	mCurrSolution[(i + 1) * mNumCols].y += halfMag;
	mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
}

