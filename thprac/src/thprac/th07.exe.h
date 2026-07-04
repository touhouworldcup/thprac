typedef unsigned char   undefined;

typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int    dword;
float10
float2
typedef unsigned long long    GUID;
typedef pointer32 ImageBaseOffset32;

typedef long long    longlong;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    wchar16;
typedef short    wchar_t;
typedef unsigned short    word;
typedef struct Gui Gui, *PGui;

typedef int i32;

typedef uint u32;

typedef struct GuiImpl GuiImpl, *PGuiImpl;

typedef float f32;

typedef struct AnmVM AnmVM, *PAnmVM;

typedef uchar u8;

typedef struct GuiMsgVm GuiMsgVm, *PGuiMsgVm;

typedef int BOOL;

typedef struct GuiFormattedText GuiFormattedText, *PGuiFormattedText;

typedef struct AnmVMPrefix AnmVMPrefix, *PAnmVMPrefix;

typedef struct _D3DVECTOR _D3DVECTOR, *P_D3DVECTOR;

typedef struct _D3DVECTOR D3DXVECTOR3;

typedef short i16;

typedef struct AnmRawInstr AnmRawInstr, *PAnmRawInstr;

typedef struct AnmLoadedSprite AnmLoadedSprite, *PAnmLoadedSprite;

typedef struct ZunVec2 ZunVec2, *PZunVec2;

typedef union ZunColor ZunColor, *PZunColor;

typedef struct ZunTimer ZunTimer, *PZunTimer;

typedef ulong DWORD;

typedef DWORD COLORREF;

typedef struct _D3DMATRIX _D3DMATRIX, *P_D3DMATRIX;

typedef struct _D3DMATRIX D3DXMATRIX;

typedef DWORD D3DCOLOR;

typedef struct ZunColor_struct ZunColor_struct, *PZunColor_struct;

typedef union _union_2605 _union_2605, *P_union_2605;

typedef struct _struct_2606 _struct_2606, *P_struct_2606;

struct ZunVec2 {
    f32 x;
    f32 y;
};

struct AnmLoadedSprite {
    i32 sourceFileIndex;
    struct ZunVec2 startPixelInclusive;
    struct ZunVec2 endPixelExclusive;
    f32 textureHeight;
    f32 textureWidth;
    struct ZunVec2 uvStart;
    struct ZunVec2 uvEnd;
    f32 heightPx;
    f32 widthPx;
    struct ZunVec2 uvScale;
    i32 spriteID;
};

struct _struct_2606 {
    float _11;
    float _12;
    float _13;
    float _14;
    float _21;
    float _22;
    float _23;
    float _24;
    float _31;
    float _32;
    float _33;
    float _34;
    float _41;
    float _42;
    float _43;
    float _44;
};

union _union_2605 {
    struct _struct_2606 field0;
    float m[4][4];
};

struct _D3DMATRIX {
    union _union_2605 field0_0x0;
};

struct _D3DVECTOR {
    float x;
    float y;
    float z;
};

struct ZunTimer {
    i32 previous;
    f32 subFrame;
    i32 current;
};

struct ZunColor_struct {
    u8 b;
    u8 g;
    u8 r;
    u8 a;
};

union ZunColor {
    D3DCOLOR asD3DCOLOR;
    struct ZunColor_struct asStruct;
};

struct AnmVMPrefix {
    D3DXVECTOR3 rotation;
    D3DXVECTOR3 angleVel;
    struct ZunVec2 scale;
    struct ZunVec2 scaleGrowth;
    struct ZunVec2 uvScrollPos;
    struct ZunTimer currentTimeInScript;
    struct ZunTimer waitTimer;
    struct ZunTimer interpCurrentTimes[5];
    struct ZunTimer interpEndTimes[5];
    u8 interpModes[5];
    undefined field10_0xc5;
    undefined field11_0xc6;
    undefined field12_0xc7;
    i32 var0;
    i32 var1;
    i32 var2;
    i32 var3;
    f32 float0;
    f32 float1;
    f32 float2;
    f32 float3;
    i32 var4;
    i32 var5;
    struct ZunVec2 uvScrollVel;
    D3DXMATRIX matrix1;
    D3DXMATRIX matrix2;
    D3DXMATRIX matrix3;
    union ZunColor color1;
    union ZunColor color2;
    u32 flags;
    i16 type;
    i16 pendingInterrupt;
};

struct AnmRawInstr {
};

