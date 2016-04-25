// File:        generator.cpp
// Description: This header file for generate the code which for protobuf message to json convert
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2016-04-22 by Haust

#include "generator.hpp"

#include <fstream>
#include <sstream>
#include <exception>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <ctime>

enum ContentType {
    NONE,
    ENUM,
    MESSAGE,
};

bool isContent(const char* line) {
    const char* p = strstr(line, "//");
    if (nullptr == p) return false;

    for (auto* s = line; s != p; ++s) {
        if (!isblank(*s)) return false;
    }

    return true;
}

std::string getUseful(const char* line) {
    std::string linestr(line);
    auto pos = linestr.find_first_of("//");
    if (std::string::npos == pos) return linestr;

    for (; pos != 0; --pos) {
        if (!isblank(linestr[pos])) break;
    }

    for (auto i = 0; i < pos; ++pos) {
        if (!isblank(linestr[i])) return linestr.substr(i, pos);
    }

    return "";
}

std::string getName(std::string const& line, std::string::size_type start) {
    for (; start < line.length(); ++start) {
        if (!isblank(line[start])) break;
    }

    auto end = line.length() - 1;
    for (; end > start; --end) {
        if (line[end] == '{') continue;
        if (!isblank(line[end])) break;
    }

    return line.substr(start, end - start + 1);
}

std::string getDateString() {
    time_t now;
    time(&now);

    struct tm* now_tm = localtime(&now);

    char tmp[128] = {0};
    strftime(tmp, sizeof(tmp), "%Y-%m-%d", now_tm);
    return tmp;
}

bool isProtoOriginalType(std::string const& type) {
    return (type == "int32" || type == "uint32" || type == "sint32" || type == "fixed32" || type == "sfixed32" ||
            type == "int64" || type == "uint64" || type == "sint64" || type == "fixed64" || type == "sfixed64" ||
            type == "double" || type == "float" || type == "bool" || type == "string" || type == "bytes");
}

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
        std::ifstream ifs(proto_path);
        if (!ifs.is_open()) {
            err_string = "Open file " + proto_path + " failed!";
            return false;
        }

        char line[256] = {0};
        ContentType type = NONE;
        std::string message_name;
        while (!ifs.eof()) {
            memset(line, 0, sizeof(line));
            ifs.getline(line, 256);
            std::string useful = getUseful(line);
            if (useful.empty()) continue;

            if (NONE == type) {
                if (useful.substr(0, 4) == "enum" && isblank(useful[4])) {
                    type = ENUM;
                    enum_vec.push_back(getName(useful, 5));
                } else if (useful.substr(0, 7) == "message" && isblank(useful[7])) {
                    type = MESSAGE;
                    message_name = getName(useful, 8);
                    message_map[message_name] = message_type();
                }
                continue;
            }

            if (ENUM == type) {
                if (useful.substr(0, 1) == "}") type = NONE;
                continue;
            }

            if (MESSAGE == type) {
                if (useful.substr(0, 1) == "}") {
                    type = NONE;
                    message_name.clear();
                } else {
                    std::istringstream iss(useful);
                    std::cout << iss.str() << std::endl;
                    Element ele;
                    iss >> ele.label >> ele.type >> ele.name;
                    if (!isProtoOriginalType(ele.type)) {
                        if (std::find(enum_vec.begin(), enum_vec.end(), ele.type) == enum_vec.end() &&
                            message_map.find(ele.type) == message_map.end()) {
                            err_string += "There is not a definition of message " + ele.type +
                                          " which is the type of element " + ele.name + " in message " + message_name +
                                          "\n";
                            continue;
                        }
                    }

                    auto pos = ele.name.find("=");
                    if (std::string::npos != pos) ele.name = ele.name.substr(0, pos);

                    message_map[message_name].push_back(ele);
                }
            }
        }

        ifs.close();
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
    idx = output_path.find_first_of("/");
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

        ofs << "// File:        " << file_name << "\n"
            << "// Description: This header file for protobuf message and json interconversion\n"
            << "// Notes:       ---\n"
            << "// Author:      Haust <wyy123_2008@qq.com>\n"
            << "// Revision:    " << getDateString() << " by Haust\n\n"
            << "#pragma once\n\n";

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
        std::ofstream ofs(output_path, std::ofstream::app);
        if (!ofs.is_open()) {
            err_string = "Open file " + output_path + " failed!";
            return false;
        }

        for (auto& pair : message_map) {
            ofs << "class " << (char)toupper(pair.first[0]) << pair.first.substr(1) << "Translator;\n";
        }

        ofs << std::endl << "#include \"../json.hpp\"\n\n";

        for (auto& pair : message_map) {
            ofs << "class " << (char)toupper(pair.first[0]) << pair.first.substr(1) << "Translator {\n"
                << "public:\n"
                << "\tstatic nlohmann::json ToJson(" << pair.first << " const& msg) {\n"
                << "\t\tnlohmann::json json;\n" << GenerateMessage2Json(pair.first, 2) << "\n"
                << "\t\treturn json;\n"
                << "\t}\n\n"
                << "\tstatic " << pair.first << " ToMessage(nlohmann::json const& json) {\n"
                << "\t\t" << pair.first << " msg;\n" << GenerateJson2Message(pair.first, 2) << "\n"
                << "\t\treturn msg;\n"
                << "\t}\n"
                << "};\n\n";
        }

        ofs.close();
    }
    catch (std::exception e) {
        err_string = e.what();
        return false;
    }
    return true;
}

