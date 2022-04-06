#pragma once
#include <sqlite3.h>
#include <string>

namespace THPrac {
int ConfigDBInit(bool noCreate = false);
int CheckCfgTable(const char* gameName);
int CheckAllGameCfgTable();
int GetStrFromDB(const char* table, const char* name, std::string& str);
int SetStrToDB(const char* table, const char* name, std::string& str);
}