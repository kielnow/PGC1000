#include "Gradius.h"

#define DEV     1

#define M_PI    3.14159265358979323846f

using namespace app;

namespace {

    enum {
        
        VIEW_W      = 72,
        VIEW_H      = 64,
        VIEW_L      = (Render::SCREEN_WIDTH - VIEW_W) / 2,
        VIEW_T      = 0,
        VIEW_R      = VIEW_L + VIEW_W,
        VIEW_B      = VIEW_T + VIEW_H,
        
        // tile
        VIEW_TW     = VIEW_W >> 3,
        VIEW_TH     = VIEW_H >> 3,
        
        BORDER_L    = 0,
        BORDER_T    = 3,
        BORDER_R    = VIEW_R,
        BORDER_B    = VIEW_H - 3,
        
        GRID_W      = VIEW_W / 8,
        GRID_H      = 4,
        
        POS_X_0     = 0,
        POS_X_1     = GRID_W,
        POS_X_2     = GRID_W * 2,
        POS_X_3     = GRID_W * 3,
        POS_X_4     = GRID_W * 4,
        POS_X_5     = GRID_W * 5,
        POS_X_6     = GRID_W * 6,
        POS_X_7     = GRID_W * 7,
        POS_X_8     = GRID_W * 8,
        
        POS_Y_0     = 0,
        POS_Y_1     = 4,
        POS_Y_2     = 8,
        POS_Y_3     = 12,
        POS_Y_4     = 16,
        POS_Y_5     = 20,
        POS_Y_6     = 24,
        POS_Y_7     = 28,
        POS_Y_8     = 32,
        POS_Y_9     = 36,
        POS_Y_10    = 40,
        POS_Y_11    = 44,
        POS_Y_12    = 48,
        POS_Y_13    = 52,
        POS_Y_14    = 56,
        POS_Y_15    = 60,
        POS_Y_16    = 64,

        PL_W        = 10,
        PL_H        = 8,
        
        // item collision
        PL_IC_W     = 6,
        PL_IC_H     = 6,
        PL_IC_L     = 1,
        PL_IC_T     = 1,
        PL_IC_R     = PL_IC_L + PL_IC_W,
        PL_IC_B     = PL_IC_T + PL_IC_H,            

        // damage collision
        PL_DC_W     = 5,
        PL_DC_H     = 4,
        PL_DC_L     = 1,
        PL_DC_T     = 2,
        PL_DC_R     = PL_DC_L + PL_DC_W,
        PL_DC_B     = PL_DC_T + PL_DC_H,

    };

    enum PAT {
        PAT_PL          = 0,
        
        PAT_EM_01       = 1,
        PAT_EM_02       = 4,
        PAT_EM_03       = 7,
        PAT_EM_04       = 10,
        PAT_EM_05       = 18,
        
        PAT_EM_SHOT_01  = 24,
        
        PAT_VFX_BEGIN   = PAT_EM_SHOT_01 + 1,
        PAT_VFX_01      = PAT_VFX_BEGIN,
        PAT_VFX_02      = PAT_VFX_BEGIN + 8, 
    };
    
    enum {

#if 0
        DM_PL       = DM_INVERSE,
        DM_ENEMY    = DM_INVERSE,
        DM_MOUNTAIN = DM_ADD,
        DM_TREE     = DM_PAT_ADD | PAT_3,
        DM_VFX      = DM_ADD,
#else   
        DM_PL       = DM_ADD,
        DM_ENEMY    = DM_ADD,
        DM_MOUNTAIN = DM_PAT_ADD | PAT_3,
        DM_TREE     = DM_PAT_ADD | PAT_0,
        DM_VFX_01   = DM_INVERSE,
        DM_VFX_02   = DM_INVERSE,
#endif

    };
    