std::string Generator::GenerateMessage2Json(std::string const& message_name, int countOfTab) {
    if (message_map.find(message_name) == message_map.end()) return "";

    std::string tab_lable;
    for (auto i = 0; i < countOfTab; ++i) tab_lable += "\t";

    auto& msg = message_map[message_name];
    std::ostringstream oss;

    for (auto& ele : msg) {
        if (isProtoOriginalType(ele.type)) {
            if (ele.label == "repeated") {
                oss << tab_lable << "for(auto ele : msg." << ele.name << "()) {\n";
                oss << tab_lable << "\tnlohmann::json tmp_json = ele;\n";
                oss << tab_lable << "\tjson[\"" << ele.name << "\"] += tmp_json;\n";
                oss << tab_lable << "}\n\n";
            } else {
                oss << tab_lable << "json[\"" << ele.name << "\"] = msg." << ele.name << "();\n\n";
            }
        } else {
            if (std::find(enum_vec.begin(), enum_vec.end(), ele.type) != enum_vec.end()) {
                if (ele.label == "repeated") {
                    oss << tab_lable << "for(auto ele : msg." << ele.name << "()) {\n";
                    oss << tab_lable << "\tnlohmann::json tmp_json = (int)ele;\n";
                    oss << tab_lable << "\tjson[\"" << ele.name << "\"] += tmp_json;\n";
                    oss << tab_lable << "}\n\n";
                } else {
                    oss << tab_lable << "json[\"" << ele.name << "\"] = (int)msg." << ele.name << "();\n\n";
                }
            } else {
                std::ostringstream translator;
                translator << (char)toupper(ele.type[0]) << ele.type.substr(1) << "Translator::ToJson";
                if (ele.label == "repeated") {
                    oss << tab_lable << "for(auto& ele : msg." << ele.name << "())\n";
                    oss << tab_lable << "\tjson[\"" << ele.name << "\"] += " << translator.str() << "(ele);\n\n";
                } else {
                    oss << tab_lable << "json[\"" << ele.name << "\"] = " << translator.str() << "(msg." << ele.name
                        << "());\n\n";
                }
            }
        }
    }

    return oss.str();
}

