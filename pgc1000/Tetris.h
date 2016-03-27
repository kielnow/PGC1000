#pragma once

#include "pgc1000.h"

namespace app {

    using namespace pgc1000;

    class Tetris : public App {
    public:
        typedef App super;
    
        Tetris();
        
        virtual void init();
        
        virtual void update();
    
    protected:
        bool isCollided(u8 type, u8 rot, s8 x, s8 y);
        void fixMino();
        u8 clearLine();
        void dropLine();
        void emitParticle(f32 x, f32 y);
        void updateAndDrawParticle();
        void drawBoard();
        void drawMino();
    
    protected:
        enum {
            BOARD_TOP           = 2,
            BOARD_WIDTH         = 10,
            BOARD_HEIGHT        = 20 + BOARD_TOP,
            
            PARTICLE_MAX        = 128,
        };
        
        enum TYPE {
            TYPE_I, TYPE_J, TYPE_L, TYPE_S, TYPE_Z, TYPE_T, TYPE_O, TYPE_MAX
        };
        
        enum ROT {
            ROT_0, ROT_90, ROT_180, ROT_270, ROT_MAX
        };
        
        static const u32 MINO[TYPE_MAX][ROT_MAX];
        
        struct Particle {
            bool enable;
            u8 w;
            u8 h;
            f32 x;
            f32 y;
            f32 vx;
            f32 vy;
        };
        
    protected:
        uint16_t mBoard[BOARD_HEIGHT];
        struct {
            u8 type;
            u8 rot;
            s8 x;
            s8 y;
        } mMino;
        struct {
            u8 fall_count;
            s8 fall_speed;
        } mInfo;
        Particle mParticle[PARTICLE_MAX];
    };

}