#include "pgc1000.h"

using namespace pgc1000;

const Lcd::Desc Lcd::DefaultDesc = {
    D8,     // di
    D15,    // rw
    D10,    // e
    A4,     // db0
    A5,     // db1
    D2,     // db2
    D3,     // db3
    D4,     // db4
    D5,     // db5
    D6,     // db6
    D7,     // db7
    D11,    // cs1
    D12,    // cs2
    D9,     // res
};

Lcd::Lcd(const Desc &desc)
    : mDI(desc.di)
    , mRW(desc.rw)
    , mE(desc.e)
    , mDB(desc.db0, desc.db1, desc.db2, desc.db3, desc.db4, desc.db5, desc.db6, desc.db7)
    , mCS(desc.cs1, desc.cs2)
    , mRES(desc.res)
{
    mDB = 0;
    mDI = 0;
    mE = 0;
    mCS = 0;
    mRES = 0;
}

void Lcd::write(u8 cs, u8 di, u8 data)
{
    mCS = cs;
    mDI = di;
    mDB = data;
    
    mE = 1;
    wait_us(TIME_E_HIGH);
    mE = 0;
    wait_us(TIME_E_LOW);
}

void Lcd::init()
{
    mRES = 0;
    wait_us(TIME_RES_LOW);
    mRES = 1;
    wait_ms(30);
    
    displayStartLine(CS_BOTH, 0);
    displayOn(CS_BOTH);
}

void Lcd::blit(u8* pdata)
{
    setAddress(CS_BOTH, 0);
    for (u8 y = 0; y < PAGE_MAX; ++y) {
        setPage(CS_BOTH, y);
        for (u8 x = 0; x < LINE_MAX_2; ++x) {
            writeDisplayData(CS_LEFT, pdata[x * PAGE_MAX + y]);
        }
        for (u8 x = LINE_MAX_2; x < LINE_MAX; ++x) {
            writeDisplayData(CS_RIGHT, pdata[x * PAGE_MAX + y]);
        }
    }
}

void Lcd::blit(u8* pdata, u8* pdirty)
{
    for (u8 y = 0; y < PAGE_MAX; ++y) {
        setPage(CS_BOTH, y);
        const u8 mask = 1 << y;
#if 0//debug
        for (u8 x = 0; x < LINE_MAX_2; ++x) {
            writeDisplayData(CS_LEFT, (pdirty[x] & mask) ? 0xFF : 0x00);
        }
        for (u8 x = LINE_MAX_2; x < LINE_MAX; ++x) {
            writeDisplayData(CS_RIGHT, (pdirty[x] & mask) ? 0xFF : 0x00);
        }
#else
        for (u8 x = 0; x < LINE_MAX_2; ++x) {
            if (pdirty[x] & mask) {
                setAddress(CS_LEFT, x);
                writeDisplayData(CS_LEFT, pdata[x * PAGE_MAX + y]);
            }
        }
        for (u8 x = LINE_MAX_2; x < LINE_MAX; ++x) {
            if (pdirty[x] & mask) {
                setAddress(CS_RIGHT, x);
                writeDisplayData(CS_RIGHT, pdata[x * PAGE_MAX + y]);
            }
        }
#endif
    }
}


Render::Render()
    : mLcd()
	, mBufferIndex(0)
    , mDrawMode(DM_POSITIVE)
	, mFrameCount(0)
    , mAnimSpeed(4)
	, mAnimPat(PAT_0)
{
}

void Render::init()
{
    mDrawMode = DM_POSITIVE;
    mAnimPat = PAT_0;
    mAnimSpeed = 4;
    mFrameCount = 0;
    
    memset(mVRAM, 0, VRAM_SIZE * BUFFER_MAX);
    memset(mDirty, 0, sizeof(mDirty));
    mBufferIndex = 0;

    mLcd.init();
    mLcd.blit(mVRAM[mBufferIndex]);
}

void Render::update()
{
    ++mFrameCount;
}

