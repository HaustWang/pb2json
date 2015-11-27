// File:        pb2json.h
// Description: This imply file for protobuf message and json interconversion 
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2015-11-23 by Haust

#include "pb2json.h"

bool Pb2Json::Json2Message(const Json& json, ProtobufMsg& message){
    auto descriptor = message.GetDescriptor();
    auto reflection = message.GetReflection();
    if(nullptr == descriptor || nullptr == reflection)
        return false;

    auto count = descriptor->field_count();
    for(auto i = 0; i < count; ++i){
        const auto field = descriptor->field(i);
        if(nullptr == field)
            continue;

        auto& value = json[field->name()];
        if(value.is_null())
            continue;

        if(field->is_repeated()) {
            if(!value.is_array()){
                return false;
            } else {
                Json2RepeatedMessage(value, message, field, reflection);
                continue;
            }
        }

        switch(field->type()){
            case ProtobufFieldDescriptor::TYPE_INT32:
                {
                    if(value.is_number())
                        reflection->SetInt32(&message, field, value.get<int32_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_UINT32:
                {
                    if(value.is_number())
                        reflection->SetUInt32(&message, field, value.get<uint32_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_INT64:
                {
                    if(value.is_number())
                        reflection->SetInt64(&message, field, value.get<int64_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_UINT64:
                {
                    if(value.is_number())
                        reflection->SetUInt64(&message, field, value.get<uint64_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                {
                    if(value.is_number())
                        reflection->SetFloat(&message, field, value.get<float>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_DOUBLE:
                {
                    if(value.is_number())
                        reflection->SetDouble(&message, field, value.get<double>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
                {
                    if(value.is_string())
                        reflection->SetString(&message, field, value.get<std::string>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_MESSAGE:
                {
                    if(value.is_object())
                        Json2Message(value, *reflection->MutableMessage(&message, field));
                }
                break;
            default:
                break;
        }
    }
    return true;
}

bool Pb2Json::Json2RepeatedMessage(const Json& json, ProtobufMsg& message,
        const ProtobufFieldDescriptor* field, const ProtobufReflection* reflection){
    int count = json.count(field->name());
    for(auto j = 0; j < count; ++j){
        switch(field->type()){
            case ProtobufFieldDescriptor::TYPE_INT32:
                {
                    if(json[j].is_number())
                        reflection->AddInt32(&message, field, json[j].get<int32_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_UINT32:
                {
                    if(json[j].is_number())
                        reflection->AddUInt32(&message, field, json[j].get<uint32_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_INT64:
                {
                    if(json[j].is_number())
                        reflection->AddInt64(&message, field, json[j].get<int64_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_UINT64:
                {
                    if(json[j].is_number())
                        reflection->AddUInt64(&message, field, json[j].get<uint64_t>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                {
                    if(json[j].is_number())
                        reflection->AddFloat(&message, field, json[j].get<float>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_DOUBLE:
                {
                    if(json[j].is_number())
                        reflection->AddDouble(&message, field, json[j].get<double>());
                }
                break;
            case ProtobufFieldDescriptor::TYPE_MESSAGE:
                {
                    if(json[j].is_object())
                        Json2Message(json[j], *reflection->AddMessage(&message, field));
                }
                break;
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
                {
                    if(json[j].is_string())
                        reflection->AddString(&message, field, json[j].get<std::string>());
                }
                break;
            default:
                break;
        }
    }
    return true;
}

void Pb2Json::Message2Json(const ProtobufMsg& message, Json& json){
    auto descriptor = message.GetDescriptor();
    auto reflection = message.GetReflection();
    if(nullptr == descriptor || nullptr == descriptor)
        return;

    auto count = descriptor->field_count();

    for (auto i = 0; i < count; ++i){
        const auto field = descriptor->field(i);

        if (field->is_repeated()){
            if (reflection->FieldSize(message, field) > 0)
                RepeatedMessage2Json(message, field, reflection, json[field->name()]);

            continue;
        }

        if (!reflection->HasField(message, field)){
            continue;
        }

        switch (field->type()){
            case ProtobufFieldDescriptor::TYPE_MESSAGE:
                {
                    const ProtobufMsg& tmp_message = reflection->GetMessage(message, field);
                    if (0 != tmp_message.ByteSize())
                        Message2Json(tmp_message, json[field->name()]);
                }
                break;
            case ProtobufFieldDescriptor::TYPE_INT32:
                json[field->name()] = reflection->GetInt32(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT32:
                json[field->name()] = reflection->GetUInt32(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_INT64:
                json[field->name()] = reflection->GetInt64(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT64:
                json[field->name()] = reflection->GetUInt64(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                json[field->name()] = reflection->GetFloat(message, field);
            case ProtobufFieldDescriptor::TYPE_BYTES:
                json[field->name()] = reflection->GetString(message, field);
                break;
            default:
                break;
        }
    }
}

void Pb2Json::RepeatedMessage2Json(const ProtobufMsg &message,
                                        const ProtobufFieldDescriptor* field,
                                        const ProtobufReflection* reflection,
                                        Json &json){
    if (nullptr == field || nullptr == reflection){
        Message2Json(message, json);
    }

    for (auto i = 0; i < reflection->FieldSize(message, field); ++i){
        Json tmp_json;
        switch (field->type()){
            case ProtobufFieldDescriptor::TYPE_MESSAGE:
                {
                    const ProtobufMsg& tmp_message = reflection->GetRepeatedMessage(message, field, i);
                    if (0 != tmp_message.ByteSize())
                    {
                        Message2Json(tmp_message, tmp_json);
                    }
                }
                break;
            case ProtobufFieldDescriptor::TYPE_INT32:
                tmp_json = reflection->GetRepeatedInt32(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT32:
                tmp_json = reflection->GetRepeatedUInt32(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_INT64:
                tmp_json = reflection->GetInt64(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT64:
                tmp_json = reflection->GetUInt64(message, field);
                break;
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                tmp_json = reflection->GetRepeatedFloat(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
                tmp_json = reflection->GetRepeatedString(message, field, i);
                break;
            default:
                break;
        }
        json += tmp_json;
    }
}
