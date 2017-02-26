#pragma once

#include "mbed.h"

#define RENDER_ENABLE_DIRTY_CHECK   1
#define SYSTEM_ENABLE_MEASURE_FPS   1
#define SYSTEM_ENABLE_FRAME_WAIT    1
#define ENABLE_PGC2000              1

namespace pgc1000
{
    
    typedef int8_t      s8;
    typedef uint8_t     u8;
    typedef int16_t     s16;
    typedef uint16_t    u16;
    typedef int32_t     s32;
    typedef uint32_t    u32;
    typedef float       f32;
    
    enum CHIP_SELECT {
        CS_NONE     = 0,
        CS_LEFT     = 1,
        CS_RIGHT    = 2,
        CS_BOTH     = CS_LEFT | CS_RIGHT,
    };
    
    enum {
        DI_INST    = 0,
        DI_DATA    = 1,
    };
    
    class Lcd {
    public:
        enum {
            LCD_PAGE_MAX    = 8,
            LCD_LINE_MAX    = 128,
            LCD_LINE_MAX_2  = LCD_LINE_MAX >> 1,
        };
    
        enum {
            TIME_E_LOW      = 1,
            TIME_E_HIGH     = 1,
            TIME_RES_LOW    = 1,
        };
    
        struct Desc {
            PinName di;
            PinName rw;
            PinName e;
            PinName db0;
            PinName db1;
            PinName db2;
            PinName db3;
            PinName db4;
            PinName db5;
            PinName db6;
            PinName db7;
            PinName cs1;
            PinName cs2;
            PinName res;
        };
        
        static const Desc DefaultDesc;
    
        Lcd(const Desc &desc = DefaultDesc);
        
        //! @name Low-level functions
        //@{
            
        void write(u8 cs, u8 di, u8 data);
        
        void displayOn(u8 cs){ write(cs, DI_INST, 0x3F); }
        void displayOff(u8 cs){ write(cs, DI_INST, 0x3E); }
        
        void setAddress(u8 cs, u8 line){ write(cs, DI_INST, 0x40 | (line & 0x3F)); }

        void setPage(u8 cs, u8 page){ write(cs, DI_INST, 0xB8 | (page & 0x07)); }        

        void displayStartLine(u8 cs, u8 line){ write(cs, DI_INST, 0xC0 | (line & 0x3F)); }
        
        void writeDisplayData(u8 cs, u8 data){ write(cs, DI_DATA, data); }
        
        //@}
    
        //! @name High-level functions
        //@{
            
        void init();
            
        void blit(u8* pdata);
        
        void blit(u8* pdata, u8* pdirty);
                    
        //@}
        
    protected:
        DigitalOut  mDI;
        DigitalOut  mRW;
        DigitalOut  mE;
        BusOut      mDB;
        BusOut      mCS;
        DigitalOut  mRES;
    };
    
    
    enum {
        FONT_SIZE = 6,
    };
    
    extern const u8 font[96][5];
    
    enum PAT {
        PAT_0 = 1,
        PAT_1,
        PAT_2,
        PAT_3,
        PAT_4,
        PAT_5,
        PAT_6,
    };
    
    enum DRAW_MODE {
        DM_POSITIVE         = 0x00000000,
        DM_NEGATIVE         = 0x00000100,
        DM_ADD              = 0x00010000,
        DM_INVERSE          = 0x00020000,
        DM_SUBTRACT         = 0x00040000,
        DM_PAT              = 0x00000200,
        DM_NPAT             = 0x00000400,
        
        //! @name alias
        //@{
        DM_DEFAULT          = DM_POSITIVE,
        DM_PAT_ADD          = DM_PAT | DM_ADD,
        DM_PAT_INV          = DM_PAT | DM_INVERSE,
        DM_NEGATIVE_ADD     = DM_NEGATIVE | DM_ADD,
        DM_NEGATIVE_INV     = DM_NEGATIVE | DM_INVERSE,
        //@}
        
