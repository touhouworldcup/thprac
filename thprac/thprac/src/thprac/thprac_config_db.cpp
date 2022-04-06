#include "thprac_config_db.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_utils.h"
#include "thprac_utils.h"
#include "thprac_launcher_games_def.h"
#include "thprac_db_helper.h"



namespace THPrac {
bool g_cfgDBOffline = nullptr;
sqlite3* g_cfgDB = nullptr;


int CheckCfgTable(const char* gameName)
{
    int rc;
    char sqlStmt[256];
    sqlite3_stmt* stmt;

    sprintf_s(sqlStmt, "CREATE TABLE IF NOT EXISTS %s (name text PRIMARY KEY, content text)", gameName);
    SQL_TEST_OK(sqlite3_prepare_v2(g_cfgDB, sqlStmt, -1, &stmt, NULL), {});
    SQL_TEST_OK(EvaluateStmt(stmt), {});

    SQL_TEST_OK(CheckColumn(g_cfgDB, gameName, "name", "text", true), {});
    SQL_TEST_OK(CheckColumn(g_cfgDB, gameName, "content", "text"), {});

    return SQLITE_OK;
}

int CheckAllGameCfgTable()
{
    int rc;

    for (auto& gameDef : gGameDefs) {
        if (gameDef.catagory == CAT_MAIN || gameDef.catagory == CAT_SPINOFF_STG) {
            SQL_TEST_OK(CheckCfgTable(gameDef.idStr), {});
        }
    }
    return SQLITE_OK;
}

int ConfigDBInit(bool noCreate)
{
    int rc;

    if (!g_cfgDB) {
        auto dbPath = LauncherGetDataDir();
        dbPath += L"\\thprac_configs.db";
        SQL_TEST_OK(sqlite3_open_v2(utf16_to_utf8(dbPath).c_str(), &g_cfgDB, SQLITE_OPEN_READWRITE | (noCreate ? 0 : SQLITE_OPEN_CREATE), NULL), 
            { g_cfgDB = nullptr; });
        SQL_TEST_OK(CheckCfgTable("thprac"), {});
    }
    return SQLITE_OK;
}

int GetStrFromDB(const char* table, const char* name, std::string& str)
{
    int rc;
    sqlite3_stmt* stmt;
    size_t columnCounter = 0;
    char sqlStmt[256];

    sprintf_s(sqlStmt, "SELECT name,content FROM %s WHERE name = '%s'", table, name);
    SQL_TEST_OK(sqlite3_prepare_v2(g_cfgDB, sqlStmt, -1, &stmt, NULL), {});
    SQL_TEST_OK(EvaluateStmt(
                    stmt, [](sqlite3_stmt* stmt, void* param) -> int {
                        std::string* pStr = (std::string*)param;
                        auto jsonStr = sqlite3_column_text(stmt, 1);
                        *pStr = (const char*)jsonStr;
                        return SQLITE_OK;
                    },
                    (void*)&str, &columnCounter),
        { return SQLITE_CORRUPT; });

    if (!columnCounter) {
        return SQLITE_CORRUPT;
    }
    return SQLITE_OK;
}

int SetStrToDB(const char* table, const char* name, std::string& str)
{
    int rc;
    sqlite3_stmt* stmt;
    size_t columnCounter = 0;
    char sqlStmt[256];

    sprintf_s(sqlStmt, "INSERT INTO %s (name,content) VALUES ('%s',?) ON CONFLICT(name) DO UPDATE SET content = ?", table, name);
    SQL_TEST_OK(sqlite3_prepare_v2(g_cfgDB, sqlStmt, -1, &stmt, NULL), {});
    sqlite3_bind_text(stmt, 1, str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, str.c_str(), -1, SQLITE_STATIC);
    SQL_TEST_OK(EvaluateStmt(stmt), {});

    return SQLITE_OK;
}


}