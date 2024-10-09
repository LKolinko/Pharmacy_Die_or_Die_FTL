#include "cpp-httplib/httplib.h"
#include <fstream>
#include <vector>
#include <random>
#include <sstream>
#include "Drug.h"
#include "Data.h"
#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

using namespace httplib;

int main() {
    std::random_device rg;
    std::mt19937 rng(rg());
    std::uniform_int_distribution<> gen_quantity(1, 1000);
    std::uniform_int_distribution<> gen_price(500, 15000);
    std::uniform_int_distribution<> gen_data(1, 100);
    std::vector<Drug> drugs(35);
    std::stringstream in(drug_string);
    for (auto& u : drugs) {
        in >> u;
        u.quantity_ = gen_quantity(rng);
        u.expiration_date_ = gen_quantity(rng);
        u.retail_price_ = gen_price(rng);
    }
    
    Client cli("http://BackEnd:8080");
    Server svr;

    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Access-Control-Allow-Credentials", "true");
        res.set_content("", "text/plain");
    });

    svr.Get("/ping", [](const auto& req, auto& res) {
        Json::Value json;
        json["response"] = "OK";
        JSON_RESPONSE(json);
    });

    svr.Get("/hi", [&cli, &drugs](const Request &, Response &res) {
        Json::Value json;
        try {
            json["drugs"] = Json::arrayValue;
            for (auto& u : drugs) {
                json["drugs"].append(u.ToJson());
            }
            auto res = cli.Post("/SetGenData", json.toStyledString(), JSON_CONTENT);
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << '\n';
        }
        json["response"] = "OK";
        JSON_RESPONSE(json);
    });

    svr.listen("0.0.0.0", 5252);
}