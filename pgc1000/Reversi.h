#pragma once

#include "pgc1000.h"

namespace app {

    using namespace pgc1000;

    class Reversi : public App {
    public:
        typedef App super;
                
        virtual void init();
        
        virtual void update();
            
    public:
        enum {
            CELL_NUM    = 8,
        };
        
    protected:
        enum CELL_STATE {
            CELL_EMPTY,
            CELL_BLACK,
            CELL_WHITE,
        };

        bool mPause;
        bool mShowInfo;
        
        struct {
            u8 cx;
            u8 cy;
            u8 p;
            u8 blackNum;
            u8 whiteNum;
            bool animeFlg;
            u8 blinkCnt;
            u8 deltaSCnt;
            u8 deltaSCnt1;
            u8 deltaYCnt;
            u8 deltaYCnt1;
            u8 endAnimeCnt;
            u8 endAnimeCnt1;
            bool endAnimeFlg;
            bool passFlg;
            bool endFlg;
        } mInfo;

        struct Cell {
            u8 state;
            bool putFlg;
            u8 animeCnt;
            u8 animeCnt1;
            bool blink;
        };
        
        struct {
            u8 cx;
            u8 cy;
            u8 p;
            bool think;
            u8 moveCnt;
            bool put;
        } ai;
        
        Cell    mBoard[CELL_NUM][CELL_NUM];

    protected:
        bool putableD(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p, s8 dx, s8 dy);
        bool putable(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p);
        u8 put(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p);
        bool checkPass(Cell board[CELL_NUM][CELL_NUM], u8 p);
        void checkBlink(Cell board[CELL_NUM][CELL_NUM], u8 px, u8 py, u8 p);
        bool aiThink();
        s8 minmax(Cell board[CELL_NUM][CELL_NUM], u8 depth, u8 p);
        s8 evaluation(Cell board[CELL_NUM][CELL_NUM], u8 p);

    };

}