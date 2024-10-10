#include "cpp-httplib/httplib.h"
#include <fstream>
#include <vector>
#include <random>
#include <sstream>

#include "Dealer.h"
#include "Drug.h"

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

using namespace httplib;

int main() {
    std::random_device rg;
    std::mt19937 rng(rg());
    std::uniform_int_distribution<> gen_quantity(1, 100);
    std::uniform_int_distribution<> gen_price(500, 15000);
    std::uniform_int_distribution<> gen_data(1, 100);

    std::vector<Drug> drugs(35);
    std::vector<std::string> names;
    std::vector<std::string> streets;
    std::vector<std::string> numbers;

    std::stringstream in(drug_string);
    for (auto& u : drugs) {
        in >> u;
        u.quantity_ = gen_quantity(rng);
        u.expiration_date_ = gen_data(rng);
        u.retail_price_ = gen_price(rng);
    }
    in = std::stringstream(user_names);
    for (std::string name; in >> name;) {
        names.push_back(name);
    }
    in = std::stringstream(user_streets);
    for (std::string street; in >> street;) {
        streets.push_back(street);
    }
    in = std::stringstream(user_phone);
    for (std::string phone; in >> phone;) {
        numbers.push_back(phone);
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

    svr.Post("/NewGeneration", [&cli, &drugs, &rng](const Request &req, Response &res) {
        std::shuffle(drugs.begin(), drugs.end(), rng);
        Json::Value json;
        Json::Value input;
        Json::Reader reader;
        reader.parse(req.body, input);        
        if (input.empty()) {
            return;
        }
        int n = input["days"].asInt(), m = input["courier"].asInt(), k = input["drugs"].asInt();
        try {
            json["courier"] = m;
            json["drugs"] = Json::arrayValue;
            for (int i = 0; i < k; ++i) {
                json["drugs"].append(drugs[i].ToJson());
            }
            cli.Post("/SetGenData", json.toStyledString(), JSON_CONTENT);
            json["answer"] = "New generation has been created";
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            json["answer"] = "New generation has not been created";
            JSON_RESPONSE(json);
            std::cerr << e.what() << '\n';
        }
    });

    svr.Post("/NextDay", [&cli, &rng, &names, &streets, &numbers](const Request &req, Response &res) {
        Json::Value json;
        try {            
            std::vector<Drug> discounted_drugs, drugs;
            
            auto data = cli.Get("/GetAllDrugs");
            
            Json::Value input;
            Json::Reader reader;
            reader.parse(data->body, input);    
            
            for (auto& u : input) {
                auto drug = Drug(u);
                if (drug.discounted) {
                    discounted_drugs.push_back(drug);
                } else {
                    drugs.push_back(drug);
                }
            }

            std::uniform_int_distribution<> gen(1, 10);
            std::uniform_int_distribution<> gen_cnt(1, 50);
            std::uniform_int_distribution<> gen_ver(1, 10);
            
            std::vector<Dealer> clients;
            
            for (int tmp = gen(rng); tmp > 0; --tmp) {
                auto client = Dealer(names, streets, numbers);
                for (auto &drug : discounted_drugs) {
                    if (gen_ver(rng) < 5) {
                        auto drug_req = drug;
                        drug_req.quantity_ = gen_cnt(rng);
                        client.add_drug(drug_req);
                    }
                }
                for (auto &drug : drugs) {
                    if (gen_ver(rng) < 3) {
                        auto drug_req = drug;
                        drug_req.quantity_ = gen_cnt(rng);
                        client.add_drug(drug_req);
                    }
                }
                if (!client.empty()) {
                    clients.push_back(client);
                }
            }

            Json::Value req_json = Json::arrayValue;
            for (auto &u : clients) {
                req_json.append(u.ToJson());
            }

            cli.Post("/NextDay", req_json.toStyledString(), JSON_CONTENT);

            json["answer"] = "Next day OK";
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            json["answer"] = "Dizdaaa";
            JSON_RESPONSE(json);
            std::cerr << e.what() << '\n';
        }
    });

    svr.listen("0.0.0.0", 5252);
}