void Render::present()
{
#if RENDER_ENABLE_DIRTY_CHECK
    //mLcd.blit(mVRAM[mBufferIndex], mDirty);
    //memset(mDirty, 0, sizeof(mDirty));
    const u8 prevIndex = (mBufferIndex + BUFFER_MAX - 1) % BUFFER_MAX;
    for (u8 x = 0; x < SCREEN_WIDTH; ++x) {
        u8 d = 0;
        for (u8 y = 0; y < 8; ++y) {
            const u32 i = (x << 3) + y;
            d |= (mVRAM[mBufferIndex][i] ^ mVRAM[prevIndex][i]) ? (1 << y) : 0;
        }
        mDirty[x] = d;
    }
    mLcd.blit(mVRAM[mBufferIndex], mDirty);
    const u8 nextIndex = (mBufferIndex + 1) % BUFFER_MAX;
    memcpy(mVRAM[nextIndex], mVRAM[mBufferIndex], VRAM_SIZE);
    mBufferIndex = nextIndex;
#else
    mLcd.blit(mVRAM);
#endif
}

bool Render::readPixel(u8 x, u8 y)
{
    return mVRAM[mBufferIndex][(x << 3) + (y >> 3)] & (1 << (y & 0x7));
}

void Render::unmap()
{
#if RENDER_ENABLE_DIRTY_CHECK
    //memset(mDirty, 0xFF, sizeof(mDirty));
#else
    /* nop */
#endif
}

void Render::clear()
{
    memset(mVRAM[mBufferIndex], 0, VRAM_SIZE);
#if RENDER_ENABLE_DIRTY_CHECK
    //memset(mDirty, 0xFF, sizeof(mDirty));
#endif
}

void Render::combine(u8 x, u8 y, u8 src, u8 mask)
{
    u8 &dst = mVRAM[mBufferIndex][(x << 3) + (y >> 3)];
#if RENDER_ENABLE_DIRTY_CHECK
    //const u8 old = dst;
#endif
    if (mDrawMode & DM_NEGATIVE)
        src = ~src;
    if (mDrawMode & 0xFF)
        mAnimPat = mDrawMode & 0xFF;
    if (mDrawMode & DM_PAT)
        src &= genAnimPat(x, y, src, mask);
    if (mDrawMode & DM_NPAT)
        src &= ~genAnimPat(x, y, src, mask);
    switch (mDrawMode & DM_OP_MASK) {
    case DM_POSITIVE:
    default:
        dst = (dst & mask) | (src & ~mask);
        break;
        
    case DM_ADD:
        dst = dst | (src & ~mask);
        break;

    case DM_INVERSE:
        dst = dst ^ (src & ~mask);
        break;
        
    case DM_SUBTRACT:
        dst = dst & ~(src & ~mask);
        break;
    }
#if RENDER_ENABLE_DIRTY_CHECK
    //if (dst != old)
    //    mDirty[x] |= 1 << (y >> 3);
#endif
}

