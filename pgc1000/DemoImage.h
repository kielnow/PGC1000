#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;
    
    class DemoImage : public App {
    public:
        typedef App super;
        
        virtual void init();
        
        virtual void update();
    
    protected:
        s8 mToneNum;
        bool mShowInfo;
    };
    
}