    const struct Image {
        u8 w;
        u8 h;
        u32 dm;
        u8 pat[16];
    } CHR[] = {
        // 0
        // PAT_PL
        { 10, 8, DM_PL, 0x81, 0xDB, 0xFF, 0xFD, 0xB4, 0x34, 0x18, 0x18, 0x18, 0x08 },
        
        // 1
        // PAT_EM_01
        { 8, 8, DM_ENEMY, 0x18, 0xBD, 0x66, 0xFF, 0xFF, 0x7E, 0xBD, 0x24 },
        { 8, 8, DM_ENEMY, 0x18, 0x7E, 0x34, 0x3C, 0x3C, 0x3C, 0x7E, 0x24 },
        { 8, 8, DM_ENEMY, 0x18, 0x3C, 0x34, 0x3C, 0x3C, 0x3C, 0x3C, 0x18 },
        
        // 4
        // PAT_EM_02
        { 8, 8, DM_ENEMY, 0xBD, 0x7E, 0xDB, 0xE7, 0xE7, 0xDB, 0x7E, 0xBD },
        { 8, 8, DM_ENEMY, 0x5A, 0x3C, 0x7E, 0x66, 0x66, 0x7E, 0x3C, 0x5A },
        { 8, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x34, 0x34, 0x3C, 0x3C, 0x18 },
        
        // 7
        // PAT_EM_03
        { 9, 8, DM_ENEMY, 0x24, 0x24, 0x66, 0x7E, 0xFF, 0xE7, 0xFF, 0xFF, 0x66 },
        { 9, 8, DM_ENEMY, 0x12, 0x36, 0x36, 0x3F, 0x7F, 0x73, 0x7F, 0x7F, 0x36 },
        { 9, 8, DM_ENEMY, 0x48, 0x6C, 0x6C, 0xFC, 0xFE, 0xCE, 0xFE, 0xFE, 0x6C },
        
        // 10
        // PAT_EM_04
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0xC7, 0xC7, 0x7E, 0x5A },
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0x73, 0x73, 0x7F, 0x4D },
        { 9, 8, DM_ENEMY, 0x18, 0x1C, 0x7E, 0x7E, 0x7F, 0x7B, 0x7B, 0xFE, 0x4D },
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0x7D, 0x7D, 0x7F, 0x4D },
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0xFF, 0xFF, 0x7E, 0x5A },//backface
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0xFE, 0xFE, 0xFE, 0xB2 },
        { 9, 8, DM_ENEMY, 0x18, 0x38, 0x7C, 0x7C, 0xFC, 0xBC, 0xBC, 0x7E, 0xB2 },
        { 9, 8, DM_ENEMY, 0x18, 0x3C, 0x3C, 0x7E, 0x7E, 0xDE, 0xDE, 0xFE, 0xB2 },
        
        // 18
        // PAT_EM_05
        { 9, 8, DM_ADD, 0x02, 0x03, 0x1F, 0x3D, 0xFD, 0x3D, 0x1F, 0x03, 0x02 },//top up
        { 9, 8, DM_ADD, 0x02, 0x03, 0x9F, 0x7D, 0x3D, 0x3D, 0x1F, 0x03, 0x02 },//top left
        { 9, 8, DM_ADD, 0x02, 0x03, 0x1F, 0x3D, 0x3D, 0x7D, 0x9F, 0x03, 0x02 },//top right
        { 9, 8, DM_ADD, 0x40, 0xC0, 0xF8, 0xBC, 0xBF, 0xBC, 0xF8, 0xC0, 0x40 },//bot up
        { 9, 8, DM_ADD, 0x40, 0xC0, 0xF9, 0xBE, 0xBC, 0xBC, 0xF8, 0xC0, 0x40 },//bot left
        { 9, 8, DM_ADD, 0x40, 0xC0, 0xF8, 0xBC, 0xBC, 0xBE, 0xF9, 0xC0, 0x40 },//bot right
        
        // 24
        // PAT_EM_SHOT_01
        { 4, 4, DM_ADD, 0x06, 0x0F, 0x0D, 0x06 },
        
        // 25
        
        // PAT_VFX_01
        { 16, 8, DM_VFX_01, 0x00, 0x00, 0x01, 0x02, 0x02, 0x84, 0x7C, 0x34, 0x24, 0x2E, 0x3E, 0x21, 0x40, 0x40, 0x80, 0x00 },//0
        { 16, 8, DM_VFX_01, 0x01, 0x01, 0x02, 0x02, 0x06, 0x0C, 0xFC, 0x74, 0x26, 0x2F, 0x38, 0x30, 0x20, 0x40, 0x40, 0x80 },//1
        { 16, 8, DM_VFX_01, 0x00, 0x00, 0x10, 0x10, 0x18, 0x18, 0x3C, 0x27, 0xE4, 0x3C, 0x18, 0x18, 0x08, 0x08, 0x00, 0x00 },//2
        { 16, 8, DM_VFX_01, 0x80, 0x40, 0x40, 0x20, 0x30, 0x38, 0x2F, 0x26, 0x74, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x01, 0x01 },//3
        { 16, 8, DM_VFX_01, 0x00, 0x80, 0x40, 0x40, 0x21, 0x3E, 0x2E, 0x24, 0x74, 0xFC, 0x84, 0x02, 0x02, 0x01, 0x00, 0x00 },//4
        { 16, 8, DM_VFX_01, 0x01, 0x01, 0x02, 0x02, 0x06, 0x0C, 0xFC, 0x74, 0x26, 0x2F, 0x38, 0x30, 0x20, 0x40, 0x40, 0x80 },//5=1
        { 16, 8, DM_VFX_01, 0x08, 0x08, 0x08, 0x08, 0x18, 0x18, 0x3C, 0xE6, 0x67, 0x3C, 0x18, 0x18, 0x10, 0x10, 0x10, 0x10 },//6
        { 16, 8, DM_VFX_01, 0x80, 0x40, 0x40, 0x20, 0x30, 0x38, 0x2F, 0x26, 0x74, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x01, 0x01 },//7=3
        
        // PAT_VFX_02
        { 8, 8, DM_VFX_02, 0x18, 0xDB, 0xF3, 0x38, 0x3E, 0x6E, 0x7C, 0x30 },//x=0
        { 8, 8, DM_VFX_02, 0x18, 0xD3, 0xB5, 0x66, 0xB3, 0xC1, 0x34, 0xEC },//x=2
        { 8, 8, DM_VFX_02, 0x08, 0x01, 0x40, 0x04, 0x81, 0x12, 0x81, 0x4C },//x=6
        { 8, 8, DM_VFX_02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },//x=12
    };
    
    const Image BG[] = {

        // 0x00
        // \.／
        { 8, 8, DM_MOUNTAIN, 0x04, 0x1C, 0x7C, 0xFC, 0xFC, 0x7C, 0x7C, 0x3C },
        { 8, 8, DM_MOUNTAIN, 0x3C, 0x1C, 0x1C, 0x0C, 0x0C, 0x04, 0x00, 0x00 },
        
        // 0x02
        // ＼    ／
        //   ＼_/
        { 8, 8, DM_MOUNTAIN, 0x04, 0x0C, 0x0C, 0x1C, 0x1C, 0x3C, 0x3C, 0x7C },//0,0
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0x7C, 0x1C },
        { 8, 8, DM_MOUNTAIN, 0x0C, 0x0C, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F },//1,1
        { 8, 8, DM_MOUNTAIN, 0x0F, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00 },

        // 0x08
        // ＼      ／
        //   ＼  ／
        //     `
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x0C },//0,0
        { 8, 8, DM_MOUNTAIN, 0x0C, 0x3C, 0x7C, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0x7C },
        { 8, 8, DM_MOUNTAIN, 0x1C, 0x1C, 0x0C, 0x0C, 0x04, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x3F, 0x7F },//1,1
        { 8, 8, DM_MOUNTAIN, 0x7F, 0x3F, 0x3F, 0x1F, 0x07, 0x03, 0x00, 0x00 },

        // 0x0E
        // ＼       ／
        //   \    ／
        //    ＼／
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0C, 0x0C, 0x1C },//0,0
        { 8, 8, DM_MOUNTAIN, 0x7C, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0x7C, 0x7C, 0x3C, 0x3C, 0x1C, 0x0C, 0x0C, 0x04 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x01, 0x03, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF },//1,1
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 8, 8, DM_MOUNTAIN, 0x7F, 0x3F, 0x1F, 0x07, 0x03, 0x01, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F },//1,2
        { 8, 8, DM_MOUNTAIN, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x01, 0x01, 0x00 },

        
        // 0x18
        // /`＼
        { 8, 8, DM_MOUNTAIN, 0x20, 0x38, 0x3E, 0x3F, 0x3F, 0x3E, 0x3E, 0x3C },
        { 8, 8, DM_MOUNTAIN, 0x3C, 0x38, 0x38, 0x30, 0x30, 0x20, 0x00, 0x00 },
 
        // 0x1A
        //     _
        //   ／  \.
        // ／     ＼
        { 8, 8, DM_MOUNTAIN, 0x20, 0x30, 0x30, 0x38, 0x38, 0x3C, 0x3C, 0x3E },//0,0
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3E, 0x38 },
        { 8, 8, DM_MOUNTAIN, 0x30, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xE0, 0xE0, 0xF0 },//1,-1
        { 8, 8, DM_MOUNTAIN, 0xF0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00 },

        // 0x20
        //     .
        //   ／  ＼
        // ／      ＼
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x30 },//0,0
        { 8, 8, DM_MOUNTAIN, 0x30, 0x3C, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3E },
        { 8, 8, DM_MOUNTAIN, 0x38, 0x38, 0x30, 0x30, 0x20, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xFC, 0xFE },//1,-1
        { 8, 8, DM_MOUNTAIN, 0xFE, 0xFC, 0xFC, 0xF8, 0xE0, 0xC0, 0x00, 0x00 },
        
        // 0x26
        //    ／＼
        //   /    ＼
        // ／       ＼
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x20, 0x30, 0x30, 0x38 },//0,0
        { 8, 8, DM_MOUNTAIN, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3E, 0x3E, 0x3C, 0x3C, 0x38, 0x30, 0x30, 0x20 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x80, 0xE0, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF },//1,-1
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 8, 8, DM_MOUNTAIN, 0xFE, 0xFC, 0xF8, 0xE0, 0xC0, 0x80, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0 },//1,-2
        { 8, 8, DM_MOUNTAIN, 0xF0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x80, 0x00 },
        
        // 0x30
        //     ／＼
        //    /    \.
        //   /      ＼
        // ／         ＼
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x20, 0x20, 0x30, 0x30, 0x38, 0x3C },//0,0
        { 8, 8, DM_MOUNTAIN, 0x3C, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
        { 8, 8, DM_MOUNTAIN, 0x3F, 0x3E, 0x3E, 0x3E, 0x3C, 0x3C, 0x38, 0x30 },
        { 8, 8, DM_MOUNTAIN, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF8, 0xFC, 0xFF },//1,-1
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xC0, 0x80, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0xC0, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },//2,-2
        { 8, 8, DM_MOUNTAIN, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC },
        { 8, 8, DM_MOUNTAIN, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 8, 8, DM_MOUNTAIN, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xF8, 0xFC },//2,-3
        { 8, 8, DM_MOUNTAIN, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00 },
        
        // 0x40
        // tree top 1
        { 8, 8, DM_TREE, 0x04, 0x1C, 0x0C, 0x1C, 0x3C, 0x0C, 0x04, 0x00 },
        { 8, 8, DM_TREE, 0x38, 0xFC, 0x38, 0x7C, 0x18, 0x00, 0x00, 0x00 },
        
        // 0x42
        // tree top 2
        { 8, 8, DM_TREE, 0x00, 0x04, 0x04, 0x0C, 0x04, 0x0C, 0x1C, 0x7C },
        { 8, 8, DM_TREE, 0x3C, 0x1C, 0xFC, 0x3C, 0x0C, 0x04, 0x38, 0x7C },
        { 8, 8, DM_TREE, 0x18, 0x3C, 0x18, 0x00, 0x38, 0x1C, 0x78, 0x3C },
        
        // 0x45
        // tree bottom 1
        { 8, 8, DM_TREE, 0x1C, 0x38, 0x1C, 0x38, 0x00, 0x1C, 0x3F, 0x1E },

        // 0x46
        // tree bottom 2
        { 8, 8, DM_TREE, 0x10, 0x38, 0x30, 0x00, 0x30, 0x3C, 0x20, 0x1F },
        { 8, 8, DM_TREE, 0x3C, 0x1F, 0x3E, 0x1C, 0x30, 0x3C, 0x20, 0x30 },
        
        // 0x48
    };
    
    
    const struct Collision {
        s8 l;
        s8 t;
        u8 w;
        u8 h;
    } BG_COL[] = {
        // 0x00
        { 2, 2, 6, 4 },
        { 0, 2, 3, 2 },
      
        // 0x02
        { 2, 2, 6, 4 },//0,0
        { 0, 2, 8, 6 },
        { 0, 2, 8, 6 },
        { 0, 0, 0, 0 },
        { 5, 0, 3, 2 },//1,1
        { 0, 0, 2, 2 },

        // 0x08
        { 0, 0, 0, 0 },//0,0
        { 3, 2, 5, 6 },
        { 0, 2, 8, 6 },
        { 0, 2, 2, 3 },
        { 5, 0, 3, 4 },//1,1
        { 0, 0, 4, 5 },

        // 0x0E
        { 0, 0, 0, 0 },//0,0
        { 1, 2, 7, 6 },
        { 0, 2, 8, 6 },
        { 0, 2, 8, 6 },
        { 0, 2, 4, 4 },
        { 4, 0, 4, 7 },//1,1
        { 0, 0, 8, 8 },
        { 0, 0, 3, 5 },
        { 7, 0, 1, 2 },//1,2
        { 0, 0, 3, 2 },

        // 0x18
        { 2, 2, 6, 4 },
        { 0, 4, 3, 2 },

        // 0x1A
        { 2, 2, 6, 4 },//0,0
        { 0, 0, 8, 6 },
        { 0, 0, 8, 6 },
        { 0, 0, 0, 0 },
        { 5, 6, 3, 2 },//1,-1
        { 0, 6, 2, 2 },

        // 0x20
        { 0, 0, 0, 0 },//0,0
        { 3, 0, 5, 6 },
        { 0, 0, 8, 6 },
        { 0, 3, 2, 3 },
        { 5, 4, 3, 4 },//1,-1
        { 0, 3, 4, 5 },

        // 0x26
        { 0, 0, 0, 0 },//0,0
        { 1, 0, 7, 6 },
        { 0, 0, 8, 6 },
        { 0, 0, 8, 6 },
        { 0, 2, 4, 4 },
        { 4, 1, 4, 7 },//1,-1
        { 0, 0, 8, 8 },
        { 0, 3, 3, 5 },
        { 7, 6, 1, 2 },//1,-2
        { 0, 6, 3, 2 },

        // 0x30
        { 6, 4, 2, 2 },//0,0
        { 2, 0, 6, 6 },
        { 0, 0, 8, 6 },
        { 0, 0, 8, 6 },
        { 0, 0, 8, 6 },
        { 0, 0, 8, 6 },
        { 0, 2, 6, 4 },
        { 5, 3, 3, 5 },//1,-1
        { 0, 0, 8, 8 },
        { 0, 0, 8, 8 },
        { 0, 0, 3, 8 },
        { 3, 0, 5, 8 },//2,-2
        { 0, 0, 7, 8 },
        { 0, 0, 0, 0 },
        { 5, 4, 3, 4 },//2,-3
        { 0, 5, 4, 3 },
        
        // 0x40
    };

    const u8 STAGE[][8] = {
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        
        { 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x03, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x04, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x45 },
        
        { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x26 },
        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0x2E, 0x2B, 0x27 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F, 0x2C, 0x28 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2D, 0x29 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2A },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x45 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x42, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        
        { 0x43, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18 },
        { 0x44, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x19 },
               
        { 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x09, 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x0A, 0x0D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },
        { 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x24, 0x21 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x25, 0x22 },
        { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x23 },

        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },

        { 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x0F, 0x13, 0x16, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x10, 0x14, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x11, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },
        { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x37, 0x31 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x3B, 0x38, 0x32 },
        { 0x00, 0xFF, 0xFF, 0xFF, 0x3F, 0x3C, 0x39, 0x33 },
        { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x3D, 0x3A, 0x34 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x35 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x36 },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x45 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x19 },
        
        { 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x03, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x04, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        // 1500
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x24, 0x21 },
        { 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x25, 0x22 },
        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x23 },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0x1B },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x1C },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1D },

        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },

        // 2000
        { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        { 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x0F, 0x13, 0x16, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x10, 0x14, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x11, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30 },
        { 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x37, 0x31 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x3B, 0x38, 0x32 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x3C, 0x39, 0x33 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3D, 0x3A, 0x34 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x35 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x36 },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0x41, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },

        // 2600
        { 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },
        { 0x09, 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x26 },
        { 0x0A, 0x0D, 0xFF, 0xFF, 0xFF, 0x2E, 0x2B, 0x27 },
        { 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F, 0x2C, 0x28 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2D, 0x29 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2A },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

        // 3000
        { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A },
        { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0x1B },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x1C },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1D },

        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x26 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2E, 0x2B, 0x27 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F, 0x2C, 0x28 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2D, 0x29 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2A },
        
        // 3200
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x46 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47 },
        
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x37, 0x31 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x3B, 0x38, 0x32 },
        { 0x00, 0xFF, 0xFF, 0xFF, 0x3F, 0x3C, 0x39, 0x33 },
        { 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x3D, 0x3A, 0x34 },
        //{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x35 },
        //{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x36 },
        
        //{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30 },
        { 0x42, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x37, 0x31 },
        { 0x43, 0xFF, 0xFF, 0xFF, 0x3E, 0x3B, 0x38, 0x32 },
        { 0x44, 0xFF, 0xFF, 0xFF, 0x3F, 0x3C, 0x39, 0x33 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3D, 0x3A, 0x34 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x35 },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x36 },

    };
    
    static const u16 STAGE_TW = sizeof(STAGE) >> 3;
    
    enum CMD {
        CMD_END,
        CMD_EM_01,
        CMD_EM_02,
        CMD_EM_03,
        CMD_EM_04,
        CMD_EM_05,
    };
    
    struct Command {
        u16 type;
        s8 x;
        s8 y;
        u8 cfg;
    };
    
    const struct Tag {
        u16 frame;
        Command cmd;
    } LEVEL[] = {
        { 60, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 70, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 80, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 90, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 200, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 210, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 220, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 230, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 300, { CMD_EM_02, VIEW_W, POS_Y_7 } },
        { 350, { CMD_EM_02, VIEW_W, POS_Y_4 } },
        { 400, { CMD_EM_03, VIEW_W, POS_Y_6 } },
        { 405, { CMD_EM_03, VIEW_W, POS_Y_3 } },

        { 440, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 450, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 460, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 470, { CMD_EM_01, VIEW_W, POS_Y_2, 0 } },
        { 490, { CMD_EM_02, VIEW_W, POS_Y_4 } },
        { 520, { CMD_EM_02, VIEW_W, POS_Y_9 } },
        { 570, { CMD_EM_03, VIEW_W, POS_Y_2 } },
        { 620, { CMD_EM_03, VIEW_W, POS_Y_10 } },
        { 660, { CMD_EM_03, VIEW_W, POS_Y_4 } },
        
        { 680, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 690, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 700, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 710, { CMD_EM_01, VIEW_W, POS_Y_12, 1 } },
        { 750, { CMD_EM_04, VIEW_W, POS_Y_10 } },
        { 780, { CMD_EM_04, VIEW_W, POS_Y_8 } },
        { 794, { CMD_EM_05, VIEW_W, 2, 0 } },
        { 810, { CMD_EM_04, VIEW_W, POS_Y_6 } },
        
        { 866, { CMD_EM_05, VIEW_W, 54, 1 } },
        { 898, { CMD_EM_05, VIEW_W, 2, 0 } },
        { 938, { CMD_EM_05, VIEW_W, 2, 0 } },
        
        { 950, { CMD_EM_02, VIEW_W, POS_Y_3 } },
        { 990, { CMD_EM_02, VIEW_W, POS_Y_6 } },
        
        { 1140, { CMD_EM_05, VIEW_W, 2, 0 } },
        { 1160, { CMD_EM_04, VIEW_W, POS_Y_12 } },
        { 1180, { CMD_EM_04, VIEW_W, POS_Y_8 } },
        { 1180, { CMD_EM_04, VIEW_W, POS_Y_10 } },
        
        //EM_06
        //EM_06
        
        { 1300, { CMD_EM_04, VIEW_W, POS_Y_7 } },
        { 1320, { CMD_EM_05, VIEW_W, 54, 1 } },
        { 1360, { CMD_EM_05, VIEW_W, 54, 1 } },
        { 1380, { CMD_EM_04, VIEW_W, POS_Y_3 } },

        //EM_06
        //EM_07
        
        { 1400, { CMD_EM_04, VIEW_W, POS_Y_7 } },
        { 1480, { CMD_EM_02, VIEW_W, POS_Y_4 } },
        { 1480, { CMD_EM_02, VIEW_W, POS_Y_9 } },
        { 1520, { CMD_EM_04, VIEW_W, POS_Y_1 } },
        { 1520, { CMD_EM_02, VIEW_W, POS_Y_5 } },

        { 4000, { CMD_END } },
    };
    
    inline f32 frand() { return static_cast<f32>(rand()) / RAND_MAX; }
}