u8 Render::genAnimPat(u8 x, u8 y, u8 src, u8 mask)
{
    u8 res = 0;
    const u8 anim = mFrameCount >> mAnimSpeed;
    for (u8 i = y & 0x7, s = (src & ~mask) >> i; s != 0; ++i, s >>= 1, ++y) {
        if (!(s & 0x1))
            continue;

        u8 pat = 0xFF;
        switch (mAnimPat) {
        case PAT_0:
            // . #
            // # .
            pat = ((x + y + anim) & 0x1) ? 0xFF : 0x00;
            break;
        case PAT_1:
            // . # # #
            // # # # .
            // # # . #
            // # . # #
            pat = ((x + y + anim) & 0x3) ? 0xFF : 0x00;
            break;
        case PAT_2:
            // . # # #
            // # . # .
            // # # . #
            // # . # .
            pat = ((y & 0x1) ? !((x + anim) & 0x1) : ((x + y + anim) & 0x3)) ? 0xFF : 0x00;
            //pat = (anim & 0x1) ? ~pat : pat;
            break;
        case PAT_3:
            // . #   >  # .   >  # #   >  # #
            // # #   >  # #   >  . #   >  # .
            pat = (((x + anim) & 0x1) || ((y + (anim >> 1)) & 0x1)) ? 0xFF : 0x00;
            break;
        case PAT_4:
            // inverse of PAT_3
            pat = !(((x + anim) & 0x1) || ((y + (anim >> 1)) & 0x1)) ? 0xFF : 0x00;
            break;
        case PAT_5:
            // . # # .  >  # . . #  >  # # # #  >  # # # #
            // # # # #  >  # # # #  >  . # # .  >  # . . #
            // # # # #  >  # # # #  >  . # # .  >  # . . #
            // . # # .  >  # . . #  >  # # # #  >  # # # #
            pat = ((((x >> 1) + x + anim) & 0x1) || (((y >> 1) + y + (anim >> 1)) & 0x1)) ? 0xFF : 0x00;
            // # . # .  >  . # . #
            // # . # .  >  . # . #
            // . # . #  >  # . # .
            // . # . #  >  # . # .
            pat |= ((x + (y >> 1) + (anim >> 2)) & 0x1) ? 0xFF : 0x00;
            //pat |= ((x + (y >> 1) + (anim >> 2)) & 0x1) ? (((x + y + anim) & 0x1) ? 0xFF : 0x00) : 0x00;
            // # # . .  >  # # . .
            // . . # #  >  . . # #
            // # # . .  >  # # . .
            // . . # #  >  . . # #
            //pat |= (((x >> 1) + y + (anim >> 2)) & 0x1) ? 0xFF : 0x00;
            break;
        case PAT_6:
            // inverse of PAT_5
            pat = ((((x >> 1) + x + anim) & 0x1) || (((y >> 1) + y + (anim >> 1)) & 0x1)) ? 0xFF : 0x00;
            pat |= ((x + (y >> 1) + (anim >> 2)) & 0x1) ? 0xFF : 0x00;
            //pat |= ((x + (y >> 1) + (anim >> 2)) & 0x1) ? (((x + y + anim) & 0x1) ? 0xFF : 0x00) : 0x00;
            pat ^= 0xFF;
            break;
        default:
            break;
        }
        res |= pat & (1 << i);
    }
    return res;
}

void Render::drawPixel(s16 x, s16 y)
{
    if (x < 0 || SCREEN_WIDTH <= x || y < 0 || SCREEN_HEIGHT <= y)
        return;
    
    const u8 mask = 1 << (y & 0x7);
#if 1
    combine(x, y, mask, ~mask);
#else
    u8 &dst = mVRAM[(x << 3) + (y >> 3)];
    switch (mDrawMode) {
    case DM_POSITIVE:
    default:
        dst |= mask;
        break;
    case DM_NEGATIVE:
        dst &= ~mask;
        break;
    case DM_INVERSE:
        dst ^= mask;
        break;
    }
#endif
}

void Render::drawFillRect(s16 x, s16 y, u8 w, u8 h)
{
    const s16 l = x + w;
    const s16 b = y + h;
    if (l < 0 || b < 0)
        return;
    
    for (s16 i = x; i < l; ++i) {
        for (s16 j = y; j < b; ++j) {
            drawPixel(i, j);
        }
    }
}

void Render::drawImage(s16 x, s16 y, const u8* img, u8 w, u8 h)
{
    const s16 l = x + w;
    const s16 b = y + h;
    if (l < 0 || SCREEN_WIDTH <= x || b < 0 || SCREEN_HEIGHT <= y)
        return;
    
    u8 shift1 = y & 0x7;
    u8 shift2 = h - (b & 0x7);
    u8 mask1 = ~((0xFF >> (8 - h)) << shift1) & 0xFF;
    u8 mask2 = (0xFF << (h - shift2)) & 0xFF;

    for (u8 i = 0; i < w; ++i) {
        const s16 xi = x + i;
        if (xi < 0)
            continue;
        if (xi >= SCREEN_WIDTH)
            break;
            
        const u8 page = img[i];
        if (0 <= y)
            combine(xi, y, page << shift1, mask1);
        if (b < SCREEN_HEIGHT && (y >> 3) != (b >> 3))
            combine(xi, b & 0xF8, page >> shift2, mask2);
    }
}

