#include "Reversi.h"

using namespace app;

namespace {
    
    enum {
        CELL_SIZE   = 8,
        BOARD_SIZE  = CELL_SIZE * Reversi::CELL_NUM,
        BOARD_L     = (Render::SCREEN_WIDTH - BOARD_SIZE) / 2,
        BOARD_T     = 0,
        BOARD_R     = BOARD_L + BOARD_SIZE,
        BOARD_B     = BOARD_T + BOARD_SIZE,
        LINE_W      = 1,
    };

    const u8 BLINK_TIME = 24;
    
}

void Reversi::init()
{
    super::init();
    
    mSystem.setFPS(60);
    
    System::BtnInfo &btnLeft = mSystem.getBtnLeft();
    System::BtnInfo &btnRight = mSystem.getBtnRight();
    System::BtnInfo &btnUp = mSystem.getBtnUp();
    System::BtnInfo &btnDown = mSystem.getBtnDown();
    btnLeft.rep1_count = btnRight.rep1_count = btnUp.rep1_count = btnDown.rep1_count = 24;
    btnLeft.rep_count  = btnRight.rep_count  = btnUp.rep_count  = btnDown.rep_count  = 8;
    
    mPause = false;
    mShowInfo = false;
    
    std::memset(&mInfo, 0, sizeof(mInfo));
    mInfo.p = CELL_BLACK;
    mInfo.blackNum = mInfo.whiteNum = 2;
    
    std::memset(mBoard, 0, sizeof(mBoard));
    mBoard[3][3].state = CELL_BLACK;
    mBoard[3][4].state = CELL_WHITE;
    mBoard[4][3].state = CELL_WHITE;
    mBoard[4][4].state = CELL_BLACK;
    
    ai.p = CELL_WHITE;
    ai.think = false;
    
    checkPass(mBoard, mInfo.p);
 
}