std::string Generator::GenerateJson2Message(std::string const& message_name, int countOfTab) {
    if (message_map.find(message_name) == message_map.end()) return "";

    std::string tab_lable;
    for (auto i = 0; i < countOfTab; ++i) tab_lable += "\t";

    auto& msg = message_map[message_name];
    std::ostringstream oss;

    for (auto& ele : msg) {
        if (isProtoOriginalType(ele.type)) {
            std::string type;
            if (ele.type == "string" || ele.type == "bytes")
                type = "string";
            else if (ele.type == "double")
                type = "double";
            else if (ele.type == "float")
                type = "float";
            else if (ele.type == "uint32" || ele.type == "fixed32")
                type = "uint32_t";
            else if (ele.type == "int32" || ele.type == "sfixed32" || ele.type == "sint32" || ele.type == "bool")
                type = "int32_t";
            else if (ele.type == "uint64" || ele.type == "fixed64")
                type = "uint64_t";
            else if (ele.type == "int64" || ele.type == "sfixed64" || ele.type == "sint64")
                type = "int64_t";

            std::string is_type;
            if (ele.type != "string" && ele.type != "bytes")
                is_type = "is_number";
            else
                is_type = "is_string";

            // check is repetead
            if (ele.label == "repeated") {
                oss << tab_lable << "if (!json[\"" << ele.name << "\"].is_array()) {\n";
                oss << tab_lable << "\tif(json[\"" << ele.name << "\"]." << is_type << "())\n";
                oss << tab_lable << "\t\tmsg.add_ " << ele.name << "(json[\"" << ele.name << "\"].get<" << type
                    << ">());\n";
                oss << tab_lable << "} else {\n";
                oss << tab_lable << "\tint count = json.count(" << ele.name << ");\n";
                oss << tab_lable << "\tfor(auto i = 0; i < count; ++i) {\n";
                oss << tab_lable << "\t\tif(json[\"" << ele.name << "\"][i]." << is_type << "())\n";
                oss << tab_lable << "\t\t\tmsg.add_" << ele.name << "(json[\"" << ele.name << "\"][i].get<" << type
                    << ">());\n";
                oss << tab_lable << "\t}\n";
                oss << tab_lable << "}\n\n";
            } else {
                oss << tab_lable << "if(json[\"" << ele.name << "\"]." << is_type << "())\n";
                oss << tab_lable << "\tmsg.set_" << ele.name << "(json[\"" << ele.name << "\"].get<" << type
                    << ">());\n\n";
            }
        } else {
            if (std::find(enum_vec.begin(), enum_vec.end(), ele.type) != enum_vec.end()) {
                if (ele.label == "repeated") {
                    oss << tab_lable << "if (!json[\"" << ele.name << "\"].is_array()) {\n";
                    oss << tab_lable << "\tif(json[\"" << ele.name << "\"].is_number())\n";
                    oss << tab_lable << "\t\tmsg.add_" << ele.name << "(json[\"" << ele.name << "\"].get<int>());\n";
                    oss << tab_lable << "} else {\n";
                    oss << tab_lable << "\tint count = json.count(" << ele.name << ");\n";
                    oss << tab_lable << "\tfor(auto i = 0; i < count; ++i) {\n";
                    oss << tab_lable << "\t\tif(json[\"" << ele.name << "\"][i].is_number())\n";
                    oss << tab_lable << "\t\t\tmsg.add_" << ele.name << "(json[\"" << ele.name
                        << "\"][i].get<int>());\n";
                    oss << tab_lable << "\t}\n";
                    oss << tab_lable << "}\n\n";
                } else {
                    oss << tab_lable << "if(json[\"" << ele.name << "\"].is_number())\n";
                    oss << tab_lable << "\tmsg.set_" << ele.name << "(json[\"" << ele.name << "\"].get<int>());\n\n";
                }
            } else {
                std::ostringstream translator;
                translator << (char)toupper(ele.type[0]) << ele.type.substr(1) << "Translator::ToMessage";
                if (ele.label == "repeated") {
                    oss << tab_lable << "if (!json[\"" << ele.name << "\"].is_array()) {\n";
                    oss << tab_lable << "\tif(json[\"" << ele.name << "\"].is_object())\n";
                    oss << tab_lable << "\t\tmsg.add_" << ele.name << "()->CopyFrom(" << translator.str() << "(json[\""
                        << ele.name << "\"]));\n";
                    oss << tab_lable << "} else {\n";
                    oss << tab_lable << "\tint count = json.count(" << ele.name << ");\n";
                    oss << tab_lable << "\tfor(auto i = 0; i < count; ++i) {\n";
                    oss << tab_lable << "\t\tif(json[\"" << ele.name << "\"][i].is_object())\n";
                    oss << tab_lable << "\t\t\tmsg.add_" << ele.name << "()->CopyFrom(" << translator.str()
                        << "(json[\"" << ele.name << "\"][i]));\n";
                    oss << tab_lable << "\t}\n";
                    oss << tab_lable << "}\n\n";
                } else {
                    oss << tab_lable << "if(json[\"" << ele.name << "\"].is_object())\n";
                    oss << tab_lable << "\tmsg.mutable_" << ele.name << "()->CopyFrom(" << translator.str()
                        << "(json[\"" << ele.name << "\"]));\n\n";
                }
            }
        }
    }

    return oss.str();
}
