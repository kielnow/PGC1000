#include <string>
#include "Tetris.h"

using namespace app;

namespace {
    
    enum {
        OFFSET_X = 49,
        OFFSET_Y = 2,
        BORDER_WIDTH = 2,
        BLOCK_SIZE = 3,
    };
    
    inline f32 frand() { return static_cast<f32>(rand()) / RAND_MAX; }
    
}

const u32 Tetris::MINO[Tetris::TYPE_MAX][Tetris::ROT_MAX] = {
    { 0x0F00, 0x2222, 0x00F0, 0x4444 }, // TYPE_I
    { 0x0710, 0x2260, 0x4700, 0x3220 }, // TYPE_J
    { 0x0740, 0x6220, 0x1700, 0x2230 }, // TYPE_L
    { 0x0360, 0x0231, 0x0036, 0x0462 }, // TYPE_S
    { 0x0630, 0x0132, 0x0063, 0x0264 }, // TYPE_Z
    { 0x0720, 0x2620, 0x2700, 0x2320 }, // TYPE_T
    { 0x0660, 0x0660, 0x0660, 0x0660 }, // TYPE_O
};

Tetris::Tetris()
{
}

void Tetris::init()
{
    super::init();
    
    std::memset(mBoard, 0, sizeof(mBoard));
    mMino.type = TYPE_J;
    mMino.rot = ROT_0;
    mMino.x = 3;
    mMino.y = -3;
    
    mInfo.fall_count = 0;
    mInfo.fall_speed = 24;
    
    std::memset(mParticle, 0, sizeof(mParticle));
}

void Tetris::update()
{
    const System::BtnInfo &btnBlack = mSystem.getBtnBlack();
    const System::BtnInfo &btnRed = mSystem.getBtnRed();
    const System::BtnInfo &btnLeft = mSystem.getBtnLeft();
    const System::BtnInfo &btnRight = mSystem.getBtnRight();
    const System::BtnInfo &btnDown = mSystem.getBtnDown();
    //const f32 analogX = mSystem.getAnalogX();
    //const f32 analogY = mSystem.getAnalogY();
    
    s8 dx = 0;
    u8 rot = mMino.rot;
    bool fall = false;
    bool fix = false;
    
    if (++mInfo.fall_count >= mInfo.fall_speed) {
        mInfo.fall_count = 0;
        fall = true;
    }
    
    fall |= btnDown.rep;
    if (btnLeft.rep) {
        dx = -1;
    } else if (btnRight.rep) {
        dx = 1;
    }
    if (btnRed.rep) {
        rot = (rot + ROT_MAX - 1) % ROT_MAX;
    }
    if (btnBlack.rep) {
        rot = (rot + 1) % ROT_MAX;
    }
    
    if (dx) {
        if (!isCollided(mMino.type, mMino.rot, mMino.x + dx, mMino.y)) {
            mMino.x += dx;
        }
    }
    
    if (mMino.rot != rot) {
        if (!isCollided(mMino.type, rot, mMino.x, mMino.y)) {
            mMino.rot = rot;
        }
    }
    
    const bool ground = isCollided(mMino.type, mMino.rot, mMino.x, mMino.y + 1);
    if (fall) {
        if (ground) {
            fix = true;
        } else {
            mMino.y += 1;
        }
    }
    
    if (fix) {
        fixMino();
        if (clearLine()) {
            dropLine();
        }

        // spawn
        mMino.type = rand() % TYPE_MAX;
        mMino.rot = ROT_0;
        mMino.x = 3;
        mMino.y = -3;
        
        if (isCollided(mMino.type, mMino.rot, mMino.x, mMino.y)) {
            // gameover
            memset(mBoard, 0, sizeof(mBoard));
        }
    }
    
    // clear
    mRender.setDrawMode(DM_NEGATIVE);
    mRender.drawFillRect(0, 0, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT);
    //mRender.drawFillRect(OFFSET_X - BORDER_WIDTH, OFFSET_Y - BORDER_WIDTH, BOARD_WIDTH * BLOCK_SIZE + 2 * BORDER_WIDTH, BOARD_HEIGHT * BLOCK_SIZE + 2 * BORDER_WIDTH);
    
    // border
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawFillRect(OFFSET_X - BORDER_WIDTH, OFFSET_Y - BORDER_WIDTH, BORDER_WIDTH, BOARD_HEIGHT * BLOCK_SIZE + 2 * BORDER_WIDTH);
    mRender.drawFillRect(OFFSET_X + BOARD_WIDTH * BLOCK_SIZE, OFFSET_Y - BORDER_WIDTH, BORDER_WIDTH, BOARD_HEIGHT * BLOCK_SIZE + 2 * BORDER_WIDTH);
    mRender.drawFillRect(OFFSET_X, OFFSET_Y - BORDER_WIDTH, 3 * BLOCK_SIZE, BORDER_WIDTH);
    mRender.drawFillRect(OFFSET_X + 7 * BLOCK_SIZE, OFFSET_Y - BORDER_WIDTH, 3 * BLOCK_SIZE, BORDER_WIDTH);
    mRender.drawFillRect(OFFSET_X, OFFSET_Y + (BOARD_HEIGHT - BOARD_TOP) * BLOCK_SIZE, BOARD_WIDTH * BLOCK_SIZE, BORDER_WIDTH);
    
    drawBoard();
    drawMino();
    updateAndDrawParticle();

    mRender.setDrawMode(DM_POSITIVE);
    mSystem.drawInfo(mRender);
    
    mRender.present();    
    super::update();
}

