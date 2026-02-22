#include <Windows.h>

#include "thprac_identify.h"
#include "thprac_utils.h"
#include "utils/utils.h"

#include <metrohash128.h>

namespace THPrac {

extern void TH06Init();
extern void TH07Init();
extern void TH08Init();
extern void TH09Init();
extern void TH095Init();
extern void TH10Init();
extern void AlcostgInit();
extern void TH11Init();
extern void TH12Init();
extern void TH125Init();
extern void TH128Init();
extern void TH13Init();
extern void TH14Init();
extern void TH143Init();
extern void TH15Init();
extern void TH16Init();
extern void TH165Init();
extern void TH17Init();
extern void TH18Init();
extern void TH185Init();
extern void TH19_v1_00a_Init();
extern void TH19_v1_10c_Init();
extern void TH20Init();

const THGameVersion alcostg = {
    .gameId = ID_ALCOSTG,
    .initFunc = AlcostgInit,
    .timeStamp = 1198873702,
    .textSize = 397312,
    .oepCode = { 0x212b, 0x322a, 0x0524, 0xac44, 0x5691, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th06 = {
    .gameId = ID_TH06,
    .initFunc = TH06Init,
    .timeStamp = 1038721275,
    .textSize = 430080,
    .oepCode = { 0x212b, 0xe22a, 0x05bc, 0xac44, 0x4867, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th07 = {
    .gameId = ID_TH07,
    .initFunc = TH07Init,
    .timeStamp = 1066942337,
    .textSize = 572928,
    .oepCode = { 0x212b, 0xe22a, 0x0a45, 0xac44, 0x5ede, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th075 = {
    .gameId = ID_TH075,
    .initFunc = nullptr,
    .timeStamp = 1120788294,
    .textSize = 2449408,
    .oepCode = { 0x212b, 0x3a2a, 0x254c, 0xac44, 0x7599, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th08 = {
    .gameId = ID_TH08,
    .initFunc = TH08Init,
    .timeStamp = 1095610188,
    .textSize = 728064,
    .oepCode = { 0x212b, 0xf22a, 0x089b, 0xac44, 0x498b, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th09 = {
    .gameId = ID_TH09,
    .initFunc = TH09Init,
    .timeStamp = 1128708539,
    .textSize = 576512,
    .oepCode = { 0x212b, 0xea2a, 0x0a16, 0xac44, 0x5900, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th095 = {
    .gameId = ID_TH095,
    .initFunc = TH095Init,
    .timeStamp = 1137085759,
    .textSize = 603136,
    .oepCode = { 0x212b, 0x122a, 0x0af8, 0xac44, 0x5ce2, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th10 = {
    .gameId = ID_TH10,
    .initFunc = TH10Init,
    .timeStamp = 1186086096,
    .textSize = 412672,
    .oepCode = { 0x212b, 0x322a, 0x05e4, 0xac44, 0x5691, 0x4646, 0xd3f8, 0x4848, 0xc249, 0xa28d }
};
const THGameVersion th105 = {
    .gameId = ID_TH105,
    .initFunc = nullptr,
    .timeStamp = 1319458676,
    .textSize = 2879488,
    .oepCode = { 0x8ba9, 0x42dd, 0xaa43, 0xba52, 0xbaba, 0x8a8a, 0x8b8b, 0x0ac5, 0x12b6, 0xc789 }
};
const THGameVersion th11 = {
    .gameId = ID_TH11,
    .initFunc = TH11Init,
    .timeStamp = 1217734394,
    .textSize = 565248,
    .oepCode = { 0x06a9, 0x4289, 0xaa43, 0xba3c, 0xbaba, 0xb9cd, 0xcc12, 0xc9a4, 0x61a5, 0x4a49 }
};
const THGameVersion th12 = {
    .gameId = ID_TH12,
    .initFunc = TH12Init,
    .timeStamp = 1251344348,
    .textSize = 617472,
    .oepCode = { 0x06a9, 0x4289, 0xaa43, 0xba3c, 0xbaba, 0xb9cd, 0xcc12, 0xc9a4, 0x61a5, 0x4a49 }
};
const THGameVersion th123 = {
    .gameId = ID_TH123,
    .initFunc = nullptr,
    .timeStamp = 1316148875,
    .textSize = 4546560,
    .oepCode = { 0xa8a9, 0x42ec, 0xaa43, 0xba52, 0xbaba, 0x8a8a, 0x8b8b, 0x0ac5, 0x12b6, 0xc789 }
};
const THGameVersion th125 = {
    .gameId = ID_TH125,
    .initFunc = TH125Init,
    .timeStamp = 1267822137,
    .textSize = 611328,
    .oepCode = { 0xf8a9, 0x42f9, 0xaa43, 0xba3c, 0xbaba, 0xb9cd, 0xcc12, 0xc9a4, 0x61a5, 0x4a49 }
};
const THGameVersion th128 = {
    .gameId = ID_TH128,
    .initFunc = TH128Init,
    .timeStamp = 1280811414,
    .textSize = 626176,
    .oepCode = { 0x03a9, 0x42dd, 0xaa43, 0xbacd, 0xbaba, 0xb9cd, 0xcc12, 0xcba4, 0x69a5, 0x0fc1 }
};
const THGameVersion th13 = {
    .gameId = ID_TH13,
    .initFunc = TH13Init,
    .timeStamp = 1313589413,
    .textSize = 656384,
    .oepCode = { 0xb6a9, 0x42dc, 0xaa43, 0xbacd, 0xbaba, 0xb9cd, 0xcc12, 0xcba4, 0x69a5, 0x0fc1 }
};
const THGameVersion th135 = {
    .gameId = ID_TH135,
    .initFunc = nullptr,
    .timeStamp = 1381475275,
    .textSize = 3928064,
    .oepCode = { 0xd7a9, 0x4340, 0xaa43, 0xbacd, 0xbaba, 0xb9cd, 0xcc12, 0xc3a4, 0x410c, 0x16e9 }
};
const THGameVersion th14 = {
    .gameId = ID_TH14,
    .initFunc = TH14Init,
    .timeStamp = 1376634201,
    .textSize = 720896,
    .oepCode = { 0xafa9, 0x42f5, 0xaa43, 0x4444, 0x4545, 0x522c, 0x1f2f, 0x0486, 0xa149, 0x511d }
};
const THGameVersion th143 = {
    .gameId = ID_TH143,
    .initFunc = TH143Init,
    .timeStamp = 1398039605,
    .textSize = 748032,
    .oepCode = { 0x6aa9, 0x428b, 0xaa43, 0x4444, 0x4545, 0x522c, 0xdf2f, 0x0511, 0xa149, 0x56f7 }
};
const THGameVersion th145 = {
    .gameId = ID_TH145,
    .initFunc = nullptr,
    .timeStamp = 1492674616,
    .textSize = 3516928,
    .oepCode = { 0x83a9, 0x424d, 0xaa43, 0xbaca, 0xbaba, 0x562c, 0x772f, 0xcb28, 0xa149, 0x4500 }
};
const THGameVersion th15 = {
    .gameId = ID_TH15,
    .initFunc = TH15Init,
    .timeStamp = 1444028071,
    .textSize = 772608,
    .oepCode = { 0x1aa9, 0x428b, 0xaa43, 0x4444, 0x4545, 0x522c, 0x872f, 0x05f1, 0xa149, 0x56f7 }
};
const THGameVersion th155 = {
    .gameId = ID_TH155,
    .initFunc = nullptr,
    .timeStamp = 1524477382,
    .textSize = 3695616,
    .oepCode = { 0x37a9, 0x424c, 0xaa43, 0xbaca, 0xbaba, 0x562c, 0xb72f, 0xc163, 0xa149, 0x47b4 }
};
const THGameVersion th16 = {
    .gameId = ID_TH16,
    .initFunc = TH16Init,
    .timeStamp = 1501534029,
    .textSize = 564736,
    .oepCode = { 0xeaa9, 0x4246, 0xaa43, 0xbaca, 0xbaba, 0xcd13, 0x11ab, 0x3db7, 0xc241, 0xa2bb }
};
const THGameVersion th165 = {
    .gameId = ID_TH165,
    .initFunc = TH165Init,
    .timeStamp = 1532998383,
    .textSize = 607232,
    .oepCode = { 0xb9a9, 0x4246, 0xaa43, 0xbac0, 0xbaba, 0xcd13, 0x11ab, 0x3db7, 0xc241, 0xa2bb }
};
const THGameVersion th17 = {
    .gameId = ID_TH17,
    .initFunc = TH17Init,
    .timeStamp = 1565429804,
    .textSize = 626688,
    .oepCode = { 0xb2a9, 0x4246, 0xaa43, 0xba3e, 0xbaba, 0xcd13, 0x11ab, 0x3db7, 0xc241, 0xa2bb }
};
const THGameVersion th175 = {
    .gameId = ID_TH175,
    .initFunc = nullptr,
    .timeStamp = 1635319660,
    .textSize = 375296,
    .oepCode = { 0x31a9, 0x4244, 0xaa43, 0xbaca, 0xbaba, 0xcd13, 0x11ab, 0x3db7, 0xc241, 0xa2bb }
};
const THGameVersion th18 = {
    .gameId = ID_TH18,
    .initFunc = TH18Init,
    .timeStamp = 1618619144,
    .textSize = 702464,
    .oepCode = { 0x9ba9, 0x4246, 0xaa43, 0xba3e, 0xbaba, 0xcd13, 0xc4ab, 0x44a4, 0x04c4, 0xa2be }
};
const THGameVersion th185 = {
    .gameId = ID_TH185,
    .initFunc = TH185Init,
    .timeStamp = 1659144319,
    .textSize = 711168,
    .oepCode = { 0x84a9, 0x4246, 0xaa43, 0xba3e, 0xbaba, 0xcd13, 0xc4ab, 0x44a4, 0x04c4, 0xa2be }
};
const THGameVersion th19_v1_00a = {
    .gameId = ID_TH19,
    .initFunc = TH19_v1_00a_Init,
    .timeStamp = 1690598468,
    .textSize = 1433600,
    .oepCode = { 0xdfa9, 0x4247, 0xaa43, 0xba3e, 0xbaba, 0x0e2e, 0x1d90, 0xb748, 0x8d5c, 0x1fbb }
};
const THGameVersion th19_v1_10c = {
    .gameId = ID_TH19,
    .initFunc = TH19_v1_10c_Init,
    .timeStamp = 1720429610,
    .textSize = 1544704,
    .oepCode = { 0xb3a9, 0x4245, 0xaa43, 0xba3e, 0xbaba, 0x0e2e, 0x1a4a, 0xb748, 0xad5c, 0x1dfb }
};
const THGameVersion th20 = {
    .gameId = ID_TH20,
    .initFunc = TH20Init,
    .timeStamp = 1753839620,
    .textSize = 1484288,
    .oepCode = { 0xaca9, 0x4244, 0xaa43, 0xba33, 0xbaba, 0x4684, 0xcc47, 0xbc05, 0xc02d, 0x4a47 }
};

const THGameVersion gGameVersions[] = {
    alcostg,
    th06,
    th07,
    th075,
    th08,
    th09,
    th095,
    th10,
    th105,
    th11,
    th12,
    th123,
    th125,
    th128,
    th13,
    th135,
    th14,
    th143,
    th145,
    th15,
    th155,
    th16,
    th165,
    th17,
    th175,
    th18,
    th185,
    th19_v1_00a,
    th19_v1_10c,
    th20,
};
const unsigned int gGameVersionsCount = elementsof(gGameVersions);

const THKnownGame gKnownGames[] = {
    { &alcostg,
        TYPE_ORIGINAL,
        { 0x3d65fad0, 0xd4c4e831,
            0x4fb58774, 0x957f0157 } },
    { &th06,
        TYPE_ORIGINAL,
        { 0xbae18847, 0x78d78ce2,
            0x4d19703a, 0x3f5cbe16 } },
    { &th06,
        TYPE_CHINESE,
        { 0x6c2e0eb5, 0x6bb9bce8,
            0x4ffcf20e, 0x65e9c0bf } },
    { &th06,
        TYPE_ORIGINAL,
        { 0xd179f6c8, 0x0af9170,
            0xae99b942, 0x4c974b53 } },
    { &th07,
        TYPE_ORIGINAL,
        { 0xd69a5df6, 0x4a78f383,
            0x956d7f9f, 0x02e3ad3b } },
    { &th07,
        TYPE_SCHINESE,
        { 0x8995e319, 0x17c3ee1c,
            0x6034a965, 0x70fefd77 } },
    { &th07,
        TYPE_SCHINESE,
        { 0xdc5ae3c0, 0x959663c2,
            0x5cebd278, 0x956424fe } },
    { &th075,
        TYPE_ORIGINAL,
        { 0xececd7e6, 0xc7e6b23e,
            0xd87c04f6, 0x98c496dc } },
    { &th075,
        TYPE_SCHINESE,
        { 0xbdcd7982, 0x41103975,
            0x75dc169d, 0x88298932 } },
    { &th08,
        TYPE_ORIGINAL,
        { 0xc1f84aea, 0xfad04d3b,
            0xf28642e5, 0x7fe28f3b } },
    { &th08,
        TYPE_SCHINESE,
        { 0x58db89b4, 0x5a9fe98a,
            0x2dea944f, 0xaebe92d9 } },
    { &th09,
        TYPE_ORIGINAL,
        { 0xdc63c169, 0x2c975430,
            0xbc21df80, 0x26937f9e } },
    { &th09,
        TYPE_SCHINESE,
        { 0x479728a5, 0xefc341e0,
            0x260d5e91, 0xe6851479 } },
    { &th095,
        TYPE_ORIGINAL,
        { 0x46eaf90e, 0x2a24f234,
            0x74b70f03, 0xbd59c6e5 } },
    { &th095,
        TYPE_STEAM,
        { 0x9e7883da, 0x3e8a7a62,
            0x333fa441, 0x9e4a5077 } },
    { &th095,
        TYPE_SCHINESE,
        { 0xc0ba2aae, 0xd037ee77,
            0xda79797d, 0x53a7ebc1 } },
    { &th10,
        TYPE_ORIGINAL,
        { 0x11c73117, 0x422a725f,
            0xc0639015, 0x06050767 } },
    { &th10,
        TYPE_STEAM,
        { 0x4771b7b2, 0x61a9b6ff,
            0x77b2d73c, 0xb05af556 } },
    { &th10,
        TYPE_SCHINESE,
        { 0x2680e8df, 0xe528eef4,
            0x2a07c721, 0x6bc772df } },
    { &th10,
        TYPE_TCHINESE,
        { 0x03a4114f, 0xa6bfb0d7,
            0xcbc29fec, 0x7028a7b1 } },
    { &th105,
        TYPE_ORIGINAL,
        { 0x31aaf075, 0x7fe79569,
            0x5e0ad6ea, 0x8105772e } },
    { &th105,
        TYPE_SCHINESE,
        { 0x6a09d7da, 0xe25c8d1b,
            0xdfa00b4f, 0x1fe8bb5f } },
    { &th11,
        TYPE_ORIGINAL,
        { 0x4e4ad931, 0xa91dc711,
            0x720e5db7, 0x57a14232 } },
    { &th11,
        TYPE_STEAM,
        { 0xa6c81bda, 0xab6fea4b,
            0xb9708f50, 0x31242cdd } },
    { &th11,
        TYPE_SCHINESE,
        { 0xca516350, 0x63057ef3,
            0x815a096e, 0x50afd6b8 } },
    { &th12,
        TYPE_ORIGINAL,
        { 0xd4d81259, 0xce424514,
            0x6e7e6326, 0x8b4c0990 } },
    { &th12,
        TYPE_STEAM,
        { 0x36865d01, 0x4402f03e,
            0x8d44dabd, 0x65281618 } },
    { &th12,
        TYPE_SCHINESE,
        { 0xddf5470b, 0xe89d1019,
            0x4fb6bf56, 0x738153ad } },
    { &th123,
        TYPE_ORIGINAL,
        { 0x7be66ad2, 0x4a21e375,
            0xc7d08b4f, 0x7cc3c681 } },
    { &th123,
        TYPE_SCHINESE,
        { 0x2a7099d1, 0xf0244e40,
            0x8fcab54c, 0xbdba4a5d } },
    { &th125,
        TYPE_ORIGINAL,
        { 0x0e1b7b17, 0x473f857e,
            0x3fe6360f, 0x40de70d3 } },
    { &th125,
        TYPE_STEAM,
        { 0x88d8f1d8, 0x4820a04b,
            0x4414c45d, 0xcfaa2117 } },
    { &th125,
        TYPE_SCHINESE,
        { 0x66928829, 0x80da58eb,
            0xd9dfd4da, 0x8333fcce } },
    { &th128,
        TYPE_ORIGINAL,
        { 0x948a4e8a, 0x665d6cd0,
            0x25fc26f7, 0xcba8f1e3 } },
    { &th128,
        TYPE_STEAM,
        { 0x48b1d757, 0x29766b53,
            0xc07d3645, 0x7d535f20 } },
    { &th128,
        TYPE_SCHINESE,
        { 0x126829c6, 0xd6eab530,
            0xf9734a83, 0xee657af5 } },
    { &th13,
        TYPE_ORIGINAL,
        { 0x4619502d, 0xe94742fc,
            0x13537c7d, 0x7212f384 } },
    { &th13,
        TYPE_STEAM,
        { 0xd60fa763, 0x380673c1,
            0x3a3f6475, 0x11dc1f00 } },
    { &th13,
        TYPE_ORIGINAL,
        { 0xb74eefbb, 0x7a38aa6f,
            0xfa5258ff, 0x81b1c16c } },
    { &th13,
        TYPE_SCHINESE,
        { 0xbfaa3ea8, 0x1242e9dd,
            0x82953fb0, 0x40d24284 } },
    { &th13,
        TYPE_SCHINESE,
        { 0x7c12239d, 0xf9e1b1a5,
            0x2388ce13, 0x7e458f48 } },
    { &th135,
        TYPE_SCHINESE,
        { 0xf2246b98, 0x9d4a7bf0,
            0x2c1d8f07, 0x84bc1a99 } },
    { &th14,
        TYPE_ORIGINAL,
        { 0x08a6fde4, 0xb4344a4a,
            0xa3636647, 0x387d4253 } },
    { &th14,
        TYPE_STEAM,
        { 0x5edd7a93, 0x774d84b,
            0x5df1b298, 0x210f5fc8 } },
    { &th14,
        TYPE_NYASAMA,
        { 0xdc9235f5, 0xe124c565,
            0x6485e269, 0xf4686950 } },
    { &th143,
        TYPE_ORIGINAL,
        { 0x2549f2c0, 0x1121c489,
            0x9aaf6330, 0x75ade252 } },
    { &th143,
        TYPE_STEAM,
        { 0x227e06a2, 0x722509d9,
            0x01718a61, 0xad905c25 } },
    { &th143,
        TYPE_NYASAMA,
        { 0x47cdb883, 0xbe3921ae,
            0xf9c0c825, 0x2cefb060 } },
    { &th145,
        TYPE_ORIGINAL,
        { 0x416e1a70, 0xcdae8d4f,
            0xe62060d0, 0xc592d164 } },
    { &th145,
        TYPE_SCHINESE,
        { 0x8c6911ea, 0xd4ab7879,
            0x82297970, 0xbfd67515 } },
    { &th15,
        TYPE_ORIGINAL,
        { 0xed3ac6ec, 0x21bc473c,
            0x186edbb4, 0x9ebd98cf } },
    { &th15,
        TYPE_STEAM,
        { 0x75871bd4, 0x0adeb360,
            0xf429659d, 0x931922e2 } },
    { &th15,
        TYPE_NYASAMA,
        { 0xcfe2e487, 0xc7e04af4,
            0x9160e8c1, 0x98291df2 } },
    { &th155,
        TYPE_ORIGINAL,
        { 0xa42ea27d, 0xf14ae534,
            0xde6c96e0, 0xb78ce61b } },
    { &th16,
        TYPE_ORIGINAL,
        { 0x4dae29e4, 0xa1af84f8,
            0xc6d1d694, 0x8cb29894 } },
    { &th16,
        TYPE_STEAM,
        { 0x60ebdd82, 0xc012c21a,
            0x9d875c04, 0x6c1f7fef } },
    { &th16,
        TYPE_NYASAMA,
        { 0xbf82bc22, 0xe19074c3,
            0x4fb9a1d7, 0xfa429c31 } },
    { &th165,
        TYPE_ORIGINAL,
        { 0x550d4b1c, 0x805408e3,
            0x1ef2ca25, 0xa257adb3 } },
    { &th165,
        TYPE_STEAM,
        { 0x886148be, 0x11ed264e,
            0x369d5e4b, 0x16045490 } },
    { &th165,
        TYPE_NYASAMA,
        { 0xfcdcbd55, 0x8b944a23,
            0x32f3f96d, 0xefb7750e } },
    { &th17,
        TYPE_ORIGINAL,
        { 0x3b46eeb2, 0xcd3b5634,
            0x3e1032a2, 0x22cac0c5 } },
    { &th17,
        TYPE_STEAM,
        { 0x9fb6f999, 0xcc31ff48,
            0x57271b1f, 0x82db4030 } },
    { &th17,
        TYPE_NYASAMA,
        { 0x2f370234, 0xff6024b0,
            0x80f25da9, 0xcf2e1bf4 } },
    { &th175,
        TYPE_ORIGINAL,
        { 0xac9cb8f9, 0x1972b2cc,
            0xadab5748, 0x751593e1 } },
    { &th18,
        TYPE_ORIGINAL,
        { 0xa3f0a451, 0x85ce8668,
            0xfa4eda8a, 0xd808c596 } },
    { &th18,
        TYPE_NYASAMA,
        { 0x71115032, 0x3a4d05bd,
            0x8dfc8ba4, 0xa476dc6f } },
    { &th18,
        TYPE_STEAM,
        { 0x7b13a1f3, 0x3eb0e397,
            0xa6f5b6ec, 0xcbf302ce } },
    { &th185,
        TYPE_ORIGINAL,
        { 0x79cfcdbf, 0xfaa400a6,
            0x5f92bcd6, 0x65a0d95 } },
    { &th185,
        TYPE_STEAM,
        { 0xcd1d1ad0, 0x34cf8cc1,
            0x32af8b3a, 0xcb07b8dc } },
    { &th19_v1_00a,
        TYPE_ORIGINAL,
        { 0x1ef2d050, 0xbddd8da2,
            0x56cedb87, 0xe674cd2c } },
    { &th19_v1_00a,
        TYPE_STEAM,
        { 0xfeac2cc1, 0xff97767b,
            0x7760eca9, 0xa7f85003 } },
    { &th19_v1_10c,
        TYPE_ORIGINAL,
        { 0xc2d1ef12, 0x6aa14d56,
            0x172284a9, 0x1d4147d8 } },
    { &th19_v1_10c,
        TYPE_STEAM,
        { 0x4bb3eeda, 0xdb1651f5,
            0xf670bb28, 0x8aea075e } },
    { &th20,
        TYPE_ORIGINAL,
        { 0x6cc723ea, 0xa71db65e,
            0x3c19f37c, 0x836fb369 } },
    { &th20,
        TYPE_STEAM,
        { 0xc0744bc1, 0x3c2aea32,
            0x9241a33e, 0xc467ae44 } },
};
const unsigned int gKnownGamesCount = elementsof(gKnownGames);

const char* gThGameStrs[] = {
    "unknown",
    "alcostg",
    "th06",
    "th07",
    "th075",
    "th08",
    "th09",
    "th095",
    "th10",
    "th105",
    "th11",
    "th12",
    "th123",
    "th125",
    "th128",
    "th13",
    "th135",
    "th14",
    "th143",
    "th145",
    "th15",
    "th155",
    "th16",
    "th165",
    "th17",
    "th175",
    "th18",
    "th185",
    "th19",
    "th20",
};

uint64_t GetExeInfo(uint8_t* mod) {
    uint32_t out_text_size = 0;
    uint32_t out_timestamp = 0;

    auto* dosHeader = (IMAGE_DOS_HEADER*)mod;
    auto* ntHeader = (IMAGE_NT_HEADERS*)(mod + dosHeader->e_lfanew);
    auto* section = (IMAGE_SECTION_HEADER*)((uintptr_t)&ntHeader->OptionalHeader + ntHeader->FileHeader.SizeOfOptionalHeader);

    out_timestamp = ntHeader->FileHeader.TimeDateStamp;

    for (unsigned i = 0; i < ntHeader->FileHeader.NumberOfSections; i++) {
        if (!_stricmp(".text", (char*)section[i].Name)) {
            out_text_size = section[i].SizeOfRawData;
            break;
        }
    }

    return (uint64_t)out_text_size << 32 | out_timestamp;
}

uint64_t GetRemoteExeInfo(HANDLE hProc, uintptr_t mod) {
    uint32_t out_text_size = 0;
    uint32_t out_timestamp = 0;

    DWORD byteRet;

    uintptr_t addr_ntHeader;
    ReadProcessMemory(
        hProc,
        (void*)(mod + offsetof(IMAGE_DOS_HEADER, e_lfanew)),
        &addr_ntHeader, sizeof(addr_ntHeader),
        &byteRet
    );
    addr_ntHeader += mod;

    uintptr_t addr_optionalHeader = addr_ntHeader + offsetof(IMAGE_NT_HEADERS, OptionalHeader);
    uintptr_t addr_fileHeader = addr_ntHeader + offsetof(IMAGE_NT_HEADERS, FileHeader);

    ReadProcessMemory(
        hProc,
        (void*)(addr_fileHeader + offsetof(IMAGE_FILE_HEADER, TimeDateStamp)),
        &out_timestamp, sizeof(out_timestamp),
        &byteRet
    );

    WORD sizeOfOptionalHeader;
    ReadProcessMemory(
        hProc,
        (void*)(addr_fileHeader + offsetof(IMAGE_FILE_HEADER, SizeOfOptionalHeader)),
        &sizeOfOptionalHeader, sizeof(sizeOfOptionalHeader),
        &byteRet
    );

    WORD numberOfSections;
    ReadProcessMemory(
        hProc,
        (void*)(addr_fileHeader + offsetof(IMAGE_FILE_HEADER, NumberOfSections)),
        &numberOfSections, sizeof(numberOfSections),
        &byteRet
    );

    for (size_t i = 0; i < numberOfSections; i++) {
        IMAGE_SECTION_HEADER section;
        ReadProcessMemory(
            hProc,
            (void*)(addr_optionalHeader + sizeOfOptionalHeader + i * sizeof(IMAGE_SECTION_HEADER)),
            &section, sizeof(section),
            &byteRet
        );

        if (!_stricmp((char*)section.Name, ".text")) {
            out_text_size = section.SizeOfRawData;
            break;
        }
    }

    return (uint64_t)out_text_size << 32 | out_timestamp;
}

const THGameVersion* IdentifyExe(uint8_t* buf) {
    auto exe_info = GetExeInfo(buf);

    for (const auto& i : gGameVersions) {
        auto info_packed = (uint64_t)i.textSize << 32 | i.timeStamp;
        if (info_packed == exe_info) {
            return &i;
        }
    }
    return nullptr;
}

const THGameVersion* IdentifyExe(const wchar_t* path) {
    MappedFile f(path);
    if (!f.fileMapView) {
        return nullptr;
    }
    return IdentifyExe((uint8_t*)f.fileMapView);
}

}