#pragma once

#include "pgc1000.h"

namespace app {

	using namespace pgc1000;

	class Decoder
	{
	public:
		Decoder(const u8* pdata = nullptr, u32 size = 0) : mpData(pdata), mSize(size) { clear(); }

		bool isEmpty() const { return !mNum && mPt >= mSize; }

		u8 get();

		void set(const u8* pdata, u32 size)
		{
			mpData = pdata;
			mSize = size;
			clear();
		}

		void clear()
		{
			mPt = mNum = 0;
			mPrev = -1;
			mContinue = false;
		}

	protected:
		const u8* mpData;
		u32 mSize;
		u32 mPt;
		s32 mNum;
		s32 mPrev;
		bool mContinue;
	};

	class BadApple : public App
	{
	public:
        typedef App super;

        virtual void init();

        virtual void update();

	protected:
        Decoder mDecoderIndex;
        Decoder mDecoderPage;
        bool mPause;
        bool mShowInfo;
	};

}
