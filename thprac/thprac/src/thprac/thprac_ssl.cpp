#define WC_RSA_BLINDING
#define NO_MULTIBYTE_PRINT
#define WOLFSSL_KEY_GEN

#include "thprac_ssl.h"
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/asn.h>

namespace THPrac {

WC_RNG* g_marketeerRng = nullptr;
void RsaRngInit()
{
    if (!g_marketeerRng) {
        g_marketeerRng = new WC_RNG();
        wc_InitRng(g_marketeerRng);
    }
}

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
        if (!in_len || in_len % 4 != 0)
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

int SSLTestInit(void* data)
{
    int ret = 0;
    RsaKey genKey;
    WC_RNG rng;

    if (wc_InitRng(&rng))
        return false;
    wc_InitRsaKey(&genKey, 0);
    ret = wc_MakeRsaKey(&genKey, 4096, 65537, &rng);
    ret = wc_RsaKeyToPublicDer(&genKey, (byte*)data, 1024 * 16);
    wc_FreeRsaKey(&genKey);
    wc_FreeRng(&rng);

    return ret;
}


bool SSLGenKey(std::string& output)
{
    bool result = false;
    byte* derBuffer = (byte*)malloc(4096);
    if (!derBuffer)
        return false;
    int ret = 0;
    RsaKey genKey;
    WC_RNG rng;

    if (wc_InitRng(&rng))
        goto end;
    if (wc_InitRsaKey(&genKey, 0))
        goto end1;
    if (wc_MakeRsaKey(&genKey, 4096, 65537, &rng))
        goto end2;

    ret = wc_RsaKeyToDer(&genKey, (byte*)derBuffer, 4096);
    if (ret) {
        output.resize(ret);
        memcpy(output.data(), derBuffer, ret);
    }

end2:
    wc_FreeRsaKey(&genKey);
end1:
    wc_FreeRng(&rng);
end:
    free(derBuffer);
    return ret;
}
std::string SSLGetPEM(std::string& key)
{
    return Base64::Encode(key.data(), key.size());
}
std::string SSLLoadPEM(std::string& key)
{
    std::string output;
    if (!Base64::Decode(key, output)) {
        return "";
    }
    return output;
}
std::string SSLDecrypt(std::string& key, void* data, size_t size)
{
    RsaRngInit();
    std::string output;

    RsaKey decodedKey;
    word32 idx = 0;
    wc_InitRsaKey(&decodedKey, 0);
    wc_RsaPrivateKeyDecode((byte*)key.data(), &idx, &decodedKey, key.size());
    wc_RsaSetRNG(&decodedKey, g_marketeerRng);

    byte* buffer = (byte*)malloc(4096);
    auto decSize = wc_RsaPrivateDecrypt((byte*)data, size, buffer, 4096, &decodedKey);
    if (decSize > 0 && decSize < 128) {
        output.resize(decSize);
        memcpy(output.data(), buffer, decSize);
    } else {
        output = "";
    }

    free(buffer);
    wc_FreeRsaKey(&decodedKey);
    return output;
}
void SSLGetInfoFromKey(std::string& privateKey, std::string& publicKey, std::string& fingerprint)
{
    RsaKey decodedKey;
    word32 idx = 0;
    wc_InitRsaKey(&decodedKey, 0);
    wc_RsaPrivateKeyDecode((byte*)privateKey.data(), &idx, &decodedKey, privateKey.size());

    byte* derBuffer = (byte*)malloc(4096);
    auto derSize = wc_RsaKeyToPublicDer(&decodedKey, (byte*)derBuffer, 4096);
    publicKey.resize(derSize);
    memcpy(publicKey.data(), derBuffer, derSize);

    uint16_t fpData[9] {};
    wc_Md5Hash(derBuffer, derSize, (byte*)&(fpData[1]));
    fpData[0] = crc16((uint8_t*)derBuffer, derSize);

    free(derBuffer);
    wc_FreeRsaKey(&decodedKey);
    fingerprint = Base64::Encode((char*)fpData, 18);
}
std::string SSLGetFingerprint(std::string& key)
{
    RsaKey decodedKey;
    word32 idx = 0;
    wc_InitRsaKey(&decodedKey, 0);
    wc_RsaPrivateKeyDecode((byte*)key.data(), &idx, &decodedKey, key.size());

    byte* derBuffer = (byte*)malloc(4096);
    auto derSize = wc_RsaKeyToPublicDer(&decodedKey, (byte*)derBuffer, 4096);

    uint16_t fpData[9] {};
    wc_Md5Hash(derBuffer, derSize, (byte*)&(fpData[1]));
    fpData[0] = crc16((uint8_t*)derBuffer, derSize);

    free(derBuffer);
    wc_FreeRsaKey(&decodedKey);
    return Base64::Encode((char*)fpData, 18);
}

}