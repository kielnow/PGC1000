#include "Bricks.h"

using namespace app;

#ifndef M_PI
#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923
#define M_PI_4  0.785398163397448309616
#endif

namespace {
    
    enum {
        VIEW_W      = 96,
        VIEW_H      = 64,
        VIEW_L      = (Render::SCREEN_WIDTH - VIEW_W) / 2,
        VIEW_T      = 0,
        VIEW_R      = VIEW_L + VIEW_W,
        VIEW_B      = VIEW_T + VIEW_H,
        
        BORDER_W    = 2,
    };
    
}

Bricks::Bricks()
//    : mBrick(new Brick[BRICK_MAX])
{
}

Bricks::~Bricks()
{
//    delete mBrick;
}

void Bricks::init()
{
    super::init();
    
    mSystem.setFPS(60);
    
    mPause = false;
    mShowInfo = false;
 
    std::memset(&mInfo, 0, sizeof(mInfo));
    mInfo.next = true;
 
    std::memset(&mBar, 0, sizeof(mBar));
    mBar.w = 24;
    mBar.h = 2;
    mBar.x = (VIEW_W - mBar.w) / 2;
    mBar.y = 56;
    mBar.speed = 2.f;
    mBar.x_old = mBar.x;
    mBar.y_old = mBar.y;
    
    std::memset(mBall, 0, sizeof(mBall));
    
    std::memset(mBrick, 0, sizeof(mBrick));
#if 1
    for (u8 y = 0; y < 8; ++y) {
        for (u8 x = 0; x < 10; ++x) {
            Brick &b = mBrick[y * 10 + x];
            b.enable = true;
            b.x = 8.f + 8.f * x;
            b.y = 6.f + 4.f * y;
            b.w = 8;
            b.h = 4;
            b.pat = ((x == 1 || x == 8) && 1 <= y && y <= 6) || (1 <= x && x <= 8 && (y == 1 || y == 6)) ? 1 : 2;
            if (3 <= x && x <= 6 && 3 <= y && y <= 4)
                b.pat = 0;
        }
    }
#else
    for (u8 y = 0; y < 11; ++y) {
        for (u8 x = 0; x < 12; ++x) {
            Brick &b = mBrick[y * 12 + x];
            b.enable = true;
            b.x = 8.f * x;
            b.y = 6.f + 4.f * y;
            b.w = 8;
            b.h = 4;
            b.pat = ((x == 1 || x == 10) && 1 <= y && y <= 10) || (1 <= x && x <= 10 && (y == 1 || y == 10)) ? 1 : 0;
        }
    }
#endif
}