void Render::drawImageShift(s8 sx, s8 sy, s16 x, s16 y, const u8* img, u8 w, u8 h)
{
    const s16 l = x + w;
    const s16 b = y + h;
    if (l < 0 || SCREEN_WIDTH <= x || b < 0 || SCREEN_HEIGHT <= y)
        return;
    
    u8 shift1 = y & 0x7;
    u8 shift2 = h - (b & 0x7);
    u8 mask1 = ~((0xFF >> (8 - h)) << shift1) & 0xFF;
    u8 mask2 = (0xFF << (h - shift2)) & 0xFF;

    sx %= w;
    sy %= 8;
    if (sy < 0)
        sy += 8;

    for (u8 i = 0; i < w; ++i) {
        const s16 xi = x + i;
        if (xi < 0)
            continue;
        if (xi >= SCREEN_WIDTH)
            break;
            
        u8 page = img[(i + sx + w) % w];
        page = (page << sy) | (page >> (8 - sy));
        if (0 <= y)
            combine(xi, y, page << shift1, mask1);
        if (b < SCREEN_HEIGHT && (y >> 3) != (b >> 3))
            combine(xi, b & 0xF8, page >> shift2, mask2);
    }
}

void Render::drawString(s16 x, s16 y, const char* fmt, ...)
{  
    if (SCREEN_WIDTH <= x || SCREEN_HEIGHT <= y)
        return;

    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    
    s16 b = y + FONT_SIZE;
    s16 cx = 0;
    u8 shift1, shift2, mask1, mask2;
    for (u8 i = 0; buf[i]; ++i, ++cx) {
        const char c = buf[i];
        
        if (i == 0 || c == '\n') {
            if (c == '\n') {
                y += FONT_SIZE;
                if (y >= SCREEN_HEIGHT)
                    break;
                b = y + FONT_SIZE;
                cx = -1;
            }

            shift1 = y & 0x7;
            shift2 = FONT_SIZE - (b & 0x7);
            mask1 = ~((0xFF >> (8 - FONT_SIZE)) << shift1) & 0xFF;
            mask2 = (0xFF << (FONT_SIZE - shift2)) & 0xFF;
        }

        if (b < 0 || c < 0x20 || 0x7F < c)
            continue;

        const s16 fx = x + cx * FONT_SIZE;
        if (fx + FONT_SIZE < 0)
            continue;

        const u8 fi = c - 0x20;
        for (u8 j = 0; j < FONT_SIZE; ++j) {
            const s16 fxj = fx + j;
            if (fxj < 0)
                continue;
            if (fxj >= SCREEN_WIDTH)
                break;
                
            const u8 page = j == 0 ? 0 : font[fi][j - 1];
            if (0 <= y)
                combine(fxj, y, page << shift1, mask1);
            if (b < SCREEN_HEIGHT && (y >> 3) != (b >> 3))
                combine(fxj, b & 0xF8, page >> shift2, mask2);
        }
    }
}


void System::BtnInfo::update(bool now)
{
    on = now;
    trg = on && !old;
    rel = old && !on;
    old = on;

    rep = trg;
    if (on) {
        if (++count >= (rep1 ? rep_count : rep1_count)) {
            rep = true;
            rep1 = true;
            count = 0;
        }
    } else {
        rep1 = false;
        count = 0;
    }
}

static const f32 MICROSEC_PER_CLOCK = 1000000.f / CLOCKS_PER_SEC;
//static const f32 MICROSEC_PER_FRAME = 1000000.f / System::FPS;

const System::Desc System::DefaultDesc = {
    LED1,   // led1
    A1,     // btn_black
    A0,     // btn_red
    A2,     // joy_x
    A3,     // joy_y
    USBTX,  // serial_tx
    USBRX,  // serial_rx
#if ENABLE_PGC2000
    PC_4,   // led2
    PC_2,   // btn_green
    PC_3,   // btn_blue
    PB_9,   // btn_white
    PB_8,   // btn_orange
    PB_12,  // dip1
    PB_13,  // dip2
    PB_14,  // dip3
    PB_15,  // dip4
#endif
};