Gradius::Gradius()
	: mPause(false)
	, mFrameCount(0)
	, mCommandPt(0)
{
}

void Gradius::init()
{
    super::init();
    
    mPause = false;
    mFrameCount = 2600;
    
    memset(&mLevel, 0, sizeof(mLevel));

    memset(&mPlayer, 0, sizeof(mPlayer));
    mPlayer.x = 8;
    mPlayer.y = 24;
    mPlayer.speed = 4;
    
    memset(&mBG, 0, sizeof(mBG));
    genBGImage();
    
    memset(mShot, 0, sizeof(mShot));
    memset(mEnemy, 0, sizeof(mEnemy));
    memset(mParticle, 0, sizeof(mParticle));
    memset(mVFX, 0, sizeof(mVFX));
    
    System::BtnInfo& btnRed = mSystem.getBtnRed();
    btnRed.rep1_count = btnRed.rep_count = 24;
    
    mRender.setAnimSpeed(0);
}

void Gradius::update()
{
    const System::BtnInfo& btnLeft  = mSystem.getBtnLeft();
    const System::BtnInfo& btnRight = mSystem.getBtnRight();
    const System::BtnInfo& btnUp    = mSystem.getBtnUp();
    const System::BtnInfo& btnDown  = mSystem.getBtnDown();
    const System::BtnInfo& btnRed   = mSystem.getBtnRed();
    const System::BtnInfo& btnGreen = mSystem.getBtnGreen();
    const System::BtnInfo& btnBlue  = mSystem.getBtnBlue();
    const u8 dip = mSystem.dip();

#if DEV
    if (mSystem.isFrameWait()) {
        if (btnBlue.on)
            mSystem.setFrameWait(false);
    } else {
        if (btnBlue.rel)
            mSystem.setFrameWait(true);
    }
#endif

    if (btnGreen.trg) {
        mPause = !mPause;
    }
    if (!mPause) {
        s8 dx = 0;
        s8 dy = 0;

        if (btnLeft.on) {
            dx = -1;
        } else if (btnRight.on) {
            dx = 1;
        }
        if (btnUp.on) {
            dy = -1;
        } else if (btnDown.on) {
            dy = 1;
        }

        if (mPlayer.dead) {
            if (++mLevel.cnt >= 120) {
                mFrameCount = 0;
                memset(&mLevel, 0, sizeof(mLevel));
                memset(&mPlayer, 0, sizeof(mPlayer));
                mPlayer.x = 8;
                mPlayer.y = 24;
                mPlayer.speed = 4;
                memset(mShot, 0, sizeof(mShot));
                memset(mEnemy, 0, sizeof(mEnemy));
                memset(mParticle, 0, sizeof(mParticle));
                memset(mVFX, 0, sizeof(mVFX));
            }
        } else {
            if (btnRed.rep) {
                genShot();
            }
            
            const u32 fc = mSystem.getFrameCount();
            if ((dx || dy) && fc % mPlayer.speed == 0) {
                mPlayer.x_old = mPlayer.x;
                mPlayer.y_old = mPlayer.y;
                mPlayer.x += dx;
                mPlayer.y += dy;
                if (mPlayer.x < BORDER_L) mPlayer.x = BORDER_L;
                if (BORDER_R <= mPlayer.x + PL_W) mPlayer.x = BORDER_R - PL_W;
                if (mPlayer.y < BORDER_T) mPlayer.y = BORDER_T;
                if (BORDER_B <= mPlayer.y + PL_H) mPlayer.y = BORDER_B - PL_H;
            }
        }
        
        mBG.x = mFrameCount >> 2;
        mBG.anim = mFrameCount;
        
        evalCommand();
        
        ++mFrameCount;
    }
    
    mRender.clear();
    
    drawBackground();    
    
    updateShot();
    updateEnemy();
    
    if (!mPause && !mPlayer.dead) {
        if (isCollidedWithBG(mPlayer.x + PL_DC_L, mPlayer.y + PL_DC_T, PL_DC_W, PL_DC_H)) {
            mPlayer.hit = true;
        }

#if DEV
        // muteki
        if (dip & 0x1) {
            mPlayer.hit = false;
        }
#endif

        if (mPlayer.hit) {
            mPlayer.dead = true;
            genVFX(VFX_01, mPlayer.x - 4, mPlayer.y);
        }
    }
    
    updateParticle();
    updateVFX();

    if (!mPlayer.dead) {
        mRender.setDrawMode(CHR[PAT_PL].dm);
        mRender.drawImage(VIEW_L + mPlayer.x, VIEW_T + mPlayer.y, CHR[PAT_PL].pat, CHR[PAT_PL].w, CHR[PAT_PL].h);
    }
    
    mRender.setDrawMode(DM_NEGATIVE);
    mRender.drawFillRect(0, 0, VIEW_L, Render::SCREEN_HEIGHT);
    mRender.drawFillRect(VIEW_R, 0, Render::SCREEN_WIDTH - VIEW_R, Render::SCREEN_HEIGHT);
    
#if DEV
    mRender.setDrawMode(DM_ADD);
    mSystem.drawInfo(mRender);
    mRender.drawString(0, 24, "%u", mFrameCount);
    if (mPause && mSystem.getFrameCount() % 40 > 15)
        mRender.drawString(0, 30, "Pause");
#endif

    mRender.present();
    super::update();    
}

