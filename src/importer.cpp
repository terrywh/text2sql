#include "importer.h"
#include <iostream>

static void on_error_exit(char* error) {
    if (error) {
        std::cerr << "ERROR> " << error << std::endl;
        sqlite3_free(error);
        exit(-1);
    }
}

void importer::create_table() {
    char* error;
    std::string sql ("CREATE TABLE IF NOT EXISTS ");
    sql.append(conf_.target_table());
    sql.append(" (rowid INTEGER PRIMARY KEY AUTOINCREMENT");
    int field_index = 0;
    for (auto field : conf_.fields()) {
        sql.push_back(',');
        sql.append(field);
    }
    sql.append(");");
    std::cout << "SQL> " << sql << std::endl;
    sqlite3_exec(db_.get(), sql.c_str(), nullptr, nullptr, &error);
    on_error_exit(error);
}

void importer::prepare_stmt() {
    std::string sql {"INSERT INTO `record` VALUES (NULL"};
    for (auto field : conf_.fields()) sql.append(", ?");
    sql.append(");");
    std::cout << "SQL> " << sql << std::endl;

    sqlite3_stmt* stmt;
    if (int r = sqlite3_prepare(db_.get(), sql.c_str(), sql.size(), &stmt, nullptr); r != 0) {
        std::cerr << "ERROR> failed to prepare statement, " << sqlite3_errstr(r) << ", fields not matching with exisiting table ?" << std::endl;
        exit(-1);
    }   
    stmt_.reset(stmt, sqlite3_finalize);
}


importer::batcher::batcher(std::shared_ptr<sqlite3> db, std::shared_ptr<sqlite3_stmt> stmt, int size)
: db_(db)
, stmt_(stmt)
, batch_(size)
, count_(0) {
    char* error;
    sqlite3_exec(db.get(), "BEGIN TRANSACTION", nullptr, nullptr, &error);
    on_error_exit(error);
}

importer::batcher::~batcher() {
    char* error;
    sqlite3_exec(db_.get(), "END TRANSACTION", nullptr, nullptr, &error);
    on_error_exit(error);
    std::cout << "\n";
}

void importer::batcher::insert(std::vector<std::string>& row) {
    if (++count_ % batch_ == 0) {
        std::cout << ".";
        char* error;
        sqlite3_exec(db_.get(), "END TRANSACTION", nullptr, nullptr, &error);
        on_error_exit(error);
        sqlite3_exec(db_.get(), "BEGIN TRANSACTION", nullptr, nullptr, &error);
        on_error_exit(error);
    }
    for (int i=0; i<row.size(); ++i) 
        sqlite3_bind_text(stmt_.get(), i+1, row[i].c_str(), row[i].size(), SQLITE_STATIC);
    
    while (sqlite3_step(stmt_.get()) == SQLITE_BUSY) usleep(1000); // 等待执行完毕
    sqlite3_clear_bindings(stmt_.get());
    sqlite3_reset(stmt_.get());
}


importer::importer(const config& conf)
: conf_(conf)
, is_(conf_.source().c_str()) {
    if (!is_.is_open()) {
        std::cerr << "ERROR> failed to open source file, " << conf.source() << std::endl;
        exit(-1);
    }

    sqlite3*  db;
    char* error;
    
    if (int r = sqlite3_open(conf_.target().c_str(), &db); r != 0) {
        std::cerr << "ERROR> failed to open/create database, " << sqlite3_errstr(r);
        exit(-1);
    }
    db_.reset(db, sqlite3_close);
    sqlite3_exec(db, "PRAGMA journal_mode = OFF", nullptr, nullptr, &error);
    on_error_exit(error);

    create_table();
    prepare_stmt();
}

void importer::run() {
    batcher batch(db_, stmt_);
    std::string line;
    std::vector<std::string> row;
    
    while (!is_.eof()) {
        std::getline(is_, line);
        import_line(line, row, batch);
    }
}

void importer::import_line(const std::string& line, std::vector<std::string>& row, batcher& batch) {
    boost::smatch match;
    if (boost::regex_match(line, match, conf_.record())) {
        if (!row.empty()) {
            batch.insert(row);
            row.clear();
        }
        row.resize(conf_.fields().size());
        for (int i=0;i<conf_.fields().size();++i)
            row[i].append(match[ conf_.fields()[i] ]);
    }
    if (!row.empty() && boost::regex_search(line, match, conf_.search())) {
        for (int i=0;i<conf_.fields().size();++i)
            row[i].append(match[ conf_.fields()[i] ]);
    }
}