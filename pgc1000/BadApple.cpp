#include "BadApple.h"
#include "BadAppleResource.h"

using namespace app;

u8 Decoder::get()
{
	if (mContinue) {
		if (mNum == 0) {
			u8 n = 0, i = 0;
			do {
				n = mpData[mPt++];
				mNum |= (n & 0x7F) << (7 * i++);
			} while (n & 0x80);
		}

		if (mNum > 0) {
			const u8 prev = mPrev;
			if (--mNum <= 0) {
				mPrev = -1;
				mContinue = false;
			}
			return prev;
		}
	}

	const u8 curr = mpData[mPt++];
	mContinue = mPrev == (s32)curr;
	return mPrev = curr;
}




void BadApple::init()
{
	super::init();
	mPause = false;
	mShowInfo = false;

	mSystem.setFPS(MOVIE_FRAME_RATE);
	mDecoderIndex.set(MOVIE_C_INDEX, MOVIE_C_INDEX_SIZE);
	mDecoderPage.set(MOVIE_C_PAGE, MOVIE_C_PAGE_SIZE);

	mRender.clear();
}

void BadApple::update()
{
	if (mSystem.getBtnRed().on) {
		mSystem.setFrameWait(false);
	} else {
		mSystem.setFrameWait(true);
	}
	if (mSystem.getBtnBlack().trg) {
		mPause = !mPause;
	}
	if (mSystem.getBtnGreen().trg) {
		mShowInfo = !mShowInfo;
	}

	if (!mPause || mSystem.getBtnBlue().trg) {
		u8* dst = mRender.map();
		{
			if (mDecoderIndex.isEmpty()) {
				mDecoderIndex.clear();
				mDecoderPage.clear();
			}

			const u32 ofs = (Render::SCREEN_WIDTH - MOVIE_FRAME_WIDTH) >> 1;

			u8 index[MOVIE_FRAME_WIDTH];
			for (u32 x = 0; x < MOVIE_FRAME_WIDTH; ++x)
				index[x] = mDecoderIndex.get();

			for (u32 y = 0; y < MOVIE_FRAME_PAGE_HEIGHT; ++y) {
				for (u32 x = 0; x < MOVIE_FRAME_WIDTH; ++x) {
					if (index[x] & (1 << y)) {
						const u8 page = mDecoderPage.get();
						dst[((x + ofs) << 3) + y] = page;
					}
				}
			}
		}
		mRender.unmap();
	}

	if (mShowInfo) {
		const f32 fps = mSystem.getActualFPS();
		const f32 time = mSystem.getFrameTime();
		mRender.setDrawMode(DM_INVERSE);
		mRender.drawString(0, 0, "%.1fFPS", fps);
		mRender.drawString(0, 6, "%.1fms", time);
		if (mPause) {
			mRender.drawString(0, 12, "Pause");
		}
		//mSystem.drawInfo(mRender);

		mRender.present();

		mRender.drawString(0, 0, "%.1fFPS", fps);
		mRender.drawString(0, 6, "%.1fms", time);
		if (mPause) {
			mRender.drawString(0, 12, "Pause");
		}
		//mSystem.drawInfo(mRender);
	} else {
		mRender.present();
	}
    super::update();
}