void Reversi::update()
{
    const System::BtnInfo &btnBlack = mSystem.getBtnBlack();
    const System::BtnInfo &btnRed = mSystem.getBtnRed();
    const System::BtnInfo &btnGreen = mSystem.getBtnGreen();
    const System::BtnInfo &btnBlue = mSystem.getBtnBlue();
    const System::BtnInfo &btnLeft = mSystem.getBtnLeft();
    const System::BtnInfo &btnRight = mSystem.getBtnRight();
    const System::BtnInfo &btnUp = mSystem.getBtnUp();
    const System::BtnInfo &btnDown = mSystem.getBtnDown();
    
    if (btnBlue.trg) {
        mShowInfo = !mShowInfo;
    }

    if (mSystem.isFrameWait()) {
        if (btnGreen.on)
            mSystem.setFrameWait(false);
    } else {
        if (btnGreen.rel)
            mSystem.setFrameWait(true);
    }
    
    if (++mInfo.blinkCnt > BLINK_TIME * 2) mInfo.blinkCnt = 0;
    if (++mInfo.deltaSCnt1 > 6) {
        mInfo.deltaSCnt = (mInfo.deltaSCnt + 1) % 7;
        mInfo.deltaSCnt1 = 0;
    }
    if (++mInfo.deltaYCnt1 > 12) {
        mInfo.deltaYCnt = (mInfo.deltaYCnt + 1) % 4;
        mInfo.deltaYCnt1 = 0;
    }
    
    if (mInfo.animeFlg) {
        bool flg = false;
        for (u8 y = 0; y < CELL_NUM; ++y) {
            for (u8 x = 0; x < CELL_NUM; ++x) {
                if (mBoard[y][x].animeCnt != 0) {
                    flg = true;
                    if (++mBoard[y][x].animeCnt1 > 6) {
                        --mBoard[y][x].animeCnt;
                        mBoard[y][x].animeCnt1 = 0;
                    }
                }
            }
        }
        mInfo.animeFlg = flg;
    } else if (mInfo.passFlg) {
        if (btnRed.trg) {
            mInfo.passFlg = false;
            mInfo.p = mInfo.p % 2 + 1;
            if ((mInfo.endFlg = checkPass(mBoard, mInfo.p)) == true) {
                std::memset(mBoard, 0, sizeof(mBoard));
                mInfo.endAnimeCnt = 0;
                mInfo.endAnimeCnt1 = 0;
                mInfo.endAnimeFlg = true;
            }
        }
    } else if (mInfo.endFlg) {
        if (mInfo.endAnimeFlg) {
            if (++mInfo.endAnimeCnt1 > 8) {
                bool flg = false;
                if (mInfo.blackNum >= mInfo.endAnimeCnt + 1) {
                    mBoard[mInfo.endAnimeCnt / CELL_NUM][mInfo.endAnimeCnt % CELL_NUM].state = CELL_BLACK;
                    flg = true;
                }
                if (mInfo.whiteNum >= mInfo.endAnimeCnt + 1) {
                    mBoard[CELL_NUM - 1 - mInfo.endAnimeCnt / CELL_NUM][CELL_NUM - 1 - mInfo.endAnimeCnt % CELL_NUM].state = CELL_WHITE;
                    flg = true;
                }
                
                ++mInfo.endAnimeCnt;
                mInfo.endAnimeCnt1 = 0;
                
                if (flg == false) {
                    mInfo.endAnimeFlg = false;
                }
            }
        }
        
        if (btnRed.trg) {
            if (mInfo.endAnimeFlg) {
                mInfo.endAnimeFlg = false;
                
                bool flg;
                u8 i = 0;
                do {
                    flg = false;
                    
                    ++i;
                    if (mInfo.blackNum >= i) {
                        mBoard[i / CELL_NUM][i % CELL_NUM].state = CELL_BLACK;
                        flg = true;
                    }
                    if (mInfo.whiteNum >= i) {
                        mBoard[CELL_NUM - 1 - i / CELL_NUM][CELL_NUM - 1 - i % CELL_NUM].state = CELL_WHITE;
                        flg = true;
                    }
                }while (!flg);

            } else {
                init();
            }
        }
    } else {
        u8 lp = mInfo.p;
        u8 lcx = mInfo.cx;
        u8 lcy = mInfo.cy;

        if (ai.p == mInfo.p) {
            if (ai.think == false) {
                aiThink();
                ai.think = true;
                ai.put = false;
                ai.moveCnt = 0;
            }
            
            if ((ai.cx == mInfo.cx) && (ai.cy == mInfo.cy)) {
                ai.put = true;
            } else {
                if (++ai.moveCnt > 12) {
                    ai.moveCnt = 0;
                    if (ai.cx != mInfo.cx) {
                        mInfo.cx += (mInfo.cx < ai.cx)? 1: -1;
                    } else {
                        mInfo.cy += (mInfo.cy < ai.cy)? 1: -1;
                    }
                }
            }
        } else {
            mInfo.cx = (mInfo.cx + CELL_NUM - (btnLeft.rep? 1: 0) + (btnRight.rep? 1: 0)) % CELL_NUM;
            mInfo.cy = (mInfo.cy + CELL_NUM - (btnUp.rep? 1: 0) + (btnDown.rep? 1: 0)) % CELL_NUM;
            
            if (btnBlack.trg) {
                for (u8 i = 1; i < CELL_NUM * CELL_NUM; ++i) {
                    u8 j = (i + mInfo.cy * CELL_NUM + mInfo.cx) % (CELL_NUM * CELL_NUM);
                    if (mBoard[j / CELL_NUM][j % CELL_NUM].putFlg) {
                        mInfo.cx = j % CELL_NUM;
                        mInfo.cy = j / CELL_NUM;
                        break;
                    }
                }
            }
            
        }
        
        if (((ai.p != mInfo.p) && btnRed.trg) || ((ai.p == mInfo.p) && ai.put)) {
            ai.think = false;
            ai.put = false;

            u8 num = put(mBoard, mInfo.cx, mInfo.cy, mInfo.p);
            if (num != 0) {
                if (mInfo.p == CELL_BLACK) {
                    ++mInfo.blackNum;
                    mInfo.blackNum += num;
                    mInfo.whiteNum -= num;
                } else {
                    ++mInfo.whiteNum;
                    mInfo.blackNum -= num;
                    mInfo.whiteNum += num;
                }
                
                mInfo.p = mInfo.p % 2 + 1;
                mInfo.animeFlg = true;
                
                mInfo.passFlg = checkPass(mBoard, mInfo.p);
                if (mInfo.passFlg && ((mInfo.endFlg = checkPass(mBoard, mInfo.p % 2 + 1)) == true)) {
                    std::memset(mBoard, 0, sizeof(mBoard));
                    mInfo.endAnimeCnt = 0;
                    mInfo.endAnimeCnt1 = 0;
                    mInfo.endAnimeFlg = true;
                    mInfo.passFlg = false;
                } else checkPass(mBoard, mInfo.p);
            }
        }
        
        if ((lcx != mInfo.cx) || (lcy != mInfo.cy) || (lp != mInfo.p)) {
            checkBlink(mBoard, mInfo.cx, mInfo.cy, mInfo.p);
            mInfo.blinkCnt = 0;
        }
    }

    
    mRender.clear();
    
    //score
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawString(BOARD_L - FONT_SIZE * 4, BOARD_B - FONT_SIZE * 3, "%02u", mInfo.blackNum);
    mRender.drawString(BOARD_R + FONT_SIZE * 2, BOARD_B - FONT_SIZE * 3, "%02u", mInfo.whiteNum);
    mRender.setDrawMode(DM_INVERSE);
    mRender.drawFillRect(BOARD_L - FONT_SIZE * 5, BOARD_B - FONT_SIZE * 4, FONT_SIZE * 4, FONT_SIZE * 3);
    mRender.drawFillRect(BOARD_R + FONT_SIZE * 1, BOARD_B - FONT_SIZE * 4, FONT_SIZE * 4, FONT_SIZE * 3);
    mRender.drawFillRect(BOARD_R + FONT_SIZE * 1 + 1, BOARD_B - FONT_SIZE * 4 + 1, FONT_SIZE * 4 - 2, FONT_SIZE * 3 - 2);
    
    //player
    mRender.setDrawMode(DM_POSITIVE);
    u8 tmpx = (mInfo.p == CELL_BLACK)? BOARD_L - FONT_SIZE * 3: BOARD_R + FONT_SIZE * 3;
    u8 tmpy = BOARD_B - FONT_SIZE * 6 + mInfo.deltaYCnt;
    mRender.drawLine(tmpx, tmpy + FONT_SIZE, tmpx - 3 + mInfo.deltaSCnt, tmpy);
    mRender.drawLine(tmpx, tmpy + FONT_SIZE, tmpx + 3 - mInfo.deltaSCnt, tmpy);
    mRender.drawLine(tmpx - 3 + mInfo.deltaSCnt, tmpy, tmpx + 3 - mInfo.deltaSCnt, tmpy);

    //board
    mRender.setDrawMode(DM_POSITIVE);
    mRender.drawFillRect(BOARD_L + 1, BOARD_T + 1, BOARD_SIZE - 1, BOARD_SIZE - 1);
    mRender.setDrawMode(DM_NEGATIVE);
    mRender.drawFillRect(BOARD_L + 2, BOARD_T + 2, BOARD_SIZE - 3, BOARD_SIZE - 3);
    mRender.setDrawMode(DM_POSITIVE);
    for (u8 i = 1; i < CELL_NUM; ++i) {
        mRender.drawFillRect(BOARD_L + i * CELL_SIZE, BOARD_T + 1, LINE_W, BOARD_SIZE - 2);
        mRender.drawFillRect(BOARD_L + 1, BOARD_T + i * CELL_SIZE, BOARD_SIZE - 2, LINE_W);
    }
    
    //stone
    for (u8 y = 0; y < CELL_NUM; ++y) {
        for (u8 x = 0; x < CELL_NUM; ++x) {
            u8 ox = BOARD_L + x * CELL_SIZE;
            u8 oy = BOARD_T + y * CELL_SIZE;

            if (mBoard[y][x].putFlg) {
                mRender.setDrawMode(DM_POSITIVE);
                mRender.drawFillRect(ox + 4, oy + 4, 1, 1);
            }
            
            if (mBoard[y][x].state == CELL_EMPTY) continue;
            if ((mBoard[y][x].blink) && (mInfo.blinkCnt > BLINK_TIME)) continue;
            
            mRender.setDrawMode(DM_POSITIVE);
            if ((mBoard[y][x].animeCnt == 0) || (mBoard[y][x].animeCnt >= 4)) {
                mRender.drawFillRect(ox + 3, oy + 2, 3, 1);
                mRender.drawFillRect(ox + 3, oy + 6, 3, 1);
                mRender.drawFillRect(ox + 2, oy + 3, 1, 3);
                mRender.drawFillRect(ox + 6, oy + 3, 1, 3);
            } else if ((mBoard[y][x].animeCnt == 1) || (mBoard[y][x].animeCnt == 3)) {
                mRender.drawFillRect(ox + 4, oy + 2, 1, 1);
                mRender.drawFillRect(ox + 4, oy + 6, 1, 1);
                mRender.drawFillRect(ox + 3, oy + 3, 1, 3);
                mRender.drawFillRect(ox + 3, oy + 3, 1, 3);
            } else {
                mRender.drawFillRect(ox + 4, oy + 2, 1, 5);
            }
            
            if (mBoard[y][x].state == CELL_BLACK) {
                if (mBoard[y][x].animeCnt < 2) mRender.drawFillRect(ox + 3, oy + 3, 3, 3);
            } else {
                if (mBoard[y][x].animeCnt > 2) mRender.drawFillRect(ox + 3, oy + 3, 3, 3);
            }
        }
    }
    
    //cursor
    if (!mInfo.endFlg && !mInfo.passFlg && (mInfo.blinkCnt <= BLINK_TIME)) {
        mRender.setDrawMode(DM_INVERSE);
        u8 ox = BOARD_L + mInfo.cx * CELL_SIZE;
        u8 oy = BOARD_T + mInfo.cy * CELL_SIZE;
        mRender.drawFillRect(ox, oy, 3, 3);
        mRender.drawFillRect(ox + 2, oy + 2, 1, 1);
        mRender.drawFillRect(ox + 6, oy, 3, 3);
        mRender.drawFillRect(ox + 6, oy + 2, 1, 1);
        mRender.drawFillRect(ox, oy + 6, 3, 3);
        mRender.drawFillRect(ox + 2, oy + 6, 1, 1);
        mRender.drawFillRect(ox + 6, oy + 6, 3, 3);
        mRender.drawFillRect(ox + 6, oy + 6, 1, 1);
    }
    
    if (!mInfo.animeFlg) {
        //pass
        if (mInfo.passFlg) {
            mRender.setDrawMode((mInfo.p == CELL_BLACK)? DM_POSITIVE: DM_NEGATIVE);
            mRender.drawFillRect(BOARD_L + (BOARD_SIZE - FONT_SIZE * 6) / 2, BOARD_T + (BOARD_SIZE - FONT_SIZE * 3) / 2, FONT_SIZE * 6, FONT_SIZE * 3);
            mRender.setDrawMode(DM_INVERSE);
            if (mInfo.blinkCnt <= BLINK_TIME) {
                mRender.drawFillRect(BOARD_L + (BOARD_SIZE - FONT_SIZE * 6) / 2 + 1, BOARD_T + (BOARD_SIZE - FONT_SIZE * 3) / 2 + 1, FONT_SIZE * 6 - 2, FONT_SIZE * 3 - 2);
                mRender.drawFillRect(BOARD_L + (BOARD_SIZE - FONT_SIZE * 6) / 2 + 2, BOARD_T + (BOARD_SIZE - FONT_SIZE * 3) / 2 + 2, FONT_SIZE * 6 - 4, FONT_SIZE * 3 - 4);
            }
            mRender.drawString(BOARD_L + (BOARD_SIZE - FONT_SIZE * 6) / 2 + FONT_SIZE, BOARD_T + (BOARD_SIZE - FONT_SIZE * 3) / 2 + FONT_SIZE, "PASS");
        }
         
        //end
        if (mInfo.endFlg) {
        }
    }

    if (mShowInfo) {
        mRender.setDrawMode(DM_INVERSE);
        mSystem.drawInfo(mRender);
    }

    mRender.present();
    super::update();
}

