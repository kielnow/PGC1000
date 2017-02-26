#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;
    
    class DemoLine : public App
    {
    public:
        typedef App super;
    
        virtual void init();
        
        virtual void update();
        
    protected:
        enum {
            POINT_NUM = 16,
        };
    
        bool mPause;
        bool mShowInfo;
        struct {
            f32 x;
            f32 y;
        } mPos;
        struct Point {
            f32 x;
            f32 y;
            f32 vx;
            f32 vy;
        } mPoint[POINT_NUM];
    };
    
}