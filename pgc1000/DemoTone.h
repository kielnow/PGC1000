#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;
    
    class DemoTone : public App
    {
    public:
        typedef App super;
    
        virtual void init();
        
        virtual void update();
        
    protected:
        s8 mCount;
        s8 mDeltaCount;
        f32 mTheta;
        bool mPause;
        s8 mToneNum;
    };
    
}