#include "thprac_replay_db.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_utils.h"
#include "thprac_utils.h"
#include "thprac_db_helper.h"
namespace THPrac {
bool g_replayDBOffline = nullptr;
sqlite3* g_replayDB = nullptr;

int CheckReplayTable()
{
    int rc;

    sqlite3_stmt* stmt;
    SQL_TEST_OK(sqlite3_prepare_v2(g_replayDB, "CREATE TABLE IF NOT EXISTS replay_table (hash integer PRIMARY KEY)", -1, &stmt, NULL), {});
    SQL_TEST_OK(EvaluateStmt(stmt), {});

    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "hash", "integer", true), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "game", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "filename", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "player", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "rank", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "shot_type", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "stage", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "score", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "date_replay", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "date_added", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "comment", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "record_hash", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "replay_data", "blob"), {});

    return SQLITE_OK;
}

int CheckRecordTable()
{
    int rc;

    sqlite3_stmt* stmt;
    SQL_TEST_OK(sqlite3_prepare_v2(g_replayDB, "CREATE TABLE IF NOT EXISTS record_table (hash integer PRIMARY KEY)", -1, &stmt, NULL), {});
    SQL_TEST_OK(EvaluateStmt(stmt), {});

    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "hash", "integer", true), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "game", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "player", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "rank", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "shot_type", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "replay_table", "stage", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "score", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "date", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "comment", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "replay_hash", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "record_table", "record_data", "blob"), {});

    return SQLITE_OK;
}

int CheckStatisticTable()
{
    int rc;

    sqlite3_stmt* stmt;
    SQL_TEST_OK(sqlite3_prepare_v2(g_replayDB, "CREATE TABLE IF NOT EXISTS statistic_table_wip (section_id integer PRIMARY KEY)", -1, &stmt, NULL), {});
    SQL_TEST_OK(EvaluateStmt(stmt), {});

    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "section_id", "integer", true), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "rank", "text"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "time", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "miss", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "bomb", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "completion", "integer"), {});
    SQL_TEST_OK(CheckColumn(g_replayDB, "statistic_table_wip", "score", "integer"), {});

    return SQLITE_OK;
}

int ReplayDBInit()
{
    int rc;
    char* errMsg = nullptr;

    if (!g_replayDB) {
        auto dbPath = LauncherGetDataDir();
        dbPath += L"\\thprac_relpays.db";
        SQL_TEST_OK(sqlite3_open_v2(utf16_to_utf8(dbPath).c_str(), &g_replayDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL), { g_replayDB = nullptr; });

        SQL_TEST_OK(CheckReplayTable(), {});
        SQL_TEST_OK(CheckRecordTable(), {});
        SQL_TEST_OK(CheckStatisticTable(), {});


    }
    return SQLITE_OK;
}
}