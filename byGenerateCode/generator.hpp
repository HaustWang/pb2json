// File:        generator.hpp
// Description: This header file for generate the code which for protobuf message to json convert
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2016-04-22 by Haust

#pragma once
#include <string>

class Generator {
    inline std::string const& ErrString() { return err_string; }

    bool Generate(std::string proto_path = "json_proto.proto", std::string output_path = "pb2json.hpp");

protected:
    bool AnalyseProtoFile(std::string const& proto_path);
    bool GenerateFileHead(std::string const& output_path);
    bool GenerateFileBody(std::string const& output_path);

private:
    std::string err_string = "";
};
