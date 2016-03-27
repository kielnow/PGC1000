#include "DemoTone.h"

#define M_PI    3.14159265358979323846f

using namespace app;

namespace {
    
    enum {
        TONE_MAX = 7,
    };
    
}

void DemoTone::init()
{
    super::init();
    mCount = 16;
    mDeltaCount = 1;
    mTheta = 0.f;
    mPause = false;
    mToneNum = 7;
    mSystem.getBtnBlack().rep_count = 1;
    System::BtnInfo &btnUp = mSystem.getBtnUp();
    System::BtnInfo &btnDown = mSystem.getBtnDown();
    btnUp.rep1_count = btnDown.rep1_count = 15;
    btnUp.rep_count = btnDown.rep_count = 5;
    mRender.setAnimSpeed(0);
}

void DemoTone::update()
{
    if (mSystem.getBtnUp().rep) {
        if (++mToneNum > TONE_MAX)
            mToneNum = TONE_MAX;
    }
    if (mSystem.getBtnDown().rep) {
        if (--mToneNum < 2)
            mToneNum = 2;
    }
    if (mSystem.getBtnRed().trg) {
        mPause = !mPause;
    }
    if (!mPause || mSystem.getBtnBlack().rep) {
        const u32 fc = mSystem.getFrameCount();
        if ((fc & 0x3) == 0) {
            mCount += mDeltaCount;
            if (mCount < 3) {
                mCount = 3;
                mDeltaCount *= -1;
            }
            if (32 < mCount) {
                mCount = 32;
                mDeltaCount *= -1;
            }
        }
        
        {
            mTheta += M_PI / 12.f;
            if (mTheta >= 2.f * M_PI) {
                mTheta = 0.f;
            } 
        }
    }
    
    const u8 RADIUS = 2;
    const s8 OFFSET_X = RADIUS * cos(mTheta) - RADIUS;
    const s8 OFFSET_Y = RADIUS * sin(mTheta) - RADIUS;
    const u8 WIDTH  = mCount;
    const u8 HEIGHT = mCount;
    const u8 X_MAX  = (Render::SCREEN_WIDTH / WIDTH) + 2 * RADIUS;
    const u8 Y_MAX  = (Render::SCREEN_HEIGHT / HEIGHT) + 2 * RADIUS;
    const u32 DM[][TONE_MAX] = {
        { DM_POSITIVE, DM_NEGATIVE, }, // 2
        { DM_POSITIVE, DM_PAT_0, DM_NEGATIVE, }, // 3
        { DM_POSITIVE, DM_PAT_3, DM_PAT_0, DM_NEGATIVE, }, // 4
        { DM_POSITIVE, DM_PAT_3, DM_PAT_0, DM_PAT_4, DM_NEGATIVE, }, // 5
        { DM_POSITIVE, DM_PAT_5, DM_PAT_3, DM_PAT_0, DM_PAT_4, DM_NEGATIVE, }, // 6
        { DM_POSITIVE, DM_PAT_5, DM_PAT_3, DM_PAT_0, DM_PAT_4, DM_PAT_6, DM_NEGATIVE, }, // 7
    };
    
    for (u8 y = 0; y < Y_MAX; ++y) {
        for (u8 x = 0; x < X_MAX; ++x) {
#if 0
            switch ((x + y) % 7) {
            case 0:
                mRender.setDrawMode(DM_POSITIVE);
                break;
            case 1:
                mRender.setDrawMode(DM_PAT_5);
                break;
            case 2:
                mRender.setDrawMode(DM_PAT_3);
                break;
            case 3:
                mRender.setDrawMode(DM_PAT_0);
                break;
            case 4:
                mRender.setDrawMode(DM_PAT_4);
                break;
            case 5:
                mRender.setDrawMode(DM_PAT_6);
                break;
            case 6:
                mRender.setDrawMode(DM_NEGATIVE);
                break;
            }
#else
            mRender.setDrawMode(DM[mToneNum - 2][(x + y) % mToneNum]);
#endif
            mRender.drawFillRect(OFFSET_X + x * WIDTH, OFFSET_Y + y * HEIGHT, WIDTH, HEIGHT);
        }
    }
    //mSystem.drawInfo(mRender);    
    mRender.present();
    super::update();
}