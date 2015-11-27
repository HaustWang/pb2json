// File:        pb2json.h
// Description: This header file for protobuf message and json interconversion 
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2015-11-23 by Haust

#pragma once

#include "json.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

class Pb2Json{
    public:
        using Json = nlohmann::json;
        using ProtobufMsg = ::google::protobuf::Message;
        using ProtobufReflection = ::google::protobuf::Reflection;
        using ProtobufFieldDescriptor = ::google::protobuf::FieldDescriptor;
        using ProtobufDescriptor = ::google::protobuf::Descriptor;

    public:
        static bool Json2Message(const Json& json, ProtobufMsg &message);
        static void Message2Json(const ProtobufMsg& message, Json& json);

    protected:
        static bool Json2RepeatedMessage(const Json& json, ProtobufMsg& message,
                const ProtobufFieldDescriptor* field, const ProtobufReflection *reflection);
        static void RepeatedMessage2Json(const ProtobufMsg& message,
                const ProtobufFieldDescriptor *field,
                const ProtobufReflection *reflection,
                Json& json);

};
