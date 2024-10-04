#pragma once

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>


#include <json/json.h>
#include <json/value.h>
#include <string>
#include <fstream>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
class Drug {
public:
    Drug() = default;

    Drug(Json::Value json) {
        name_ = json["name"].asString();
        group_ = json["group"].asString();
        type_ = json["type"].asString();
        dosage_ = json["dose"].asString();
        expiration_date_ = json["expiryDate"].asString();
        quantity_ = std::stoi(json["quantity"].asString());
        retail_price_ = json["retailPrice"].asString();
    }

    Drug(std::string name, std::string group, std::string type, 
    std::string dosage, std::string expiration_date, int64_t quantity, std::string retail_price)
    : dosage_(dosage), quantity_(quantity),
    name_(name), type_(type), group_(group),
    retail_price_(retail_price),
    expiration_date_(expiration_date_) {}

    bsoncxx::document::value ToBson() {
        auto doc_value = make_document(
            kvp("name", name_),
            kvp("group", group_),
            kvp("type", type_),
            kvp("dosage", std::stoi(dosage_)),
            kvp("expiration_date", expiration_date_),
            kvp("retail_price", std::stoi(retail_price_)),
            kvp("quantity", quantity_)
        );
        return doc_value;
    }

    bsoncxx::document::value ToFindBson() {
        auto doc_value = make_document(
            kvp("name", name_),
            kvp("group", group_),
            kvp("type", type_),
            kvp("dosage", std::stoi(dosage_)),
            kvp("expiration_date", expiration_date_),
            kvp("retail_price", std::stoi(retail_price_))
        );
        return doc_value;
    }

    friend std::istream& operator>>(std::istream& in, Drug& drug); 
    friend std::ostream& operator<<(std::ostream& out, Drug& drug);

    std::string dosage_, expiration_date_;
    std::string name_, type_, group_;
    std::string retail_price_;
    int64_t quantity_;
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