bool Reversi::putableD(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p, s8 dx, s8 dy)
{
    if (board[py][px].state != CELL_EMPTY) return false;
    
    int num = 0;
    for (u8 i = 1;; ++i) {
        s8 tmpx = px + dx * i;
        s8 tmpy = py + dy * i;
        if ((tmpx < 0) || (tmpx >= CELL_NUM) || (tmpy < 0) || (tmpy >= CELL_NUM) || (board[tmpy][tmpx].state == CELL_EMPTY)) {
            num = 0;
            break;
        }
        if (board[tmpy][tmpx].state == p) break;
        ++num;
    }
    
    return num != 0;
}

bool Reversi::putable(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p)
{
    const s8 dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    const s8 dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
    
    if (board[py][px].state != CELL_EMPTY) return false;

    bool flg = false;
    for (u8 i = 0; i < 8; ++i) {
        if ((flg = putableD(board, px, py, p, dx[i], dy[i])) == true) break;
    }
    
    return flg;
}

u8 Reversi::put(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p)
{
    const s8 dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    const s8 dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
    
    u8 num = 0;
    for (u8 i = 0; i < 8; ++i) {
        if (putableD(board, px, py, p, dx[i], dy[i])) {
            for (u8 j = 1;; ++j) {
                s8 tmpx = px + dx[i] * j;
                s8 tmpy = py + dy[i] * j;
                if (board[tmpy][tmpx].state == p) {
                    num += j - 1;
                    break;
                }
                board[tmpy][tmpx].state = p;
                board[tmpy][tmpx].animeCnt = 3 + j * 2;
                board[tmpy][tmpx].animeCnt1 = 20;
            }
        }
    }
    if (num != 0) {
        board[py][px].state = p;
    }
    
    return num;
}