void Bricks::update()
{
    const f32 deltaTime = mSystem.getDeltaTime();
    const System::BtnInfo &btnBlack = mSystem.getBtnBlack();
    const System::BtnInfo &btnRed = mSystem.getBtnRed();
    const System::BtnInfo &btnGreen = mSystem.getBtnGreen();
    const System::BtnInfo &btnBlue = mSystem.getBtnBlue();
    const System::BtnInfo &btnLeft = mSystem.getBtnLeft();
    const System::BtnInfo &btnRight = mSystem.getBtnRight();
    const f32 analogX = mSystem.getAnalogX();
    
    if (btnBlack.trg) {
        mPause = !mPause;
    }
    if (btnBlue.trg) {
        mShowInfo = !mShowInfo;
    }
    if (btnGreen.trg) {
        mSystem.setFPS(mSystem.getFPS() == 30 ? 60 : 30);
    }
    
    if (!mPause) {
        
        if (mInfo.ball == 0) {
            if (mInfo.dead) {
                if (++mInfo.cnt >= 60) {
                    mInfo.dead = false;
                    mInfo.next = true;
                }
            }
            
            if (mInfo.next) {
                if (mInfo.life > 0)
                {
                    mInfo.next = false;
                    mInfo.cnt = 0;

                    --mInfo.life;
                    
                    mBar.w = 24;
                    mBar.x = (VIEW_W - mBar.w) / 2;
                    mBar.speed = 2.f;
                    mBar.x_old = mBar.x;
                    
                    Ball &b = mBall[0];
                    b.enable = true;
                    b.hold = true;
                    b.w = 3;
                    b.h = 3;
                    b.x = mBar.x + ((mBar.w - b.w) / 2);
                    b.y = mBar.y - b.h;
                    b.speed = 0.8f;
                    const f32 theta = -(f32)M_PI_2 * 0.1f;
                    b.vx = b.speed * sin(theta);
                    b.vy = b.speed * -cos(theta);
                    b.x_old = b.x;
                    b.y_old = b.y;
                    ++mInfo.ball;
                }
                else
                {
                    mInfo.life = 3;
                    mInfo.score = mInfo.score_temp = 0;
                }
            }
        }
        
        if (mInfo.score_temp < mInfo.score) {
            ++mInfo.score_temp;
        }

        f32 dx = 0;
        
        if (btnLeft.on || btnRight.on) {
            dx = mBar.speed * analogX * deltaTime;
        }
    
        if (!mInfo.dead) {
            mBar.x_old = mBar.x;
            if (dx != 0.f) {
                mBar.x += dx;
                if (mBar.x < 0.f) mBar.x = 0.f;
                if (VIEW_W < mBar.x + mBar.w) mBar.x = static_cast<f32>(VIEW_W - mBar.w);
            }
            
            if (btnRed.trg) {
                for (u8 i = 0; i < BALL_MAX; ++i) {
                    Ball &b = mBall[i];
                    if (!b.enable)
                        continue;
                    b.hold = 0;
                }
            }
        }
        
        for (u8 i = 0; i < BALL_MAX; ++i) {
            Ball &b = mBall[i];
            if (!b.enable)
                continue;
            
            b.x_old = b.x;
            b.y_old = b.y;

            if (b.hold) {
                if (dx != 0.f) {
                    b.x += mBar.x - mBar.x_old;
                }
                continue;
            }
            
            bool flipX = false;
            bool flipY = false;            
            f32 nx = 0.f;
            f32 ny = 0.f;
            
            b.x += b.vx * deltaTime;
            b.y += b.vy * deltaTime;
            if (b.x < 0) { b.x = 0; flipX = true; }
            if (VIEW_W < b.x + b.w) { b.x = VIEW_W - b.w; flipX = true; }
            if (b.y < 0) { b.y = 0; flipY = true; }
            //if (VIEW_H < b.y + b.h) { b.y = VIEW_H - b.h; flipY = true; }
            
            if (VIEW_H < b.y) {
                b.enable = false;
                if (--mInfo.ball == 0) {
                    mInfo.dead = true;
                }
                continue;
            }
            
            const s8 bx = (s8)b.x;
            const s8 by = (s8)b.y;
            const f32 cx = b.x + (f32)b.w * 0.5f;
            const f32 cy = b.y + (f32)b.h * 0.5f;

            if (isCollided(bx, by, b.w, b.h, (s8)mBar.x, (s8)mBar.y, mBar.w, mBar.h)) {
                if (b.vy > 0.f) {
                    // 荳翫°繧�
                    b.y = mBar.y - (f32)b.h;
                    
                    const f32 w = (f32)mBar.w;
                    const f32 dw = w * 0.5f;
                    f32 theta = 0.f;
                    
                    if (cx < mBar.x + dw) {
                        theta = 0.8f * (f32)M_PI_2 * ((cx - (mBar.x + dw)) / dw);
                    }
                    if (mBar.x + w - dw < cx) {
                        theta = 0.8f * (f32)M_PI_2 * (cx - (mBar.x + w - dw)) / dw;
                    }
                    
                    const f32 speed = sqrt(b.vx * b.vx + b.vy * b.vy);
                    b.vx = speed * sin(theta);
                    b.vy = speed * -cos(theta);
                } else {
                    // 荳九°繧�
                    //b.y = mBar.y + mBar.h;
                    //flipY = true;
                }
            }
            
            for (u32 j = 0; j < BRICK_MAX; ++j) {
                Brick &brick = mBrick[j];
                if (!brick.enable)
                    continue;
                    
                if (isCollided(bx, by, b.w, b.h, brick.x, brick.y, brick.w, brick.h)) {
                    brick.enable = false;
                    mInfo.score += 10;
                    
                    if (brick.x < cx && cx < brick.x + brick.w)
                    {
                        flipY = true;
                        
#if 1
                        flipX = false;
                        nx = ny = 0.f;
#else                        
                        f32 ry = (brick.y + (brick.h / 2) < cy) ? 1.f : -1.f;
                        ny = (ny + ry) * 0.5f;
#endif
                    }
                    else if (brick.y < cy && cy < brick.y + brick.h)
                    {
                        flipX = true;

#if 1
                        flipY = false;
                        nx = ny = 0.f;
#else                        
                        f32 rx = (brick.x + (brick.w / 2) < cx) ? 1.f : -1.f;
                        nx = (nx + rx) * 0.5f;
#endif
                    }
#if 1
                    else if (flipX || flipY)
                    {
                        nx = ny = 0.f;
                    }
#endif
                    else
                    {
                        flipX = flipY = true;
                                                
                        const f32 px[4] = { brick.x, brick.x + brick.w, brick.x, brick.x + brick.w };
                        const f32 py[4] = { brick.y, brick.y, brick.y + brick.h, brick.y + brick.h };
                        
                        u8 nearest = 0;
                        f32 dist2 = 10000.f;
                        for (u8 k = 0; k < 4; ++k) {
                            f32 d2 = pow(px[k] - cx, 2.f) + pow(py[k] - cy, 2.f);
                            if (d2 < dist2) {
                                nearest = k;
                                dist2 = d2;
                            }
                        }
                        
                        f32 rx = (cx - b.vx) - px[nearest];
                        f32 ry = (cy - b.vy) - py[nearest];
                        const f32 ird = 1.f / sqrt(rx * rx + ry * ry);
                        rx *= ird;
                        ry *= ird;
                        
                        nx = (nx + rx) * 0.5f;
                        ny = (ny + ry) * 0.5f;
                    }
                }
            }
            

            const f32 nd2 = nx * nx + ny * ny;
            if (nd2 > 0.f)
            {
                const f32 ind = 1.f / sqrt(nd2);
                nx *= ind;
                ny *= ind;
                
                b.vx += 2.f * nx;
                b.vy += 2.f * ny;
                const f32 ivd = 1.f / sqrt(b.vx * b.vx + b.vy * b.vy);
                b.vx *= b.speed * ivd;
                b.vy *= b.speed * ivd;
                
                if (fabs(b.vx) < 0.1f) {
                    b.vx = 0.1f * ((rand() & 1) ? 1.f : -1.f);
                }
                if (fabs(b.vy) < 0.1f) {
                    b.vy = 0.1f * (b.vy > 0.f ? 1.f : -1.f);
                }
            }
            else
            {
                if (flipX) b.vx *= -1.f;
                if (flipY) b.vy *= -1.f;
            }
        }
    }
    
    
    mRender.clear();
    
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawFillRect(VIEW_L - BORDER_W, VIEW_T, BORDER_W, VIEW_H);
    mRender.drawFillRect(VIEW_R, VIEW_T, BORDER_W, VIEW_H);
    
    for (u8 i = 0; i < mInfo.life; ++i) {
        mRender.drawFillRect(VIEW_L + 1 + 7 * i, VIEW_T + 1, 6, 2);
    }
    mRender.drawString(VIEW_L + VIEW_W - 1 - 9 * FONT_SIZE, VIEW_T, "%9d", mInfo.score_temp);
    
    for (u32 i = 0; i < BRICK_MAX; ++i) {
        const Brick &b = mBrick[i];
        if (!b.enable)
            continue;

        mRender.setAnimSpeed(0);
        mRender.setDrawMode(DM_PAT_0);
        mRender.drawFillRect(VIEW_L + b.x, VIEW_T + b.y, b.w, 1);
        mRender.drawFillRect(VIEW_L + b.x, VIEW_T + b.y, 1, b.h);
        mRender.setDrawMode(DM_PAT_3);
        mRender.drawFillRect(VIEW_L + b.x, VIEW_T + b.y + b.h - 1, b.w, 1);
        mRender.drawFillRect(VIEW_L + b.x + b.w - 1, VIEW_T + b.y, 1, b.h);
        switch (b.pat) {
        case 1:
            mRender.setAnimSpeed(4);
            mRender.setDrawMode(DM_PAT_1);
            break;
        case 2:
            mRender.setAnimSpeed(0);
            mRender.setDrawMode(DM_PAT_6);
            break;
        default:
            mRender.setDrawMode(DM_POSITIVE);
            break;
        }
        mRender.drawFillRect(VIEW_L + b.x + 1, VIEW_T + b.y + 1, b.w - 2, b.h - 2);
    }
    
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawFillRect(VIEW_L + (s16)mBar.x, VIEW_T + (s16)mBar.y, mBar.w, mBar.h);
    
    for (u8 i = 0; i < BALL_MAX; ++i) {
        const Ball &b = mBall[i];
        if (!b.enable)
            continue;
        
        mRender.setAnimSpeed(0);
        mRender.setDrawMode(DM_PAT_0);
        mRender.drawFillRect(VIEW_L + (s16)b.x_old, VIEW_T + (s16)b.y_old, b.w, b.h);// 鬮倬�溘〒蜍輔°縺吶→阮�縺上※隕九∴縺ｪ縺�縺ｮ縺ｧ
        mRender.setDrawMode(DM_POSITIVE);
        mRender.drawFillRect(VIEW_L + (s16)b.x, VIEW_T + (s16)b.y, b.w, b.h);
    }

    if (mInfo.dead) {
#if 1
        switch (mInfo.cnt) {
        case 0:
            mRender.setDrawMode(DM_INVERSE | DM_PAT | PAT_4);
            break;
        case 1:
            mRender.setDrawMode(DM_INVERSE | DM_PAT | PAT_0);
            break;
        case 2:
            mRender.setDrawMode(DM_INVERSE | DM_PAT | PAT_3);
            break;
        default:
            mRender.setDrawMode(DM_INVERSE);
        }
        mRender.drawFillRect(VIEW_L, VIEW_T, VIEW_W, VIEW_H);
#else
        mRender.setDrawMode(DM_INVERSE);
        const s16 h = 4 * mInfo.cnt;
        mRender.drawFillRect(VIEW_L, VIEW_T + VIEW_H - h, VIEW_W, h);
#endif
    }

    if (mShowInfo) {
        mRender.setDrawMode(DM_INVERSE);
        mSystem.drawInfo(mRender);
    }
    
    mRender.present();
    super::update();
}