// procedural image
void Gradius::genBGImage()
{
    BGImage &top = mBG.top;
    BGImage &bot = mBG.bot;
    
    top.h = 2;
    top.v = 1;
    u8 tc = 0, tn = 2;
    
    bot.h = 2;
    bot.v = 1;
    u8 bc = 0, bn = 3;
    
    for (u8 i = 0; i < 128; ++i) {
        top.pat[i] = (~(0xFF << top.h)) & 0xFF;
        if (++tc >= tn) {
            tc = 0;
            tn = rand() % 11;
            top.h += top.v;
            if (top.h == 2 || top.h == 3)
                top.v *= -1;
        }
        
        bot.pat[i] = (~(0xFF >> bot.h)) & 0xFF;
        if (++bc >= bn) {
            bc = 0;
            bn = rand() % 11;
            bot.h += bot.v;
            if (bot.h == 2 || bot.h == 3)
                bot.v *= -1;
        }
    }
}

void Gradius::drawBackground()
{
    const u16 tx = mBG.x >> 3;
    const u8 scrollx = mBG.x & 0x7;
    for (u8 x = 0; x < VIEW_TW + 1; ++x) {
        for (u8 y = 0; y < VIEW_TH; ++y) {
            const u8 tpat = STAGE[(tx + x) % STAGE_TW][y];
            if (tpat == 0xFF)
                continue;
            
            const Image &img = BG[tpat];
            mRender.setDrawMode(img.dm);
            mRender.drawImage(VIEW_L + (x << 3) - scrollx, VIEW_T + (y << 3), img.pat, img.w, img.h);
            
#if 0//DEV
            if (tpat >= 0x40)
                continue;
            const Collision &c = BG_COL[tpat];
            mRender.setDrawMode(DM_INVERSE);
            mRender.drawFillRect(VIEW_L + (x << 3) - scrollx + c.l, VIEW_T + (y << 3) + c.t, c.w, c.h);
#endif
        }
    }

    const s8 shiftx = mBG.anim & 0x7F;
    mRender.setDrawMode(DM_ADD);
    mRender.drawImageShift(shiftx, 0, VIEW_L, VIEW_T, mBG.top.pat, VIEW_W);
    mRender.drawImageShift(shiftx, 0, VIEW_L, VIEW_B - 8, mBG.bot.pat, VIEW_W);
    //mRender.drawFillRect(VIEW_L, 0, VIEW_W, 2);
    //mRender.drawFillRect(VIEW_L, VIEW_H - 2, VIEW_W, 2);
}