System::System(const Desc &desc)
    : FPS(0)
	, MICROSEC_PER_FRAME(0.f)
	, mFrameWait(true)
    , mFrameCount(0)
    , mPrevTime(0)
    , mFrameTime(0.f)
    , mDeltaTime(0.f)
    , mFPS(0.f)
    , mFrameCountTemp(0)
    , mPrevTimeTemp(0)
	, mAnalogX(0.f)
	, mAnalogY(0.f)
    , mLed1(desc.led1)
    , mBtnBlack(desc.btn_black, PullUp)
    , mBtnRed(desc.btn_red, PullUp)
    , mJoyX(desc.joy_x)
    , mJoyY(desc.joy_y)
    , mSerial(desc.serial_tx, desc.serial_rx)
#if ENABLE_PGC2000
    , mLed2(desc.led2)
    , mBtnGreen(desc.btn_green, PullUp)
    , mBtnBlue(desc.btn_blue, PullUp)
    , mBtnWhite(desc.btn_white, PullUp)
    , mBtnOrange(desc.btn_orange, PullUp)
    , mDip(desc.dip1, desc.dip2, desc.dip3, desc.dip4)
#endif
{
}

void System::init()
{
    setFPS(30);

    mSerial.baud(115200);
    
    mFrameWait = true;
    mFrameTime = mFPS = 0.f;
    mFrameCount = mFrameCountTemp = 0;
    mPrevTime = mPrevTimeTemp = clock();

    memset(&mBtnInfoBlack, 0, sizeof(BtnInfo));
    memset(&mBtnInfoRed, 0, sizeof(BtnInfo));
#if ENABLE_PGC2000
    memset(&mBtnInfoGreen, 0, sizeof(BtnInfo));
    memset(&mBtnInfoBlue, 0, sizeof(BtnInfo));
    memset(&mBtnInfoWhite, 0, sizeof(BtnInfo));
    memset(&mBtnInfoOrange, 0, sizeof(BtnInfo));
#endif
    memset(&mBtnInfoLeft, 0, sizeof(BtnInfo));
    memset(&mBtnInfoRight, 0, sizeof(BtnInfo));
    memset(&mBtnInfoUp, 0, sizeof(BtnInfo));
    memset(&mBtnInfoDown, 0, sizeof(BtnInfo));
    mBtnInfoBlack.rep1_count = mBtnInfoRed.rep1_count = 15;
    mBtnInfoBlack.rep_count = mBtnInfoRed.rep_count = 5;

    mBtnInfoLeft.rep1_count = mBtnInfoRight.rep1_count = 5;
    mBtnInfoLeft.rep_count = mBtnInfoRight.rep_count = 3;
    
    mBtnInfoUp.rep1_count = mBtnInfoDown.rep1_count = 5;
    mBtnInfoUp.rep_count = mBtnInfoDown.rep_count = 2;    

#if ENABLE_PGC2000
    mDip.mode(PullUp);
#endif
}

void System::update()
{
    ++mFrameCount;
    //mLed1 = mFrameCount & 1;
    //mSerial.printf("%d ", mFrameCount);
    
    mBtnInfoBlack.update(!mBtnBlack.read());
    mBtnInfoRed.update(!mBtnRed.read());
#if ENABLE_PGC2000
    mBtnInfoGreen.update(!mBtnGreen.read());
    mBtnInfoBlue.update(!mBtnBlue.read());
    mBtnInfoWhite.update(!mBtnWhite.read());
    mBtnInfoOrange.update(!mBtnOrange.read());
#endif
    mAnalogX = mJoyX.read() - 0.5f;
    mBtnInfoLeft.update(mAnalogX < -0.1f);
    mBtnInfoRight.update(mAnalogX > 0.1f);
    mAnalogY = 0.5f - mJoyY.read();
    mBtnInfoUp.update(mAnalogY < -0.1f);
    mBtnInfoDown.update(mAnalogY > 0.1f);
    wait_us(100);
    
#if SYSTEM_ENABLE_MEASURE_FPS
    if (++mFrameCountTemp > FPS) {
        const f32 elapsed = static_cast<f32>(clock() - mPrevTimeTemp) * MICROSEC_PER_CLOCK;
        const f32 us = elapsed / FPS;
        mFPS = 1000000.f / us;
        mFrameCountTemp = 0;
        mPrevTimeTemp = clock();
    }
#endif
}

