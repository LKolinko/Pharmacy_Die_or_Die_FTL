#pragma once
#include <map>
#include <mutex>
#include "Drug.h"
#include <json/json.h>
#include <json/value.h>
class DataBase {
public:
    DataBase() {
        std::ifstream in;
        try {
            in.open("save.txt");
            Drug drug;
            while (in >> drug) {
                data_[drug.group_][drug.type_][drug.dosage_][drug.name_] = drug;
            }
            in.close();
        } catch (...) {
            std::cerr << "Save Not Found" << std::endl;
        }
    }
    void Insert(Drug& drug) {
        std::lock_guard g(mutex_);
        if (data_[drug.group_][drug.type_][drug.dosage_].find(drug.name_) != data_[drug.group_][drug.type_][drug.dosage_].end()) {
            data_[drug.group_][drug.type_][drug.dosage_][drug.name_].quantity_ += drug.quantity_;
            return;
        }
        data_[drug.group_][drug.type_][drug.dosage_][drug.name_] = drug;
    }
    void Erase(Drug& drug, int64_t count) {
        std::lock_guard g(mutex_);
        if (data_[drug.group_][drug.type_][drug.dosage_][drug.name_].quantity_ == count) {
            data_[drug.group_][drug.type_][drug.dosage_].erase(drug.name_);
            return;
        }
        data_[drug.group_][drug.type_][drug.dosage_][drug.name_].quantity_ -= count;
    }
    Json::Value ToJson() {
        Json::Value json;
        return json;
    }
    ~DataBase() {
        std::ofstream out("save.txt");
        for (auto& group : data_) {
            for (auto& type : group.second) {
                for (auto& dosage : type.second) {
                    for (auto& name : dosage.second) {
                        out << name.second << ' ';
                    }
                }
            }
        }
        out.close();
    }
private:
    std::mutex mutex_;
    std::map<std::string, std::map<std::string, std::map<int64_t, std::map<std::string, Drug>>>> data_;
};