void Gradius::genShot()
{
    if (mPlayer.scnt >= 2)
        return;
    
    for (u8 i = 0; i < SHOT_MAX; ++i) {
        Shot &s = mShot[i];
        if (s.enable)
            continue;

        s.type       = TT_SHOT;
        s.enable     = true;
        s.subtype    = SHOT_01;
        s.x          = mPlayer.x + PL_W - 6;
        s.y          = mPlayer.y + 3;
        s.vx         = 2;
        s.vy         = 0;
        s.w          = 6;
        s.h          = 2;
        
        ++mPlayer.scnt;
        break;
    }
}

void Gradius::updateShot()
{
    for (u8 i = 0; i < SHOT_MAX; ++i) {
        Shot &s = mShot[i];
        if (!s.enable)
            continue;
            
        switch (s.subtype) {
        case SHOT_01:
        default:
            if (!mPause) {
                if (isCollidedWithBG(s.x, s.y, s.w, s.h)) {
                    s.enable = false;
                    --mPlayer.scnt;
                }
                
                if (!s.enable)
                    break;

                s.x += s.vx;
                s.y += s.vy;
                if (s.x + s.w < 0 || VIEW_W < s.x || s.y + s.h < 0 || VIEW_H < s.y) {
                    s.enable = false;
                    --mPlayer.scnt;
                    break;
                }
            }
            
            mRender.setDrawMode(DM_POSITIVE);
            mRender.drawFillRect(VIEW_L + s.x, VIEW_T + s.y, 2, s.h);
            mRender.drawFillRect(VIEW_L + s.x + 3, VIEW_T + s.y, 2, s.h);
            //mRender.setDrawMode(DM_PAT_3);
            //mRender.drawFillRect(VIEW_L + s.x + 2, VIEW_T + s.y, 1, s.h);
            break;
        }
    }
}

