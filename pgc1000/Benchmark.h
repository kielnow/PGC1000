#pragma once

#include "pgc1000.h"

namespace app {
    
    using namespace pgc1000;
    
    class Benchmark : public App
    {
    public:
        virtual void exec();
    };
    
}