        DM_PAT_MASK         = 0x000000FF,   //!< animation pattern mask
        DM_SRC_MOD_MASK     = 0x0000FF00,   //!< src modifier mask
        DM_OP_MASK          = 0x00FF0000,   //!< operator mask
        
        //! @name deprecated
        //@{
        DM_POSITIVE_ADD     = DM_ADD,
        DM_PAT_0            = DM_PAT | PAT_0,
        DM_PAT_1            = DM_PAT | PAT_1,
        DM_PAT_2            = DM_PAT | PAT_2,
        DM_PAT_3            = DM_PAT | PAT_3,
        DM_PAT_4            = DM_PAT | PAT_4,
        DM_PAT_5            = DM_PAT | PAT_5,
        DM_PAT_6            = DM_PAT | PAT_6,
        //@}
    };
    
    class Render {
    public:
        enum {
            SCREEN_WIDTH    = 128,
            SCREEN_HEIGHT   = 64,
            
            VRAM_SIZE       = Lcd::LCD_PAGE_MAX * Lcd::LCD_LINE_MAX,
            BUFFER_MAX      = 2,
        };
    
        Render();
        
        void init();
        
        void update();
        
        void present();
        
        void clear();
                
        void drawPixel(s16 x, s16 y);
        
        //! deprecated
        void drawFillRect(s16 x, s16 y, u8 w, u8 h){ fillRect(x, y, w, h); }

        void fillRect(s16 x, s16 y, u8 w, u8 h);
        
        void drawLine(s16 x1, s16 y1, s16 x2, s16 y2, bool conservative = true);

        void drawPolygon(u8 n, const s16* x, const s16* y);
        
        //! deprecated
        void drawImage8(s16 x, s16 y, const u8* img, u8 w, u8 h = 8){ drawImage(x, y, img, w, h); }
        
        //! @brief draw image of which max height is 8.
        void drawImage(s16 x, s16 y, const u8* img, u8 w, u8 h = 8);
        
        void drawImageShift(s8 sx, s8 sy, s16 x, s16 y, const u8* img, u8 w, u8 h = 8);

        //! @brief draw image of which max height is 8.
        void drawGrayImage(s16 x, s16 y, const u32* img, u8 w, u8 h = 8);

        void drawString(s16 x, s16 y, const char* fmt, ...);
                
        void setDrawMode(u32 mode){ mDrawMode = mode; }
        u32 getDrawMode() const { return mDrawMode; }
        
        void setAnimPat(u8 pat){ mAnimPat = pat; }
        u8 getAnimPat() const { return mAnimPat; }
        
        //! @param[in] speed between 0(faster) to 32(slower)
        void setAnimSpeed(u8 speed){ mAnimSpeed = speed; }
        u8 getAnimSpeed() const { return mAnimSpeed; }
        
        bool readPixel(u8 x, u8 y);
        
        u8* map(){ return mVRAM[mBufferIndex]; }
        void unmap();
        
    protected:
        void combine(u8 x, u8 y, u8 src, u8 mask = 0xFF);
        u8 genAnimPat(u8 x, u8 y, u8 src, u8 mask);

    protected:
        Lcd mLcd;
        u8 mVRAM[BUFFER_MAX][VRAM_SIZE];
        u8 mDirty[Lcd::LCD_LINE_MAX];
        u8 mBufferIndex;
        u32 mDrawMode;
        u32 mFrameCount;
        u8 mAnimSpeed;
        u8 mAnimPat;
    };
    
    
    class System {
    public:
        /*
        enum {
            FPS = 30,
        };
        */
        
        struct Desc {
            PinName led1;
            PinName btn_black;
            PinName btn_red;
            PinName joy_x;
            PinName joy_y;
            PinName serial_tx;
            PinName serial_rx;
#if ENABLE_PGC2000
            PinName led2;
            PinName btn_green;
            PinName btn_blue;
            PinName btn_white;
            PinName btn_orange;
            PinName dip1;
            PinName dip2;
            PinName dip3;
            PinName dip4;
#endif
        };
        
