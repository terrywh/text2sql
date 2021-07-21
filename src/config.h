#pragma once
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <string>
#include <vector>

class config {
    boost::filesystem::path log_;
    boost::filesystem::path  db_;
    std::string table_;
    boost::regex         record_;
    boost::regex         search_;
    std::vector<std::string> fields_;

    void extract_fields(boost::regex& regex);

public:
    config(const boost::program_options::variables_map& vm);

    const boost::filesystem::path& source() const { return log_; }
    const boost::filesystem::path& target() const { return db_; }
    const std::string& target_table() const { return table_; }
    const boost::regex& record() const { return record_; }
    const boost::regex& search() const { return search_; }
    const std::vector<std::string>& fields() const { return fields_; }
};