void Gradius::evalCommand()
{
    while (mFrameCount == LEVEL[mLevel.pc].frame) {
        const Command &cmd = LEVEL[mLevel.pc].cmd;
        bool end = false;
        switch (cmd.type) {
        case CMD_EM_01:
            genEnemy(EM_01, cmd.x, cmd.y, cmd.cfg, PAT_EM_01);
            break;
            
        case CMD_EM_02:
            genEnemy(EM_02, cmd.x, cmd.y, cmd.cfg, PAT_EM_02);
            break;
            
        case CMD_EM_03:
            genEnemy(EM_03, cmd.x, cmd.y, cmd.cfg, PAT_EM_03);
            break;
        
        case CMD_EM_04:
            genEnemy(EM_04, cmd.x, cmd.y, cmd.cfg, PAT_EM_04);
            break;

        case CMD_EM_05:
            genEnemy(EM_05, cmd.x, cmd.y, cmd.cfg, PAT_EM_05);
            break;

        case CMD_END:
            end = true;
            break;
            
        default:
            break;
        }
        
        if (end || ++mLevel.pc >= sizeof(STAGE) / sizeof(Tag)) {
            mFrameCount = mLevel.pc = 0;
        }
    }
    
    // ignore
    while (mFrameCount > LEVEL[mLevel.pc].frame) {
        ++mLevel.pc;
    }
}

Enemy* Gradius::genEnemy(u8 subtype, s8 x, s8 y, u8 cfg, u8 pat)
{
    for (u8 i = 0; i < ENEMY_MAX; ++i) {
        Enemy &e = mEnemy[i];
        if (e.enable)
            continue;
        
        memset(&e, 0, sizeof(Enemy));
        e.type      = TT_ENEMY;
        e.enable    = true;
        e.init      = false;
        e.subtype   = subtype;
        e.x         = x;
        e.y         = y;
        e.vx        = -1;
        e.vy        = 0;
        e.cfg       = cfg;
        e.pat       = pat;
        e.w         = CHR[pat].w;
        e.h         = CHR[pat].h;
        return &e;
    }
    return NULL;
}

