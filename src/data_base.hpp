// #pragma once

// class DatabaseManager {
// public:
//     DatabaseManager(const std::string& db_path) {
//         sqlite3_open(db_path.c_str(), &db_);
//     }

//     ~DatabaseManager() {
//         sqlite3_close(db_);
//     }

//     // Пример функции для вставки сообщений
//     void save_message(const std::string& channel, const std::string& user, const std::string& message) {
//         std::string sql = "INSERT INTO messages (channel, user, message) VALUES (?, ?, ?);";
//         sqlite3_stmt *stmt;
//         sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
//         sqlite3_bind_text(stmt, 1, channel.c_str(), -1, SQLITE_STATIC);
//         sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_STATIC);
//         sqlite3_bind_text(stmt, 3, message.c_str(), -1, SQLITE_STATIC);

//         sqlite3_step(stmt);
//         sqlite3_finalize(stmt);
//     }

//     // Другие методы для авторизации, выборки и управления данными
// private:
//     sqlite3* db_;
// };