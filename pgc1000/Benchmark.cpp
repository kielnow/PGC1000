#include "Benchmark.h"

using namespace app;

void Benchmark::exec()
{
    App::init();
    
    mRender.drawString(0, 6, 
        " !\"#$%%&'()*+,-./\n"
        "0123456789:;<=>?\n"
        "@ABCDEFGHIJKLMNO\n"
        "PQRSTUVWXYZ[\\]^_\n"
        "`abcdefghijklmno\n"
        "pqrstuvwxyz{|}~ ");
  
    mRender.drawFillRect(0, 42, 96, 10);
    
    u8* vram = mRender.map();
    for (u8 i = 0; i < 128; ++i) {
        vram[(i << 3) + 7] = i;
    } 
    mRender.unmap();
    
    const u8 img[] = { 0x70, 0x18, 0x7D, 0xB6, 0xBC, 0x3C, 0xBC, 0xB6, 0x7D, 0x18, 0x70 };
    mRender.drawImage8(100, 10, img, sizeof(img));
    
    s8 x = 0, y = 0, vx = 1, vy = 1;
    const u8 w = 30, h = 30;
    //s8 x = 0, y = 0, vx = 4, vy = 4;
    //const u8 w = (sizeof(img) + 1) * 4, h = 8;

    mRender.setDrawMode(DM_INVERSE);
    mRender.drawFillRect(x, y, w, h);
    //for (int i = 0; i < 4; ++i) mRender.drawImage8(x + i * (sizeof(img) + 1), y, img, sizeof(img));

    while (true) {
        mRender.drawFillRect(x, y, w, h);
        //for (int i = 0; i < 4; ++i) mRender.drawImage8(x + i * (sizeof(img) + 1), y, img, sizeof(img));
        x += vx;
        y += vy;
        if (x < 0 || 128 - w <= x)
            vx *= -1;
        if (y < 0 || 64 - h <= y)
            vy *= -1;
    
        mRender.setDrawMode(DM_POSITIVE);
        mRender.drawString(0, 0, "FPS:%.2f", mSystem.getFPS());
        //mRender.drawString(0, 6, "%u ", mSystem.dip());

        mRender.setDrawMode(DM_INVERSE);    
        mRender.drawFillRect(x, y, w, h);
        //for (int i = 0; i < 4; ++i) mRender.drawImage8(x + i * (sizeof(img) + 1), y, img, sizeof(img));
        
        mRender.present();
        mSystem.update();
        //mSystem.frameWait();
    }
}