void Gradius::updateEnemy()
{
    for (u8 i = 0; i < ENEMY_MAX; ++i) {
        Enemy &e = mEnemy[i];
        if (!e.enable)
            continue;
        
        if (!mPause) {
            if (e.subtype < EM_SHOT_01) {
                for (u8 j = 0; j < SHOT_MAX; ++j) {
                    Shot &s = mShot[j];
                    if (!s.enable)
                        continue;
                    
                    if (isCollided(s.x, s.y, s.w, s.h, e.x, e.y, e.w, e.h)) {
                        s.enable = false;
                        --mPlayer.scnt;
                        
                        e.enable = false;
                        emitParticle(e.x + (e.w >> 1), e.y + (e.h >> 1));
                        for (u8 i = 0; i < 3; ++i) {
                            genVFX(VFX_02, e.x + i + ((frand() - 0.2f) * 6.f), e.y + ((frand() - 0.5f) * 6.f), i * 3);
                        }
                        break;
                    }
                }
            }

            if (!e.enable)
                continue;
                
            if (isCollided(mPlayer.x + PL_DC_L, mPlayer.y + PL_DC_T, PL_DC_W, PL_DC_H, e.x, e.y, e.w, e.h)) {
                mPlayer.hit = true;
            }
            
            switch (e.subtype) {
            case EM_01:
                if (!e.init) {
                    e.init = true;
                    e.wk._u8[2] = 0;
                    e.anim = e.ano1 == 3 ? 1 : e.ano1;
                }
            
                e.x += e.vx;
                e.y += e.vy;
                if (++e.wk._u8[2] >= 3) {
                    e.wk._u8[2] = 0;
                    e.x += e.vx;
                }
                if (++e.ano0 >= 3) {
                    e.ano0 = 0;
                    e.ano1 = (e.ano1 + 1) & 0x3;
                    e.anim = e.ano1 == 3 ? 1 : e.ano1;
                }
                
                switch (e.rno0) {
                case 0:
                    if (e.x < POS_X_4) {
                        e.vx = 1;
                        e.vy = e.cfg ? -1 : 1;
                        e.wk._u8[0] = 0;
                        ++e.rno0;
                    }
                    break;
                case 1:
                    if (++e.wk._u8[0] >= 8) {
                        e.vx = -1;
                        e.vy = 0;
                        ++e.rno0;
                    }
                    break;
                case 2:
                    if (e.x < POS_X_2) {
                        e.vx = 1;
                        e.vy = e.cfg ? -1 : 1;
                        e.wk._u8[0] = 0;
                        e.wk._s8[1] = (e.cfg ? -1 : 1) * (mPlayer.y - e.y);
                        e.rno0 = e.wk._s8[1] < 0 ? 4 : 3;
                    }
                    break;
                case 3:
                    if (++e.wk._u8[0] >= e.wk._u8[1]) {
                        e.vx = 1;
                        e.vy = 0;
                        ++e.rno0;
                    }
                    break;
                case 4:
                    if (e.x >= VIEW_W || e.y >= VIEW_H) {
                        e.enable = false;
                    }
                    break;
                default:
                    break;
                }
                break;
                
            case EM_02:
                if (!e.init) {
                    e.init = true;
                    e.wk._u8[0] = 0;
                    e.wk._f32[1] = M_PI;
                    e.wk._f32[3] = static_cast<f32>(e.y) + 0.5f;
                }
            
                e.x += e.vx;
                if (++e.wk._u8[0] >= 2) {
                    e.wk._u8[0] = 0;
                    e.wk._f32[1] += M_PI / 12.f;
                    if (e.wk._f32[1] >= 2.f * M_PI) {
                        e.wk._f32[1] -= 2.f * M_PI;
                    }
                    e.wk._f32[2] = 2.f * cos(e.wk._f32[1]);
                    e.wk._f32[3] += e.wk._f32[2];
                    e.y = static_cast<s8>(e.wk._f32[3]);
                }
                if (++e.ano0 >= 4) {
                    e.ano0 = 0;
                    e.ano1 = (e.ano1 + 1) & 0x3;
                    e.anim = e.ano1 == 3 ? 1 : e.ano1;
                }
                
                if (e.x + e.w < 0) {
                    e.enable = false;
                }
                break;
                
            case EM_03:
                if (!e.init) {
                    e.init = true;
                    e.wk._u8[0] = 0;
                    e.wk._u8[1] = 0;
                    e.wk._u8[4] = PL_H >> 2;
                    e.wk._u8[5] = (PL_H * 3) >> 2;
                    e.wk._u8[6] = e.h >> 2;
                    e.wk._u8[7] = (e.h * 3) >> 2;
                }
            
                e.x += e.vx;
                if (++e.wk._u8[0] >= (e.wk._u8[1] ? 1 : 2)) {
                    e.wk._u8[0] = 0;
                    e.wk._u8[1] = (e.wk._u8[1] + 1) % 3;
                    e.y += e.vy;
                }
                if (e.y + e.wk._u8[6] > mPlayer.y + e.wk._u8[5] || mPlayer.y + e.wk._u8[4] > e.y + e.wk._u8[7]) {
                    const bool up = (mPlayer.y - e.y) < 0;
                    e.vy = up ? -1 : 1;
                    e.anim = up ? 1 : 2;
                } else {
                    e.vy = 0;
                    e.anim = 0;
                }
                
                if (e.x + e.w < 0) {
                    e.enable = false;
                }
                break;
                
            case EM_04:
                if (!e.init) {
                    e.init = true;
                    e.wk._u8[0] = 0;
                    e.wk._f32[1] = static_cast<f32>(mPlayer.y - e.y) / (e.x - mPlayer.x);
                    e.wk._f32[2] = static_cast<f32>(e.y);
                    
                    e.vx = -1;
                    e.vy = 0;
                }
                
                switch (e.rno0) {
                case 0:
                    if (++e.wk._u8[0] >= 16) {
                        e.wk._f32[1] = static_cast<f32>(mPlayer.y - e.y) / (e.x - mPlayer.x);
                        e.wk._f32[2] = static_cast<f32>(e.y);
                        e.wk._u8[0] = 0;
                        ++e.rno0;
                    }
                    break;
                default:
                    if (++e.wk._u8[0] >= 2) {
                        e.wk._u8[0] = 0;
                        e.x += e.vx;
                    }
                    break;
                }
                
                e.x += e.vx;
                e.wk._f32[2] += e.wk._f32[1];
                e.y = static_cast<s8>(e.wk._f32[2]);
                if (++e.ano0 >= 2) {
                    e.ano0 = 0;
                    e.anim = (e.anim + 1) & 0x7;
                }
                
                if (e.x + e.w < 0) {
                    e.enable = false;
                }
                break;
                
            case EM_05:
                if (!e.init) {
                    e.init = true;
                    if (e.cfg)
                        e.pat += 3;//bot
                }

                if (++e.wk._u8[0] >= 4) {
                    e.wk._u8[0] = 0;
                    --e.x;
                }

                if (mPlayer.x < e.x) {
                    e.anim = 1;
                } else if (e.x + e.w < mPlayer.x) {
                    e.anim = 2;
                } else {
                    e.anim = 0;
                }
                
                if (++e.wk._u16[1] >= (e.rno0 ? 120 : 100)) {
                    if (e.rno0 == 0) {
                        ++e.rno0;
                    }
                    e.wk._u16[1] = 0;
                    genEnemy(EM_SHOT_01, e.x + 3, e.y + (e.cfg ? 4 : 0), 0, PAT_EM_SHOT_01);
                }
                
                if (e.x + e.w < 0) {
                    e.enable = false;
                }
                break;
                
            case EM_SHOT_01:
                if (!e.init) {
                    e.init = true;
                    e.wk._f32[0] = (mPlayer.x + PL_W / 2) - e.x;
                    e.wk._f32[1] = (mPlayer.y + PL_H / 2) - e.y;
                    
                    f32 len = sqrt(e.wk._f32[0] * e.wk._f32[0] + e.wk._f32[1] * e.wk._f32[1]);
                    len *= 1.4f;
                    e.wk._f32[0] /= len;
                    e.wk._f32[1] /= len;
                    
                    e.wk._f32[2] = e.x;
                    e.wk._f32[3] = e.y;
                }
                
                e.wk._f32[2] += e.wk._f32[0];
                e.wk._f32[3] += e.wk._f32[1];
                
                e.x = static_cast<s8>(e.wk._f32[2]);
                e.y = static_cast<s8>(e.wk._f32[3]);

                if (e.x + e.w < 0 || VIEW_W < e.x || e.y + e.h < 0 || VIEW_H < e.y) {
                    e.enable = false;
                }
                break;
    
            default:
                break;
            }
            
            if (!e.enable)
                continue;
        }
        
        const Image &img = CHR[e.pat + e.anim];
        mRender.setDrawMode(img.dm);
        mRender.drawImage(VIEW_L + e.x, VIEW_T + e.y, img.pat, img.w, img.h);
    }
}

