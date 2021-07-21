#include "config.h"
#include <iostream>

void config::extract_fields(boost::regex& regex) {
    for (int i=0; i<regex.mark_count(); ++i) {
        auto x = regex.subexpression(i);
        std::string_view s (x.first + 1, x.second - x.first - 1);
        if (s[0] == '?' && s[1] == '<') 
            fields_.push_back( std::string(s.substr(2, s.find_first_of('>') - 2)) );
    }
}

config::config(const boost::program_options::variables_map& vm)
: log_(vm.at("source").as<std::string>()) {
    if (!vm.count("target"))
        db_ = log_.parent_path() / (log_.stem().string() + ".db");
    else
        db_ = vm.at("target").as<std::string>();
    table_ = vm.at("target_table").as<std::string>();
    try {
        record_.assign(vm.at("record").as<std::string>(), boost::regex_constants::save_subexpression_location);
        search_.assign(vm.at("search").as<std::string>(), boost::regex_constants::save_subexpression_location);
    } 
    catch(const std::exception& ex) {
        std::cerr << "ERROR> failed to compile regex, " << ex.what()
            << "\t record = " << vm["record"].as<std::string>() << "\n"
            << "\t extra = " << vm["search"].as<std::string>() << "\n";
        exit(-1);
    }
    extract_fields(record_);
    extract_fields(search_);
}
