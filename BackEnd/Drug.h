#pragma once
#include <string>
class Drug {
public:
    Drug() = default;
    Drug(int64_t dosage, int64_t quantity, int64_t retail_price, 
    int64_t wholesale_prices, std::string name, 
    std::string type, std::string group) : dosage_(dosage), quantity_(quantity),
    name_(name), type_(type), group_(group), wholesale_prices_(wholesale_prices),
    retail_price_(retail_price) {}
    
    int64_t dosage_, quantity_;
    std::string name_, type_, group_;
    int64_t wholesale_prices_, retail_price_;
};