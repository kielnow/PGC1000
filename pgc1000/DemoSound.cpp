#include "DemoSound.h"

using namespace app;

void DemoSound::init()
{
    super::init();
    mVolume = 15;
    mFreq = 0xFF;
    mEnv = 0;
    mEnvFreq = 8;
    mPause = false;
    memset(mLog, 0, sizeof(mLog));
    mHead = 0;
}

void DemoSound::update()
{
    Psg &psg = mSound.getPsg();
    
    if (mSystem.getBtnRed().trg) {
        mPause = !mPause;
    }
    if (mSystem.getBtnBlack().trg) {
        mEnv = (mEnv + 1) % 8;
    }

#if 1
    if (mSystem.getBtnDown().rep) {
        if (mEnvFreq < 32) {
            --mEnvFreq;
        } else {
            mEnvFreq >>= 1;
        }
        if (mEnvFreq <= 0) {
            mEnvFreq = 0;
        }
    }
    if (mSystem.getBtnUp().rep) {
        if (mEnvFreq < 32) {
            ++mEnvFreq;
        } else {
            mEnvFreq <<= 1;
        }
        if (mEnvFreq >= 0xFFFF) {
            mEnvFreq = 0xFFFF;
        }
    }
#else
    if (mSystem.getBtnDown().rep) {
        if (--mVolume <= 0) {
            mVolume = 0;
        }
    }
    if (mSystem.getBtnUp().rep) {
        if (++mVolume >= 15) {
            mVolume = 15;
        }
    }
#endif

    if (mPause) {
        if (mSystem.getBtnLeft().rep) {
            mFreq -= 0x7F;
            if (mFreq <= 1) {
                mFreq = 1;
            }
        }
        if (mSystem.getBtnRight().rep) {
            mFreq += 0x7F;
            if (mFreq >= 0x0FFF) {
                mFreq = 0x0FFF;
            }
        }
    } else {
        if (mSystem.getFrameCount() % 5 == 0) {
            u8 r = rand() & 0xFF;
            mFreq = r << 4;
        }
    }
    
    mLog[mHead] = mFreq >> 4;
    mHead = (mHead + 1) % LOG_MAX;
    
    psg.setMixer(MIX_TONE_A);
    //psg.setVolume(CHANNEL_A, false, mVolume);
    psg.setVolume(CHANNEL_A, true, mVolume);
    psg.setToneFrequency(CHANNEL_A, mFreq);
    psg.setEnvelopeControl(0x08 + mEnv);
    psg.setEnvelopeFrequency(mEnvFreq);
    
    mRender.clear();
    
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawString(0, 24, "Vol=%-2d", mVolume);
    mRender.drawString(0, 30, "FQ=%-4u", (u16)mFreq);
    mRender.drawString(52, 24, "Env=%-2u", mEnv);
    mRender.drawString(52, 30, "EnvFQ=%-4u", (u16)mEnvFreq);
    
    const u8 w = Render::SCREEN_WIDTH / LOG_MAX;
    const u8 h = 28;
    mRender.drawFillRect(0, 36, Render::SCREEN_WIDTH, h);
    mRender.setDrawMode(DM_INVERSE);
    for (u8 i = 0; i < LOG_MAX; ++i) {
        mRender.drawFillRect(w * i, Render::SCREEN_HEIGHT - 2 - ((h - 2) * mLog[i] / 0xFF), w, 2);
    }
    mRender.drawFillRect(w * mHead, Render::SCREEN_HEIGHT - h, 8, h);
    
    mRender.setDrawMode(DM_POSITIVE);
    mSystem.drawInfo(mRender);
    mRender.present();
    super::update();
}