        static const Desc DefaultDesc;
    
        System(const Desc &desc = DefaultDesc);
        
        void init();
        
        void update();
        
        void frameWait();
        
        void drawInfo(Render &render);
        
        u32 getFPS() const { return FPS; }
        void setFPS(u32 fps)
        {
            FPS = fps;
            MICROSEC_PER_FRAME = 1000000 / FPS;
            FRAME_PER_MICROSEC = (f32)FPS * 0.000001f;
        }
        
        bool isFrameWait() const { return mFrameWait; }
        void setFrameWait(bool b){ mFrameWait = b; }
        
        u32 getFrameCount() const { return mFrameCount; }
        
        f32 getActualFPS() const { return mFPS; }
        f32 getFrameTime() const { return mFrameTime; }

        f32 getDeltaTime() const { return mDeltaTime; }
        
        struct BtnInfo {
            u8 trg      : 1;
            u8 on       : 1;
            u8 rep      : 1;
            u8 rel      : 1;
            u8 old      : 1;
            u8 rep1     : 1;
            u8 reserved : 2;
            u8 count;
            u8 rep1_count;
            u8 rep_count;
            
            void update(bool now);
        };

        struct BtnInfo &getBtnBlack(){ return mBtnInfoBlack; }
        struct BtnInfo &getBtnRed(){ return mBtnInfoRed; }
        struct BtnInfo &getBtnLeft(){ return mBtnInfoLeft; }
        struct BtnInfo &getBtnRight(){ return mBtnInfoRight; }
        struct BtnInfo &getBtnUp(){ return mBtnInfoUp; }
        struct BtnInfo &getBtnDown(){ return mBtnInfoDown; }
        f32 getAnalogX() const { return mAnalogX; }
        f32 getAnalogY() const { return mAnalogY; }
        
        Serial &getSerial(){ return mSerial; }
        void printf(char* fmt, ...);
        void scanf(char* fmt, ...);

        void setLed1(bool b){ mLed1 = b; }

#if ENABLE_PGC2000
        struct BtnInfo &getBtnGreen(){ return mBtnInfoGreen; }
        struct BtnInfo &getBtnBlue(){ return mBtnInfoBlue; }
        struct BtnInfo &getBtnWhite(){ return mBtnInfoWhite; }
        struct BtnInfo &getBtnOrange(){ return mBtnInfoOrange; }

        u8 dip(){ return (~mDip.read()) & mDip.mask(); }
        
        void setLed2(bool b){ mLed2 = b; }
#endif

    protected:
        u32 FPS;
        u32 MICROSEC_PER_FRAME;
        f32 FRAME_PER_MICROSEC;
        
        bool mFrameWait;
        u32 mFrameCount;
        u32 mPrevTime;
        f32 mFrameTime;
        f32 mDeltaTime;
        f32 mFPS;
        u32 mFrameCountTemp;
        u32 mPrevTimeTemp;

        BtnInfo mBtnInfoBlack;
        BtnInfo mBtnInfoRed;
#if ENABLE_PGC2000
        BtnInfo mBtnInfoGreen;
        BtnInfo mBtnInfoBlue;
        BtnInfo mBtnInfoWhite;
        BtnInfo mBtnInfoOrange;
#endif
        BtnInfo mBtnInfoLeft;
        BtnInfo mBtnInfoRight;
        BtnInfo mBtnInfoUp;
        BtnInfo mBtnInfoDown;
        f32 mAnalogX;
        f32 mAnalogY;

