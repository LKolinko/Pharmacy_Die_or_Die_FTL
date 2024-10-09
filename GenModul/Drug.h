#pragma once

#include <json/json.h>
#include <json/value.h>
#include <string>
#include <fstream>
#include <chrono>

class Drug {
public:
    Drug() = default;

    Drug(Json::Value json) {
        name_ = json["name"].asString();
        group_ = json["group"].asString();
        type_ = json["type"].asString();
        dosage_ = json["dosage"].asInt();
        expiration_date_ = json["expiration_date"].asInt();
        quantity_ = json["quantity"].asInt();
        retail_price_ = json["retail_price"].asInt();
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

    friend std::istream& operator>>(std::istream& in, Drug& drug); 
    friend std::ostream& operator<<(std::ostream& out, Drug& drug);

    int32_t expiration_date_;
    std::string name_, type_, group_;
    int32_t retail_price_;
    int32_t quantity_, dosage_;
};

std::istream& operator>>(std::istream& in, Drug& drug) {
    in >> drug.name_ >> drug.group_ >> drug.dosage_ >> drug.type_;
    return in;
}

std::ostream& operator<<(std::ostream& out, Drug& drug) {
    out << drug.name_ << ' ' << drug.group_ << ' ' << drug.type_ << ' ' << drug.dosage_ << ' ' << drug.expiration_date_
    << ' ' << drug.quantity_ << ' ' << drug.retail_price_;
    return out;
}