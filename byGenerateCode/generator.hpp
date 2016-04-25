// File:        generator.hpp
// Description: This header file for generate the code which for protobuf message to json convert
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2016-04-22 by Haust

#pragma once
#include <string>
#include <vector>
#include <map>

class Generator {
public:
    inline std::string const& ErrString() { return err_string; }

    bool Generate(std::string proto_path = "json_proto.proto", std::string output_path = "pb2json.hpp");

protected:
    bool AnalyseProtoFile(std::string const& proto_path);
    bool GenerateFileHead(std::string const& output_path);
    bool GenerateFileBody(std::string const& output_path);
    std::string GenerateMessage2Json(std::string const& message_name, int countOfTab);
    std::string GenerateJson2Message(std::string const& message_name, int countOfTab);

    struct Element {
        std::string label;
        std::string type;
        std::string name;
    };

    using message_type = std::vector<Element>;
    using message_map_type = std::map<std::string, message_type>;

private:
    std::string err_string = "";
    message_map_type message_map;
    std::vector<std::string> enum_vec;
};