void System::frameWait()
{
#if SYSTEM_ENABLE_FRAME_WAIT
    const f32 elapsed = static_cast<f32>(clock() - mPrevTime) * MICROSEC_PER_CLOCK;
    mFrameTime = elapsed * 0.001f;
    if (elapsed < MICROSEC_PER_FRAME) {
        mLed1 = 1;
        wait_us(MICROSEC_PER_FRAME - elapsed);
    }
    mLed1 = 0;
    mDeltaTime = static_cast<f32>(clock() - mPrevTime) * MICROSEC_PER_CLOCK / MICROSEC_PER_FRAME;
    mPrevTime = clock();
#endif
}

void System::drawInfo(Render &render)
{
    const u32 dm = render.getDrawMode();
    render.drawString(0, 0, "%.1fFPS", mFPS);
    render.drawString(0, 6, "%.1fms", mFrameTime);
    render.setDrawMode(DM_POSITIVE);
    render.drawFillRect(0, 12, 12, 12);
    render.setDrawMode(DM_NEGATIVE);
    render.drawFillRect(12, 12, 12, 12);
    render.setDrawMode(DM_INVERSE);
    render.drawString(3 + mAnalogX * 6, 16 + mAnalogY * 6, "+");
    render.drawString(12, 16, "%c%c", (mBtnInfoBlack.on ? (mBtnInfoBlack.rep ? 'X' : 'O') : '-'), (mBtnInfoRed.on ? (mBtnInfoRed.rep ? 'X' : 'O') : '-'));
    render.setDrawMode(dm);
}

void System::printf(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    mSerial.vprintf(fmt, args);
    va_end(args);
}

void System::scanf(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    mSerial.vscanf(fmt, args);
    va_end(args);
}


#if ENABLE_PGC2000
const Psg::Desc Psg::DefaultDesc = {
    PC_6,   // wr
    PC_5,   // cs
    PA_12,  // a0
    PC_8,   // ic
    PB_7,   // d0
    PA_15,  // d1
    PC_9,   // d2
    PB_2,   // d3
    PC_12,  // d4
    PA_11,  // d5
    PC_10,  // d6
    PC_11,  // d7
};

Psg::Psg(const Desc &desc)
    : mAddress(0xFF)
	, mWR(desc.wr)
    , mCS(desc.cs)
    , mA0(desc.a0)
    , mIC(desc.ic)
    , mD(desc.d0, desc.d1, desc.d2, desc.d3, desc.d4, desc.d5, desc.d6, desc.d7)
{
}

void Psg::init()
{
    mAddress = 0xFF;
    mCS = 0;
    mWR = 1;
    mIC = 0;
    wait_us(5);
    mIC = 1;
}

void Psg::writeAddress(u8 adr)
{
    //mCS = 0;
    mAddress = 0xFF;
    mD = adr;
    mA0 = 0;
    mWR = 0;
    wait_us(1);
    mWR = 1;
    wait_us(1);
}

void Psg::writeData(u8 data)
{
    //mCS = 0;
    mD = data;
    mA0 = 1;
    mWR = 0;
    wait_us(1);
    mWR = 1;
    wait_us(1);
}

void Psg::write(u8 adr, u8 data)
{
    //mCS = 0;
    if (mAddress != adr) {
        mD = mAddress = adr;
        mA0 = 0;
        mWR = 0;
        wait_us(1);
        mWR = 1;
        wait_us(1);
    }
    mD = data;
    mA0 = 1;
    mWR = 0;
    wait_us(1);
    mWR = 1;
    wait_us(1);
}


Sound::Sound()
    : mPsg()
{
}

void Sound::init()
{
    mPsg.init();
}
#endif


App::App()
{
}

App::~App()
{
}

void App::init()
{
    mSystem.init();
    mRender.init();
#if ENABLE_PGC2000
    mSound.init();
#endif
}

void App::exec()
{
    init();
    while (true) {
        update();
#if SYSTEM_ENABLE_FRAME_WAIT
        if (mSystem.isFrameWait())
            mSystem.frameWait();
#endif
    }
}

void App::update()
{
    mRender.update();
    mSystem.update();
}
