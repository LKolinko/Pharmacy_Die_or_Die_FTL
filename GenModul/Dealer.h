#pragma once

#include "Drug.h"
#include "Data.h"

#include <json/json.h>
#include <json/value.h>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <array>
#include <random>

class Dealer {
public:
    Dealer() = default;

    Dealer(std::vector<std::string>& names,
            std::vector<std::string>& streets,
            std::vector<std::string>& numbers) {
        std::random_device rg;
        std::mt19937 rng(rg());
        std::uniform_int_distribution<> gen(0, names.size());
        address_ = streets[gen(rng)];
        last_name_ = names[gen(rng)];
        phone_number_ = numbers[gen(rng)];
        last_ = -1;
        period_ = -1;
    }

    void add_drug(Drug& drug) {
        drugs_.push_back(drug);
    }

    Json::Value ToJson() {
        Json::Value json;
        json["last_name"] = last_name_;
        json["address"] = address_;
        json["phone_number"] = phone_number_;
        json["period"] = period_;
        json["last"] = last_;
        json["drugs"] = Json::arrayValue;
        for (auto& drug : drugs_) {
            json["drugs"].append(drug.ToJson());
        }
        return json;
    }

    bool empty() {
        return drugs_.empty();
    }

private:
    std::vector<Drug> drugs_;
    std::string address_, last_name_, phone_number_;
    int32_t period_, last_;
};