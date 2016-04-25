// File:        pb2json.hpp
// Description: This header file for protobuf message and json interconversion
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2016-04-25 by Haust

#pragma once

class Test1Translator;
class Test2Translator;

#include "../json.hpp"

class Test1Translator {
public:
	static nlohmann::json ToJson(test1 const& msg) {
		nlohmann::json json;
		json["test_1"] = msg.test_1();

		json["test_2"] = msg.test_2();

		json["test_3"] = msg.test_3();


		return json;
	}

	static test1 ToMessage(nlohmann::json const& json) {
		test1 msg;
		if(json["test_1"].is_number())
			msg.set_test_1(json["test_1"].get<int32_t>());

		if(json["test_2"].is_number())
			msg.set_test_2(json["test_2"].get<int32_t>());

		if(json["test_3"].is_string())
			msg.set_test_3(json["test_3"].get<string>());


		return msg;
	}
};

class Test2Translator {
public:
	static nlohmann::json ToJson(test2 const& msg) {
		nlohmann::json json;
		for(auto& ele : msg.test_1())
			json["test_1"] += Test1Translator::ToJson(ele);

		json["test_2"] = msg.test_2();

		for(auto ele : msg.test_3()) {
			nlohmann::json tmp_json = ele;
			json["test_3"] += tmp_json;
		}

		for(auto ele : msg.test_6()) {
			nlohmann::json tmp_json = (int)ele;
			json["test_6"] += tmp_json;
		}


		return json;
	}

	static test2 ToMessage(nlohmann::json const& json) {
		test2 msg;
		if (!json["test_1"].is_array()) {
			if(json["test_1"].is_object())
				msg.add_test_1()->CopyFrom(Test1Translator::ToMessage(json["test_1"]));
		} else {
			int count = json.count(test_1);
			for(auto i = 0; i < count; ++i) {
				if(json["test_1"][i].is_object())
					msg.add_test_1()->CopyFrom(Test1Translator::ToMessage(json["test_1"][i]));
			}
		}

		if(json["test_2"].is_string())
			msg.set_test_2(json["test_2"].get<string>());

		if (!json["test_3"].is_array()) {
			if(json["test_3"].is_number())
				msg.add_ test_3(json["test_3"].get<int64_t>());
		} else {
			int count = json.count(test_3);
			for(auto i = 0; i < count; ++i) {
				if(json["test_3"][i].is_number())
					msg.add_test_3(json["test_3"][i].get<int64_t>());
			}
		}

		if (!json["test_6"].is_array()) {
			if(json["test_6"].is_number())
				msg.add_test_6(json["test_6"].get<int>());
		} else {
			int count = json.count(test_6);
			for(auto i = 0; i < count; ++i) {
				if(json["test_6"][i].is_number())
					msg.add_test_6(json["test_6"][i].get<int>());
			}
		}


		return msg;
	}
};