bool Tetris::isCollided(u8 type, u8 rot, s8 x, s8 y)
{
    for (u8 i = 0; i < 4; ++i) {
        const u8 line = (MINO[type][rot] >> (i * 4)) & 0xF;
        const s8 l = line ? ((line & 0x3) ? ((line & 0x1) ? 0 : 1) : ((line & 0x4) ? 2 : 3)) : 4;
        const s8 r = line ? ((line & 0xC) ? ((line & 0x8) ? 3 : 2) : ((line & 0x2) ? 1 : 0)) : 0;
        if ((x + l < 0 || BOARD_WIDTH <= x + r) || (line && BOARD_TOP + y + i >= BOARD_HEIGHT) || (mBoard[BOARD_TOP + y + i] & (x > 0 ? (line << x) : (line >> -x)))) {
            return true;
        }
    }
    return false;
}

void Tetris::fixMino()
{
    for (u8 i = 0; i < 4; ++i) {
        const u8 line = (MINO[mMino.type][mMino.rot] >> (i * 4)) & 0xF;
        if (line) {
            mBoard[BOARD_TOP + mMino.y + i] |= (mMino.x > 0 ? (line << mMino.x) : (line >> -mMino.x));
        }
    }
}

u8 Tetris::clearLine()
{
    u8 n = 0;
    for (s8 i = BOARD_HEIGHT - 1; i >= 0; --i) {
        if (!mBoard[i]) {
            break;
        }
        if (mBoard[i] == 0x03FF) {
            mBoard[i] = 0;
            ++n;
            
            for (u8 j = 0; j < BOARD_WIDTH; ++j) {
                emitParticle(static_cast<f32>(OFFSET_X + j * BLOCK_SIZE), static_cast<f32>(OFFSET_Y + i * BLOCK_SIZE));
            }
        }
    }
    return n;
}

void Tetris::dropLine()
{
    u8 c = mBoard[BOARD_HEIGHT - 1] ? 0 : 1;
    for (s8 i = BOARD_HEIGHT - 2; i >= 0; --i) {
        if (!mBoard[i]) {
            ++c;
            continue;
        }
        if (c) {
            mBoard[i + c] = mBoard[i];
            mBoard[i] = 0;
        }
    }
}

void Tetris::emitParticle(f32 x, f32 y)
{
    u8 c = 0;
    for (u8 i = 0; i < PARTICLE_MAX; ++i) {
        if (mParticle[i].enable) {
            continue;
        }
            
        mParticle[i].enable = true;
        mParticle[i].x = x + ((c & 1) ? 1.f : 0.f);
        mParticle[i].y = y + ((c & 2) ? 1.f : 0.f);
        mParticle[i].vx = 12.f * (frand() - 0.5f);
        mParticle[i].vy = -1.5f * (frand() + 2.f);
        mParticle[i].w = mParticle[i].h = 2;

        if (++c >= 1) {
            break;
        }
    }
}

void Tetris::updateAndDrawParticle()
{
    mRender.setDrawMode(DM_INVERSE);
    for (u8 i = 0; i < PARTICLE_MAX; ++i) {
        if (!mParticle[i].enable) {
            continue;
        }
        
        mRender.drawFillRect(static_cast<u8>(mParticle[i].x), static_cast<u8>(mParticle[i].y), mParticle[i].w, mParticle[i].h);
        
        mParticle[i].x += mParticle[i].vx;
        mParticle[i].y += mParticle[i].vy;
        mParticle[i].vy += 0.2f;
        if (mParticle[i].x < 0.f || Render::SCREEN_WIDTH <= mParticle[i].x + mParticle[i].w)
            mParticle[i].vx *= -1.f;
        if (Render::SCREEN_HEIGHT <= mParticle[i].y) {
            mParticle[i].enable = false;
        } else {
            mRender.drawFillRect(static_cast<u8>(mParticle[i].x), static_cast<u8>(mParticle[i].y), mParticle[i].w, mParticle[i].h);
        }
    }
}

void Tetris::drawBoard()
{
    mRender.setDrawMode(DM_PAT_2);
    for (u8 y = 0; y < BOARD_HEIGHT; ++y) {
        for (u8 x = 0; x < BOARD_WIDTH; ++x) {
            if (mBoard[y] & (1 << x)) {
                mRender.drawFillRect(OFFSET_X + x * BLOCK_SIZE, OFFSET_Y + (y - BOARD_TOP) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            }
        }
    }
}

void Tetris::drawMino()
{
    mRender.setDrawMode(DM_PAT_2);
    for (u8 y = 0; y < 4; ++y) {
        for (u8 x = 0; x < 4; ++x) {
            if (MINO[mMino.type][mMino.rot] & (1 << (y * 4 + x))) {
                mRender.drawFillRect(OFFSET_X + (mMino.x + x) * BLOCK_SIZE, OFFSET_Y + (mMino.y + y) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            }
        }
    }
}
