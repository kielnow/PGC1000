#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;
    
    class DemoSound : public App {
    public:
        typedef App super;
    
        virtual void init();
        
        virtual void update();
        
    protected:
        enum {
            LOG_MAX = 128,
        };
        
    protected:
        s8 mVolume;
        s16 mFreq;
        u8 mEnv;
        s32 mEnvFreq;
        bool mPause;
        u8 mLog[LOG_MAX];
        u8 mHead;
    };
    
}
