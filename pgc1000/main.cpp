#include "mbed.h"
#include "Benchmark.h"
#include "Tetris.h"
#include "DemoTone.h"
#include "Gradius.h"
#include "DemoImage.h"
#include "DemoSound.h"
#include "Bricks.h"
#include "BadApple.h"

int main()
{
    //app::Benchmark app;
    //app::Tetris app;
    //app::DemoTone app;
    //app::Gradius app;
    //app::DemoImage app;
    //app::DemoSound app;
    //app::Bricks app;
    app::BadApple app;
    app.exec();
    return 0;
}
