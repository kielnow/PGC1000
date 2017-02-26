#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;

    class Bricks : public App {
    public:
        typedef App super;
        
        Bricks();
        
        virtual ~Bricks();
        
        virtual void init();
        
        virtual void update();
        
    protected:

        static bool isCollided(s8 x1, s8 y1, u8 w1, u8 h1, s8 x2, s8 y2, u8 w2, u8 h2)
        {
            return (x2 < x1 + w1 && x1 < x2 + w2 && y2 < y1 + h1 && y1 < y2 + h2);
        }
    
    protected:
        enum {
            BALL_MAX = 8,
            BRICK_MAX = 144,
        };
        
    protected:
        bool mPause;
        bool mShowInfo;
        
        struct {
            u8 dead : 1;
            u8 next : 1;
            u8 reserved : 6;
            u8 life;
            u8 ball;
            u32 score;
            u32 score_temp;            
            u32 cnt;
        } mInfo;
    
        struct {
            f32 x;
            f32 y;
            u8 w;
            u8 h;
            f32 speed;
            f32 x_old;
            f32 y_old;
        } mBar;
        
        struct Ball {
            u8 enable : 1;
            u8 hold : 1;
            u8 reserved : 6;
            f32 x;
            f32 y;
            u8 w;
            u8 h;
            f32 speed;
            f32 vx;
            f32 vy;
            f32 x_old;
            f32 y_old;
        } mBall[BALL_MAX];
        
        struct Brick {
            u8 enable : 1;
            u8 reserved : 7;
            u8 x;
            u8 y;
            u8 w;
            u8 h;
            u8 pat;
        } mBrick[BRICK_MAX];
    };

}
