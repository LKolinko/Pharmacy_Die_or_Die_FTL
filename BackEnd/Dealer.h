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
        period_ = bson["period"].get_int32().value;
        last_ = bson["last"].get_int32().value;
        auto mas = bson["drugs"].get_array().value;
        for (auto& e : mas) {
            drugs_.push_back(Drug(e.get_document().view()));
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
        bsoncxx::builder::stream::array drugs_bson;
        for (auto& drug : drugs_) {
            drugs_bson << drug.ToBson();
        }
        drugs_bson << bsoncxx::builder::stream::finalize;
        auto doc_value = make_document(
            kvp("last_name", last_name_),
            kvp("address", address_),
            kvp("phone_number", phone_number_),
            kvp("period", period_),
            kvp("last", last_),
            kvp("drugs", drugs_bson)
        );
        return doc_value;
    }

    std::string address_, last_name_, phone_number_;
    int32_t period_, last_;
    std::vector<Drug> drugs_;
};