#pragma once
#include <string>
#include <stdint.h>

std::string RSAVerficate(void* keyData, size_t keySize, void* data, size_t size, std::string& output);
