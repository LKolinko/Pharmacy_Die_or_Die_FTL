#pragma once

#include "Drug.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <json/json.h>
#include <json/value.h>
#include <string>
#include <fstream>
#include <chrono>
#include <array>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

class Dealer {
public:
    Dealer() = default;

    Dealer(Json::Value json) : Dealer(bsoncxx::from_json(json.toStyledString())) {};

    Dealer(bsoncxx::v_noabi::document::view bson) {
        last_name_ = bson["last_name"].get_string().value;
        address_ = bson["address"].get_string().value;
        phone_number_ = bson["phone_number"].get_string().value;
        last_ = bson["last"].get_int32().value;
        if (bson["drugs"]) {
            auto mas = bson["drugs"].get_array().value;
            for (auto& e : mas) {
                drugs_.push_back(Drug(e.get_document()));
            }
        }
    }

    bsoncxx::document::value ToFindBson() {
        auto doc_value = make_document(
            kvp("last_name", last_name_),
            kvp("address", address_),
            kvp("phone_number", phone_number_)
        );
        return doc_value;
    }

    bsoncxx::document::value ToBson() {
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::open_array;
        using bsoncxx::builder::stream::close_array;
        using bsoncxx::builder::stream::finalize;

        document data_builder{};
        data_builder << "last_name" << last_name_ << "address" << address_ << "phone_number" 
        << phone_number_ << "last" << last_;
        auto array_builder = data_builder << "drugs" << open_array;
        for (auto& u : drugs_) {
            array_builder << u.ToBson();
        }
        array_builder << close_array;
        bsoncxx::document::value doc = data_builder << finalize;
        return doc;
    }

    bsoncxx::document::value ToDataBson() {
        auto doc_value = make_document(
            kvp("last_name", last_name_),
            kvp("address", address_),
            kvp("phone_number", phone_number_),
            kvp("last", last_)
        );
        return doc_value;
    }

    Json::Value ToOrderJson() {
        Json::Value json;
        json["last_name"] = last_name_;
        json["address"] = address_;
        json["phone_number"] = phone_number_;
        json["drugs"] = Json::arrayValue;
        for (auto& drug : drugs_) {
            json["drugs"].append(drug.ToJson());
        }
        return json;
    }

    Json::Value ToNameJson(bool is_alive) {
        Json::Value json;
        json["last_name"] = last_name_;
        json["address"] = address_;
        json["phone_number"] = phone_number_;
        json["last"] = last_;
        json["in_alive"] = is_alive;
        return json;
    }

    std::string address_, last_name_, phone_number_;
    int32_t last_;
    std::vector<Drug> drugs_;
};