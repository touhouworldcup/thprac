#pragma once
#include <string>

namespace THPrac {
int SSLTestInit(void* data);

bool SSLGenKey(std::string& output);
std::string SSLGetPEM(std::string& key);
std::string SSLLoadPEM(std::string& key);
void SSLGetInfoFromKey(std::string& privateKey, std::string& publicKey, std::string& fingerprint);
std::string SSLDecrypt(std::string& key, void* data, size_t size);
std::string SSLGetFingerprint(std::string& key);
}