        DigitalOut mLed1;
        DigitalIn mBtnBlack;
        DigitalIn mBtnRed;
        AnalogIn mJoyX;
        AnalogIn mJoyY;
        Serial mSerial;
#if ENABLE_PGC2000  
        DigitalOut mLed2;
        DigitalIn mBtnGreen;
        DigitalIn mBtnBlue;
        DigitalIn mBtnWhite;
        DigitalIn mBtnOrange;
        BusIn mDip;
#endif
    };
    

#if ENABLE_PGC2000
    enum CHANNEL {
        CHANNEL_A,
        CHANNEL_B,
        CHANNEL_C,
        CHANNEL_MAX,
    };
    
    enum MIX {
        MIX_TONE_A      = 0x01,
        MIX_TONE_B      = 0x02,
        MIX_TONE_C      = 0x04,
        MIX_TONE_ALL    = 0x07,
        MIX_NOISE_A     = 0x08,
        MIX_NOISE_B     = 0x10,
        MIX_NOISE_C     = 0x20,
        MIX_NOISE_ALL   = 0x38,
        MIX_ALL         = 0x3F,
    };
    
    enum ENVELOPE_SHAPE {
        ES_SAW_INV          = 0x8,  // ＼｜＼｜＼｜＼｜＼
        ES_SLOPE_DOWNWARD   = 0x9,  // ＼＿＿＿＿＿＿＿＿
        ES_TRIANGLE_INV     = 0xA,  // ＼／＼／＼／＼／＼
        ES_WEDGE_INV        = 0xB,  // ＼｜￣￣￣￣￣￣￣
        ES_SAW              = 0xC,  // ／｜／｜／｜／｜／
        ES_SLOPE_UPWARD     = 0xD,  // ／￣￣￣￣￣￣￣￣
        ES_TRIANGLE         = 0xE,  // ／＼／＼／＼／＼／
        ES_WEDGE            = 0xF,  // ／｜＿＿＿＿＿＿＿
    };

    class Psg {
    public:
        struct Desc {
            PinName wr;
            PinName cs;
            PinName a0;
            PinName ic;
            PinName d0;
            PinName d1;
            PinName d2;
            PinName d3;
            PinName d4;
            PinName d5;
            PinName d6;
            PinName d7;
        };
        
        static const Desc DefaultDesc;
        
        Psg(const Desc &desc = DefaultDesc);
        
        void init();
        
        //! @name Low-level functions
        //@{
        
        void writeAddress(u8 adr);
        
        void writeData(u8 data);
        
        void write(u8 adr, u8 data);
        
        //@}
        
        //! @name High-level functions
        //@{
        
        void setToneFrequency(u8 channel, u16 freq){ write(0x00 + (channel << 1), freq & 0xFF); write(0x01 + (channel << 1), (freq >> 8) & 0xFF); }
        
        void setNoiseFrequency(u8 freq){ write(0x06, freq); }
        
        //! @param[in] tone 00CBAcba. 'CBA' is noise. 'cba' is tone. set '0' to enable the channel.
        void setMixer(u8 mix){ write(0x07, ~mix); }
        
        void setVolume(u8 channel, bool mode, u8 level){ write(0x08 + channel, (mode ? 0x10 : 0x00) | level); }
        
        void setEnvelopeFrequency(u16 freq){ write(0x0B, freq & 0xFF); write(0x0C, (freq >> 8) & 0xFF); }
        
        void setEnvelopeControl(u8 es){ write(0x0D, es); }
    
        //@}
    
    protected:
        u8 mAddress;
        
        DigitalOut mWR;
        DigitalOut mCS;
        DigitalOut mA0;
        DigitalOut mIC;
        BusOut mD;
    };
    
    class Sound {
    public:
        Sound();
        
        void init();
        
        Psg &getPsg(){ return mPsg; }
        
    protected:
        Psg mPsg;
    };
#endif

    
    class App {
    public:
        App();
        
        virtual ~App();
        
        virtual void init();
        
        virtual void exec();
        
        virtual void update();
        
    protected:
        System mSystem;
        Render mRender;
#if ENABLE_PGC2000
        Sound mSound;
#endif
    };

}
