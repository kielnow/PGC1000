#pragma once

#include "pgc1000.h"

namespace app {

    using namespace pgc1000;
    
    // type
    
    enum TASK_TYPE {
        TT_SHOT,
        TT_ENEMY,
        TT_PARTICLE,
        TT_VFX,
    };
    
    // subtype
    
    enum SHOT_TYPE {
        SHOT_01,
    };
    
    enum ENEMY_TYPE {
        EM_01,
        EM_02,
        EM_03,
        EM_04,
        EM_05,
        EM_SHOT_01,
    };
    
    enum VFX_TYPE {
        VFX_01,
        VFX_02,
    };
    
    struct Task {
        u8 type;
        u8 enable   : 1;
        u8 init     : 1;
        u8 reserved : 6;
    };
    
    struct Shot : public Task {
        u8 subtype;
        s8 x;
        s8 y;
        s8 vx;
        s8 vy;
        u8 w;
        u8 h;
    };
            
    struct Enemy : public Task {
        u8 subtype;
        s8 x;
        s8 y;
        s8 vx;
        s8 vy;
        u8 w;
        u8 h;
        u8 cfg;
        union {
            struct {
                u8 rno0 : 4;
                u8 rno1 : 4;
                u8 rno2 : 4;
                u8 rno3 : 4;
            };
            u16 rno;
        };
        union {
            u8  _u8[32];
            u16 _u16[16];
            u32 _u32[8];
            s8  _s8[32];
            s16 _s16[16];
            s32 _s32[8];
            f32 _f32[8];
        } wk;
        union {
            struct {
                u8 ano0 : 4;
                u8 ano1 : 4;
            };
            u8 ano;
        };
        u8 pat;
        u8 anim;
    };
    
    struct Particle : public Task {
        u8 w;
        u8 h;
        f32 x;
        f32 y;
        f32 vx;
        f32 vy;
        f32 x_old;
        f32 y_old;        
        u8 cnt;
    };
    
    struct VFX : public Task {
        u8 subtype;
        s8 x;
        s8 y;
        u8 cnt;
        s8 delay;
        union {
            struct {
                u8 ano0;
                u8 ano1;
            };
            u16 ano;
        };
        u8 pat;
        u8 anim;
    };
    
    class Gradius : public App {
    public:
        typedef App super;
        
        Gradius();
        
        virtual void init();
        
        virtual void update();
        
    protected:
        void genBGImage();
        void drawBackground();

        void genShot();
        void updateShot();

        void evalCommand();
        Enemy* genEnemy(u8 subtype, s8 x, s8 y, u8 cfg, u8 pat);
        void updateEnemy();
        
        void emitParticle(f32 x, f32 y);
        void updateParticle();

        void genVFX(u8 subtype, s8 x, s8 y, u8 delay = 0);
        void updateVFX();

        static bool isCollided(s8 x1, s8 y1, u8 w1, u8 h1, s8 x2, s8 y2, u8 w2, u8 h2)
        {
            return (x2 < x1 + w1 && x1 < x2 + w2 && y2 < y1 + h1 && y1 < y2 + h2);
        }
        
        bool isCollidedWithBG(s8 x, s8 y, u8 w, u8 h) const;
                
    protected:
        enum {
            SHOT_MAX        = 16,
            ENEMY_MAX       = 64,
            PARTICLE_MAX    = 64,
            VFX_MAX         = 32,
        };
        
        struct BGImage {
            u8 h;
            s8 v;
            u8 pat[128];
        };
        
    protected:
        bool mPause;
        u16 mFrameCount;
        u16 mCommandPt;
        
        struct {
            u16 pc; // program counter
            u16 cnt;
        } mLevel;
        
        struct {
            union {
                struct {
                    u8 hit      : 1;
                    u8 dead     : 1;
                    u8 reserved : 6;
                };
                u8 flag;
            };
            s8 x;
            s8 y;
            s8 x_old;
            s8 y_old;
            u8 speed;
            u8 scnt;    // shot count
        } mPlayer;

        struct {
            BGImage top;
            BGImage bot;
            u16 x;
            u8 anim;
        } mBG;
        
        Shot mShot[SHOT_MAX];
        Enemy mEnemy[ENEMY_MAX];
        Particle mParticle[PARTICLE_MAX];
        VFX mVFX[VFX_MAX];
    };

}
