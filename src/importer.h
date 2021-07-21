#pragma once
#include "config.h"
#include <sqlite3.h>
#include <fstream>

class importer {
    const config&                 conf_;
    std::shared_ptr<sqlite3>        db_;
    std::shared_ptr<sqlite3_stmt> stmt_;
    std::ifstream                   is_;

    std::vector<std::string> row_;

    void create_table();
    void prepare_stmt();

    class batcher {
        std::shared_ptr<sqlite3> db_;
        std::shared_ptr<sqlite3_stmt> stmt_;

        int batch_;
        int count_;

    public:
        batcher(std::shared_ptr<sqlite3> db, std::shared_ptr<sqlite3_stmt> stmt, int size = 512);
        ~batcher();
        void insert(std::vector<std::string>& row);
    };
    void import_line(const std::string& line, std::vector<std::string>& row, batcher& batch);

public:

    
    importer(const config& conf);
    void run();
};
