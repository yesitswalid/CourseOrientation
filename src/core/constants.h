#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace AppConstants {

constexpr int UDP_PORT = 1234;
constexpr int MYSQL_PORT_DEFAULT = 3306;

constexpr const char *DB_LOCAL_NAME    = "course.db";
constexpr const char *DB_CONNECTION    = "QSQLITE";
constexpr const char *MYSQL_CONNECTION = "QMYSQL";
constexpr const char *CONFIG_DIR       = "Configuration/";
constexpr const char *CONFIG_FILE      = "config.json";

constexpr int CARD_ID_MIN_LEN = 7;
constexpr int CARD_ID_MAX_LEN = 8;

}

#endif // CONSTANTS_H
