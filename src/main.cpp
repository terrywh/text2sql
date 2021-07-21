#include "config.h"
#include "importer.h"
#include <iostream>
#include <boost/program_options.hpp>

const static std::string default_regex_record {R"REGEX(\((?<key>\d+)\) \[(?<timestamp>[^\]]+)\] <(?<thread_id>[^\]]+)> \((?<severity>[^\)]+)\)\s+\[(?<module>[^\]]+)\]\s+(?<message>.+))REGEX"};
const static std::string default_regex_extra  {R"REGEX(payload\(0\) = (?<payload_0>.+)|payload\(1\) = (?<payload_1>.+)|payload\(2\) = (?<payload_2>.+))REGEX"};

int main(int argc, char* argv[]) {
    boost::program_options::options_description desc("text2sql usage");
    desc.add_options()
        ("help,h", "display this help message")
        ("record", boost::program_options::value<std::string>()->default_value(default_regex_record), "regex matching whole log record line")
        ("search", boost::program_options::value<std::string>()->default_value(default_regex_extra), "regex to search for, extracting extra data")
        ("source", boost::program_options::value<std::string>(), "source log file")
        ("target", boost::program_options::value<std::string>(), "sqlite db output file, default to [source].db")
        ("target_table", boost::program_options::value<std::string>()->default_value("record"), "store record in this table of target database");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    vm.notify();

    if (!vm.count("source")) {
        std::cout << desc
            << "\nusing Regex with named capture group, the captured sequence will be stored in the database, eg: \n"
            << "\trecord = [(?<severity>[^\d]+)] (?<message>.+)\n"
            << "\textra = (?<digits>[^\d]+)\n"
            << "will extract the following log record into the target database:\n"
            << "\t[debug] this is the log message with some digits '123\n"
            << "and form a table:\n"
            << "\t| severity |   message   | digits |\n"
            << "\t| -------- | ----------- | ------ |\n"
            << "\t|   debug  | this is ... |   123  |\n";
        exit(-2);
    }
    config  conf(vm);
    importer s(conf);
    s.run();
    return 0;
}