bool Reversi::checkPass(Cell board[CELL_NUM][CELL_NUM], u8 p)
{
    bool flg = true;
    for (u8 y = 0; y < CELL_NUM; ++y) {
        for (u8 x = 0; x < CELL_NUM; ++x) {
            if ((board[y][x].putFlg = putable(board, x, y, p)) == true) {
                flg = false;
            }
        }
    }
    
    return flg;
}

void Reversi::checkBlink(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p)
{
    const s8 dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    const s8 dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
    
    for (u8 y = 0; y < CELL_NUM; ++y) {
        for (u8 x = 0; x < CELL_NUM; ++x) {
            board[y][x].blink = false;
        }
    }
    
    for (u8 i = 0; i < 8; ++i) {
        if (putableD(board, px, py, p, dx[i], dy[i])) {
            for (u8 j = 1;; ++j) {
                s8 tmpx = px + dx[i] * j;
                s8 tmpy = py + dy[i] * j;
                if (board[tmpy][tmpx].state == p) {
                    break;
                }
                board[tmpy][tmpx].blink = true;
            }
        }
    }
}

bool Reversi::aiThink()
{
    minmax(mBoard, 4, ai.p);
    
    return true;
}

s8 Reversi::minmax(Cell board[CELL_NUM][CELL_NUM], u8 depth, u8 p)
{
    if (depth-- == 0) return evaluation(board, ai.p);
    
    s8 best = (p == ai.p)? -100: 100;
    u8 bestX = 0;
    u8 bestY = 0;
    for (u8 y = 0; y < CELL_NUM; ++y) {
        for (u8 x = 0; x < CELL_NUM; ++x) {
            if (putable(board, x, y, p)) {
                Cell nBoard[CELL_NUM][CELL_NUM];
                for (u8 yy = 0; yy < CELL_NUM; ++yy) {
                    for (u8 xx = 0; xx < CELL_NUM; ++xx) {
                        nBoard[yy][xx] = board[yy][xx];
                    }
                }
                put(nBoard, x, y, p);
                
                s8 s = minmax(nBoard, depth, p % 2 + 1);
                
                if (((p == ai.p) && (best < s)) || ((p != ai.p) && (best > s))) {
                    best = s;
                    bestX = x;
                    bestY = y;
                }
            }
        }
    }
    ai.cx = bestX;
    ai.cy = bestY;
    
    return best;
}

