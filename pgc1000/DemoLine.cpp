#include "DemoLine.h"

using namespace app;

template<class T> T min(T x, T y){ return x < y ? x : y; }
template<class T> T max(T x, T y){ return x > y ? x : y; }
template<class T> T clamp(T x, T mn, T mx){ return max(mn, min(x, mx)); }

void DemoLine::init()
{
    super::init();
    
    mSystem.setFPS(60);
    
    mPause = false;
    mShowInfo = false;
    
    mPos.x = 90;
    mPos.y = 60;
    
    mPoint[ 0].x    = 20.f;
    mPoint[ 0].y    = 4.f;
    mPoint[ 0].vx   = 1.f;
    mPoint[ 0].vy   = -1.f;
    mPoint[ 4].x    = 120.f;
    mPoint[ 4].y    = 30.f;
    mPoint[ 4].vx   = 1.2f;
    mPoint[ 4].vy   = 1.f;
    mPoint[ 8].x    = 96.f;
    mPoint[ 8].y    = 58.f;
    mPoint[ 8].vx   = -0.9f;
    mPoint[ 8].vy   = 1.1f;
    mPoint[12].x    = 4.f;
    mPoint[12].y    = 50.f;
    mPoint[12].vx   = -1.3f;
    mPoint[12].vy   = -0.8f;
    for (u8 i = 0; i < 4; ++i) {
        Point& p = mPoint[i * 4];
        for (u8 j = 0; j < 4; ++j) {
            Point& q = mPoint[i * 4 + j];
            q.x = p.x + 2.f * j;
            q.y = p.y + 1.5f * j;
            q.vx = p.vx;
            q.vy = p.vy;
        }
    }
}

void DemoLine::update()
{
    const f32 deltaTime = mSystem.getDeltaTime();
    const System::BtnInfo &btnBlack = mSystem.getBtnBlack();
    const System::BtnInfo &btnRed = mSystem.getBtnRed();
    const System::BtnInfo &btnBlue = mSystem.getBtnBlue();
#if 0
    const System::BtnInfo &btnUp = mSystem.getBtnUp();
    const System::BtnInfo &btnDown = mSystem.getBtnDown();
    const System::BtnInfo &btnLeft = mSystem.getBtnLeft();
    const System::BtnInfo &btnRight = mSystem.getBtnRight();
    const f32 analogX = mSystem.getAnalogX();
    const f32 analogY = mSystem.getAnalogY();
    const f32 speed = 2.f;
#endif

    if (btnBlue.trg) {
        mShowInfo = !mShowInfo;
    }

    if (btnBlack.trg) {
        mPause = !mPause;
    }

    if (mSystem.isFrameWait()) {
        if (btnRed.on)
            mSystem.setFrameWait(false);
    } else {
        if (btnRed.rel)
            mSystem.setFrameWait(true);
    }

#if 0
    if (btnUp.on || btnDown.on) {
        mPos.y = clamp(mPos.y + analogY * speed, 0.f, 63.f);
    }
    if (btnLeft.on || btnRight.on) {
        mPos.x = clamp(mPos.x + analogX * speed, 0.f, 127.f);
    }
    mRender.drawLine(63, 31, (s16)mPos.x, (s16)mPos.y);    
#endif

    if (!mPause) {
        for (u8 i = 0; i < POINT_NUM; ++i) {
            Point& p = mPoint[i];
            p.x += p.vx;
            p.y += p.vy;
            if (p.x < 0.f) {
                p.x = 0.f;
                p.vx *= -1.f;
                p.x += p.vx;
            } else if (127.f < p.x) {
                //p.x = 127.f;
                p.vx *= -1.f;
                p.x += p.vx;
            }
            if (p.y < 0.f) {
                //p.y = 0.f;
                p.vy *= -1.f;
                p.y += p.vy;
            } else if (63.f < p.y) {
                //p.y = 63.f;
                p.vy *= -1.f;
                p.y += p.vy;
            }
        }        
    }
    
    mRender.clear();

    mRender.setDrawMode(DM_POSITIVE);
    for (u8 i = 0; i < POINT_NUM; ++i) {
        Point& p = mPoint[i];
        Point& q = mPoint[((i / 4 + 1) * 4 + i % 4) % POINT_NUM];
        mRender.drawLine((s16)p.x, (s16)p.y, (s16)q.x, (s16)q.y);    
    }
    
    if (mShowInfo) {
        mRender.setDrawMode(DM_POSITIVE);
        mSystem.drawInfo(mRender);
    }
    if (mPause && mSystem.getFrameCount() % 80 > 30) {
        mRender.setDrawMode(DM_POSITIVE_ADD);
        mRender.drawString(0, 30, "Pause");
    }

    mRender.present();
    super::update();
}