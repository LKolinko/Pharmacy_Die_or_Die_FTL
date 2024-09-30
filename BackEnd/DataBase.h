#pragma once
#include <map>
#include <mutex>
#include "Drug.h"
class DataBase {
public:
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
private:
    std::mutex mutex_;
    std::map<std::string, std::map<std::string, std::map<int64_t, std::map<std::string, Drug>>>> data_;
};