struct GuiFormattedText {
    D3DXVECTOR3 pos;
    i32 fmtArg;
    i32 type;
    struct ZunTimer timer;
};

struct AnmVM {
    struct AnmVMPrefix prefix;
    D3DXVECTOR3 pos;
    i16 activeSpriteIndex;
    i16 baseSpriteIndex;
    i16 scriptIndex;
    undefined field5_0x1da;
    undefined field6_0x1db;
    struct AnmRawInstr *beginningOfScript;
    struct AnmRawInstr *currentInstruction;
    struct AnmLoadedSprite *loadedSprite;
    D3DXVECTOR3 posInitial;
    D3DXVECTOR3 posFinal;
    D3DXVECTOR3 rotateInitial;
    D3DXVECTOR3 rotateFinal;
    struct ZunVec2 scaleInitial;
    struct ZunVec2 scaleFinal;
    union ZunColor color1Initial;
    union ZunColor color1Final;
    D3DXVECTOR3 pos2;
    i32 timeOfLastSpriteSet;
    u8 fontWidth;
    u8 fontHeight;
    undefined field22_0x242;
    undefined field23_0x243;
    undefined field24_0x244;
    undefined field25_0x245;
    undefined field26_0x246;
    undefined field27_0x247;
    undefined field28_0x248;
    undefined field29_0x249;
    undefined field30_0x24a;
    undefined field31_0x24b;
};

struct GuiMsgVm {
    u8 *msgFile;
    void *currentInstr;
    i32 currentMsgIdx;
    struct ZunTimer timer;
    i32 framesElapsedDuringPause;
    struct AnmVM portraits[2];
    struct AnmVM dialogueLines[2];
    struct AnmVM introLines[2];
    COLORREF textColorsA[4];
    COLORREF textColorsB[4];
    u32 fontSize;
    u32 ignoreWaitCounter;
    bool dialogueSkippable;
    undefined field13_0xe0d;
    undefined field14_0xe0e;
    undefined field15_0xe0f;
};

struct GuiImpl {
    struct AnmVM vms[33];
    u8 bossLifeBarState;
    undefined field2_0x4bcd;
    undefined field3_0x4bce;
    undefined field4_0x4bcf;
    struct AnmVM stageTextSprites[5];
    struct AnmVM playerSpellPortrait;
    struct AnmVM enemySpellPortrait;
    struct AnmVM bombSpellCutInSpriteA;
    struct AnmVM enemySpellCutInSpriteA;
    struct AnmVM bombSpellCutInSpriteB;
    struct AnmVM enemySpellCutInSpriteB;
    struct AnmVM bombSpellName;
    struct AnmVM enemySpellName;
    struct AnmVM bombSpellNameBackground;
    struct AnmVM enemySpellNameBackground;
    struct AnmVM loadingScreenSprite;
    struct AnmVM nowLoadingSprite;
    struct AnmVM arcadeZoneSprite;
    struct AnmVM enemySpellStatsDigit;
    struct AnmVM enemySpellStats;
    struct AnmVM stageTransitionSprites[14][12];
    u32 stageTransitionActiveScriptCount;
    struct GuiMsgVm msg;
    u32 stageClearScreenCounter;
    i32 clearBonusTotal;
    BOOL finishedStage;
    struct GuiFormattedText bonusScore;
    struct GuiFormattedText popupText;
    struct GuiFormattedText spellcardBonus;
    i32 clearBonusPower;
    i32 clearBonusPointItems;
    i32 clearBonusCherryMax;
    i32 clearBonusGraze;
};

struct Gui {
    i32 frameNumber;
    u32 flags;
    struct GuiImpl *impl;
    f32 bombSpellcardBarLength; /* leftover from EoSD */
    f32 blueSpellcardBarLength; /* leftover from EoSD */
    u32 bossUIOpacity;
    i32 eclSetLives;
    i32 spellcardSecondsRemaining;
    i32 previousSpellcardSecondsRemaining;
    bool bossPresent;
    undefined field10_0x25;
    undefined field11_0x26;
    undefined field12_0x27;
    f32 bossLifeBarMaxSize;
    f32 bossLifeBarSize;
    undefined field15_0x30;
    undefined field16_0x31;
    undefined field17_0x32;
    undefined field18_0x33;
    f32 bossLifeBarSegmentStop[8];
    f32 bossLifeBarSegmentStart[8];
    i32 bossLifeBarSegmentColor[8];
};