s8 Reversi::evaluation(Cell board[CELL_NUM][CELL_NUM], u8 p)
{
    const s8 table[CELL_NUM][CELL_NUM] = {
         30, -12,   0,  -1,  -1,   0, -12,  30,
        -12, -15,  -3,  -3,  -3,  -3, -15, -12,
          0,  -3,   0,  -1,  -1,   0,  -3,   0,
         -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,
         -1,  -3,  -1,  -1,  -1,  -1,  -3,  -1,
          0,  -3,   0,  -1,  -1,   0,  -3,   0,
        -12, -15,  -3,  -3,  -3,  -3, -15, -12,
         30, -12,   0,  -1,  -1,   0, -12,  30,
    };
    
    u8 b = 0;
    u8 w = 0;
    u8 e = 0;
    for (u8 y = 0; y < CELL_SIZE; ++y) {
        for (u8 x = 0; x < CELL_SIZE; ++x) {
            if (board[y][x].state != CELL_EMPTY) ++e;
            if (board[y][x].state != CELL_BLACK) ++b;
            if (board[y][x].state != CELL_WHITE) ++w;
        }
    }
    if (e == 0) return (p == CELL_BLACK)? b: w;
    
    s8 re = 0;
    for (u8 y = 0; y < CELL_SIZE; ++y) {
        for (u8 x = 0; x < CELL_SIZE; ++x) {
            if (board[y][x].state == CELL_EMPTY) continue;
            re += table[y][x] * ((board[y][x].state == p)? 1: -1);
        }
    }
    
    return re;
}
