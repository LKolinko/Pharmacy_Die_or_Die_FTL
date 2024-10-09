#pragma once

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <bsoncxx/builder/basic/document.hpp>
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

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
class Drug {
public:
    Drug() = default;

    Drug(Json::Value json) : Drug(bsoncxx::from_json(json.toStyledString())) {}

    Drug(bsoncxx::v_noabi::document::view   bson) {
        name_ = bson["name"].get_string().value;
        group_ = bson["group"].get_string().value;
        type_ = bson["type"].get_string().value;
        dosage_ = bson["dosage"].get_int32();
        expiration_date_ = bson["expiration_date"].get_int32().value;
        quantity_ = bson["quantity"].get_int32();
        retail_price_ = bson["retail_price"].get_int32().value;
    }

    Drug(std::string name, std::string group, std::string type, 
    int64_t dosage, std::string expiration_date, int64_t quantity, int32_t retail_price)
    : dosage_(dosage), quantity_(quantity),
    name_(name), type_(type), group_(group),
    retail_price_(retail_price),
    expiration_date_(expiration_date_) {}

    Json::Value ToJson() {
        Json::Value json;
        json["name"] = name_;
        json["group"] = group_;
        json["type"] = type_;
        json["dosage"] = dosage_;
        json["expiration_date"] = expiration_date_;
        json["quantity"] = quantity_;
        json["retail_price"] = retail_price_;
        return json;
    }

    bsoncxx::document::value ToBson() {
        auto doc_value = make_document(
            kvp("name", name_),
            kvp("group", group_),
            kvp("type", type_),
            kvp("dosage", dosage_),
            kvp("expiration_date", expiration_date_),
            kvp("retail_price", retail_price_),
            kvp("quantity", quantity_)
        );
        return doc_value;
    }

    bsoncxx::document::value ToFindBson() {
        auto doc_value = make_document(
            kvp("name", name_),
            kvp("group", group_),
            kvp("type", type_),
            kvp("dosage", dosage_),
            kvp("expiration_date", expiration_date_),
            kvp("retail_price", retail_price_)
        );
        return doc_value;
    }

    bool time_validation(int current_time) {
        if (current_time >= expiration_date_) {
            return false;
        }
        if (expiration_date_ - current_time <= 30) {
            retail_price_ /= 2;
        }
        return true;
    }

    friend std::istream& operator>>(std::istream& in, Drug& drug); 
    friend std::ostream& operator<<(std::ostream& out, Drug& drug);

    int32_t expiration_date_;
    std::string name_, type_, group_;
    int32_t retail_price_;
    int32_t quantity_, dosage_;
};

std::istream& operator>>(std::istream& in, Drug& drug) {
    in >> drug.name_ >> drug.group_ >> drug.type_ >> drug.dosage_
    >> drug.quantity_ >> drug.retail_price_;
    return in;
}

std::ostream& operator<<(std::ostream& out, Drug& drug) {
    out << drug.name_ << ' ' << drug.group_ << ' ' << drug.type_ << ' ' << drug.dosage_
    << ' ' << drug.quantity_ << ' ' << drug.retail_price_;
    return out;
}