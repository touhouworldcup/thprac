#define WC_RSA_BLINDING
#define NO_MULTIBYTE_PRINT
#define WOLFSSL_KEY_GEN

#include "LRMP_SSL.h"
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/asn.h>


uint16_t crc16(uint8_t const* data, size_t size)
{
    uint16_t crc = 0;
    while (size--) {
        crc ^= *data++;
        for (unsigned k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    }
    return crc;
}
class Base64 {
public:
    static std::string Encode(const std::string& data)
    {
        return Encode(data.c_str(), data.size());
    }
    static std::string Encode(const char* data, size_t size)
    {
        static constexpr char sEncodingTable[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'
        };

        size_t in_len = size;
        size_t out_len = 4 * ((in_len + 2) / 3);
        std::string ret(out_len, '\0');
        size_t i;
        char* p = const_cast<char*>(ret.c_str());

        for (i = 0; i < in_len - 2; i += 3) {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
            *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
            *p++ = sEncodingTable[data[i + 2] & 0x3F];
        }
        if (i < in_len) {
            *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
            if (i == (in_len - 1)) {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                *p++ = '=';
            } else {
                *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
                *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
            }
            *p++ = '=';
        }

        return ret;
    }

    static bool Decode(const std::string& input, std::string& out)
    {
        static constexpr unsigned char kDecodingTable[] = {
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
            64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
            64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
        };

        size_t in_len = input.size();
        if (in_len % 4 != 0)
            return false;

        size_t out_len = in_len / 4 * 3;
        if (input[in_len - 1] == '=')
            out_len--;
        if (input[in_len - 2] == '=')
            out_len--;

        out.resize(out_len);

        for (size_t i = 0, j = 0; i < in_len;) {
            uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
            uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

            uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

            if (j < out_len)
                out[j++] = (triple >> 2 * 8) & 0xFF;
            if (j < out_len)
                out[j++] = (triple >> 1 * 8) & 0xFF;
            if (j < out_len)
                out[j++] = (triple >> 0 * 8) & 0xFF;
        }

        return true;
    }
};
WC_RNG* RngInit()
{
    static WC_RNG rng;
    static bool rngStatus = false;
    if (!rngStatus) {
        rngStatus = true;
        wc_InitRng(&rng);
    }
    return &rng;
}
char dummyKey[8192] {};
std::string RSAVerficate(void* keyData, size_t keySize, void* data, size_t size, std::string& output)
{
    static byte* tmpBuffer = nullptr;
    RngInit();

    RsaKey& decodedKey = *(RsaKey*)dummyKey;
    word32 idx = 0;
    wc_InitRsaKey((RsaKey*)dummyKey, 0);
    if (wc_RsaPublicKeyDecode((byte*)keyData, &idx, (RsaKey*)dummyKey, (word32)keySize) != 0) {
        return "";
    }

    if (!tmpBuffer) {
        tmpBuffer = (byte*)malloc(4096);
    }
    int encSize = wc_RsaPublicEncrypt((byte*)data, (word32)size, tmpBuffer, 4096, (RsaKey*)dummyKey, RngInit());
    if (encSize <= 0) {
        return "";
    }
    output.resize(encSize);
    memcpy(output.data(), tmpBuffer, encSize);

    uint16_t fpData[9] {};
    wc_Md5Hash((byte*)keyData, (word32)keySize, (byte*)&(fpData[1]));
    fpData[0] = crc16((uint8_t*)keyData, keySize);
    return Base64::Encode((char*)fpData, 18);
}
