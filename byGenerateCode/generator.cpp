// File:        generator.cpp
// Description: This header file for generate the code which for protobuf message to json convert
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2016-04-22 by Haust

#include "generator.hpp"

#include <fstream>
#include <exception>

bool Generator::Generate(std::string proto_path, std::string output_path) {
    if (0 == output_path.length()) {
        output_path = "pb2json.hpp";
    }

    if (0 == proto_path.size()) {
        proto_path = "json_proto.proto";
    }

    if (!AnalyseProtoFile(proto_path)) return false;
    if (!GenerateFileHead(output_path)) return false;
    if (!GenerateFileBody(output_path)) return false;

    return true;
}

bool Generator::AnalyseProtoFile(std::string const& proto_path) {
    try {
        std::ifstream ofs(proto_path);
        if (!ofs.is_open()) {
            err_string = "Open file " + proto_path + " failed!";
            return false;
        }
    }
    catch (std::exception e) {
        err_string = e.what();
        return false;
    }

    return true;
}

bool Generator::GenerateFileHead(std::string const& output_path) {
    std::string::size_type idx = output_path.find_last_of(".");
    if (std::string::npos == idx) {
        err_string = "There is not the file postfix";
        return false;
    }

    std::string file_postfix = output_path.substr(idx);
    if (file_postfix != ".hpp" && file_postfix != ".h" && file_postfix != ".hxx") {
        err_string = "The output file format must be a c or c++ head file";
        return false;
    }

    std::string file_name;
    idx = output_path.find_first_not_of("/");
    if (std::string::npos != idx) {
        file_name = output_path.substr(0, idx);
    } else {
        file_name = output_path;
    }

    try {
        std::ofstream ofs(output_path);
        if (!ofs.is_open()) {
            err_string = "Open file " + output_path + " failed!";
            return false;
        }

        ofs << "// File:        " << file_name << std::endl
            << "// Description: This header file for protobuf message and json interconversion" << std::endl
            << "// Notes:       ---" << std::endl << "// Author:      Haust <wyy123_2008@qq.com>" << std::endl
            << "// Revision:    2015-11-23 by Haust" << std::endl << std::endl << "#pragma once" << std::endl
            << std::endl;

        ofs.close();
    }
    catch (std::exception e) {
        err_string = e.what();
        return false;
    }
    return true;
}

bool Generator::GenerateFileBody(std::string const& output_path) {
    try {
        std::ofstream ofs(output_path, std::ofstream::ate);
        if (!ofs.is_open()) {
            err_string = "Open file " + output_path + " failed!";
            return false;
        }
    }
    catch (std::exception e) {
        err_string = e.what();
        return false;
    }
    return true;
}