void Gradius::emitParticle(f32 x, f32 y)
{
    u8 c = 0;
    for (u8 i = 0; i < PARTICLE_MAX; ++i) {
        Particle &p = mParticle[i];
        if (p.enable)
            continue;
            
        p.type = TT_PARTICLE;
        p.enable = true;
        p.x = x;
        p.y = y;
        p.vx = 4.f * (frand() - 0.9f);
        p.vy = 3.f * (frand() - 0.5f);
        p.w = 1;
        p.h = 1;
        p.cnt = 0;

        if (++c >= 8)
            break;
    }
}

void Gradius::updateParticle()
{
    mRender.setDrawMode(DM_ADD);
    for (u8 i = 0; i < PARTICLE_MAX; ++i) {
        Particle &p = mParticle[i];
        if (!p.enable)
            continue;
        
        if (!mPause) {
            p.x_old = p.x;
            p.y_old = p.y;
            p.x += mParticle[i].vx;
            p.y += mParticle[i].vy;
            p.vx *= 0.95f;
            p.vy *= 0.95f;
            //p.vy += 0.2f;
            if (p.x < 0.f || VIEW_W <= p.x + p.w) {
                p.enable = false;
                //mParticle[i].vx *= -1.f;
            }    
            if (++p.cnt >= 24 || VIEW_H <= p.y) {
                p.enable = false;
                continue;
            }
        }
        
        mRender.drawFillRect(VIEW_L + static_cast<s8>(p.x_old), VIEW_T + static_cast<s8>(p.y_old), p.w, p.h);
        mRender.drawFillRect(VIEW_L + static_cast<s8>(p.x), VIEW_T + static_cast<s8>(p.y), p.w, p.h);
    }
}

void Gradius::genVFX(u8 subtype, s8 x, s8 y, u8 delay)
{
    for (u8 i = 0; i < VFX_MAX; ++i) {
        VFX &f = mVFX[i];
        if (f.enable)
            continue;
        
        memset(&f, 0, sizeof(VFX));
        f.type = TT_VFX;
        f.enable = true;
        f.subtype = subtype;
        f.x = x;
        f.y = y;
        f.delay = delay;

        switch (subtype) {
        case VFX_01:
            f.pat = PAT_VFX_01;
            break;
            
        case VFX_02:
            f.pat = PAT_VFX_02;
            break;
        }
        
        break;
    }
}

void Gradius::updateVFX()
{
    for (u8 i = 0; i < VFX_MAX; ++i) {
        VFX &f = mVFX[i];
        if (!f.enable)
            continue;
        
        if (!mPause) {
            if (f.delay > 0) {
                --f.delay;
                continue;
            }
            
            switch (f.subtype) {
            case VFX_01:
                if (++f.ano0 >= 3) {
                    f.ano0 = 0;
                    f.anim = (f.anim + 1) % 8;
                }
                if (++f.cnt >= 32) {
                    f.enable = false;
                }
                break;

            case VFX_02:
                if (++f.ano0 >= 2) {
                    f.ano0 = 0;
                    ++f.anim;
                    switch (f.ano1++) {
                    case 0:
                        f.x += 2;
                        break;
                    case 1:
                        f.x += 4;
                        break;
                    case 2:
                        f.x += 5;
                        break;
                    case 3:
                        f.enable = false;
                        break;
                    }
                }
                break;
            }
            
            if (!f.enable)
                continue;
        }
        
        const Image &img = CHR[f.pat + f.anim];
        mRender.setDrawMode(img.dm);
        mRender.drawImage(VIEW_L + f.x, VIEW_T + f.y, img.pat, img.w, img.h);
    }
}

bool Gradius::isCollidedWithBG(s8 x, s8 y, u8 w, u8 h) const
{
    const s8 lr[] = { static_cast<s8>(x & ~0x7), static_cast<s8>((x + w) & ~0x7) };
    const s8 tb[] = { static_cast<s8>(y & ~0x7), static_cast<s8>((y + h) & ~0x7) };
    const u8 nx = lr[0] == lr[1] ? 1 : 2;
    const u8 ny = tb[0] == tb[1] ? 1 : 2;

    const u8 tx = mBG.x >> 3;
    const u8 sx = mBG.x & 0x7;
    
    for (u8 ix = 0; ix < nx; ++ix) {
        for (u8 iy = 0; iy < ny; ++iy) {
            const u8 tpat = STAGE[tx + (lr[ix] >> 3)][tb[iy] >> 3];
            if (tpat >= 0x40)
                continue;
                
            const Collision &c = BG_COL[tpat];
            if (isCollided(x, y, w, h, lr[ix] - sx + c.l, tb[iy] + c.t, c.w, c.h)) {
                return true;
            }
        }
    }
    return false;
}
