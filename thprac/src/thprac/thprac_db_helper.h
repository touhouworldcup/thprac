#pragma once
#define NOMINMAX
#include <Windows.h>
#include <sqlite3.h>
#include <string>

#define SQL_TEST_OK(expr, failed) \
    rc = expr;                    \
    if (rc != SQLITE_OK) {        \
        failed return rc;         \
    }

#define SQL_TEST_OK_NORET(expr, failed) \
    rc = expr;                          \
    if (rc != SQLITE_OK) {              \
        failed                          \
    }

namespace THPrac {

static int EvaluateStmt(sqlite3_stmt* stmt, int (*callback)(sqlite3_stmt*, void*) = nullptr, void* param = nullptr, size_t* counter = nullptr)
{
    for (auto stepRC = sqlite3_step(stmt); stepRC != SQLITE_DONE; stepRC = sqlite3_step(stmt)) {
        switch (stepRC) {
        case SQLITE_ROW: {
            (*counter)++;
            if (callback) {
                auto callbackRC = callback(stmt, param);
                if (callbackRC != SQLITE_OK) {
                    sqlite3_reset(stmt);
                    sqlite3_finalize(stmt);
                    return callbackRC;
                }
            }
        } break;
        case SQLITE_BUSY:
            Sleep(1);
            break;
        case SQLITE_OK:
            break;
        default:
            sqlite3_reset(stmt);
            sqlite3_finalize(stmt);
            return stepRC;
        }
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

static int CheckColumn(sqlite3* database, const char* dbName, const char* name, const char* type, bool pk = false)
{
    int rc;
    sqlite3_stmt* stmt;
    size_t columnCounter = 0;

    struct CheckColumnParam {
        const char* type;
        bool isPrimaryKey;
    } param { type, pk };

    SQL_TEST_OK(sqlite3_prepare_v2(database, "SELECT name,type,pk FROM pragma_table_info(?) WHERE name = ?", -1, &stmt, NULL), {});
    sqlite3_bind_text(stmt, 1, dbName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    SQL_TEST_OK(EvaluateStmt(
                    stmt, [](sqlite3_stmt* stmt, void* param) -> int {
                        CheckColumnParam* p = (CheckColumnParam*)param;
                        auto nameStr = sqlite3_column_text(stmt, 0);
                        auto typeStr = sqlite3_column_text(stmt, 1);
                        bool isPK = sqlite3_column_int(stmt, 2);
                        if (strcmp((const char*)typeStr, (const char*)p->type) || (p->isPrimaryKey != isPK)) {
                            return SQLITE_CORRUPT;
                        }
                        return SQLITE_OK;
                    },
                    (void*)&param, &columnCounter),
        { return SQLITE_CORRUPT; });

    if (!columnCounter) {
        if (pk) {
            return SQLITE_CORRUPT;
        }

        char sqlStmt[64];
        sprintf_s(sqlStmt, "ALTER TABLE %s ADD %s %s", dbName, name, type);
        SQL_TEST_OK(sqlite3_prepare_v2(database, sqlStmt, -1, &stmt, NULL), {});
        SQL_TEST_OK(EvaluateStmt(stmt), {});
    }

    return SQLITE_OK;
}
}