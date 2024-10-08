#include "cpp-httplib/httplib.h"
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mutex>
#include <algorithm>

#include "Dealer.h"

using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    int64_t generatin_time = 0, courier = 0, global_profit = 0;
    std::mutex data_base_mutex;

    Server svr;
    mongocxx::instance instance{};
    
    std::string uri_string = std::getenv("MONGO_URI");
    if (uri_string.empty()) {
        std::cerr << "MONGO_URI is not set" << std::endl;
        return EXIT_FAILURE;
    }

    mongocxx::client client{mongocxx::uri{uri_string}};

    auto db = client["mydb"];
    auto drugs = db["drugs"];
    auto dilers = db["clients"];
    auto orders = db["orders"];
    auto solve_today = db["solve_today"];

    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Access-Control-Allow-Credentials", "true");
        res.set_content("", "text/plain");
    });

    svr.Get("/ping", [](const auto& req, auto& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json;
        json["response"] = "OK";
        JSON_RESPONSE(json);
    });

    svr.Get("/GetAllDrugs", [&drugs, &client, &data_base_mutex, &generatin_time](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json(Json::arrayValue);
        auto session = client.start_session();
        try {
            session.start_transaction();

            auto cursor_all = drugs.find({});
            std::vector<Drug> d;
            for (auto doc : cursor_all) {
                d.push_back(Drug(doc));
            }
            for (auto& u : d) {
                auto old_drug = u;
                auto result = u.time_validation(generatin_time);
                if (!result) {
                    drugs.delete_one(old_drug.ToBson());
                }
                json.append(u.ToJson());
            }

            session.commit_transaction();
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Post("/AddItem", [&drugs, &client, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json;
        Json::Reader reader;
        reader.parse(req.body, json);        
        if (json.empty()) {
            return;
        }
        auto session = client.start_session();
        try {
            session.start_transaction();
            Drug e(json);
            auto result = drugs.find_one(e.ToFindBson());
            if (!result) {
                json["answer"] = "New Drug added";
                auto insert_one_result = drugs.insert_one(e.ToBson());
            } else {
                auto doc = result->view();
                bsoncxx::builder::stream::document update_builder;
                update_builder << "$inc" << bsoncxx::builder::stream::open_document
                << "quantity" << e.quantity_ 
                << bsoncxx::builder::stream::close_document;
                auto update_one_result = drugs.update_one(doc, update_builder.view());
                json["answer"] = "number of drugs updated";
            }
            JSON_RESPONSE(json);
            session.commit_transaction();
        } catch (const mongocxx::operation_exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Post("/SetGenData", [&drugs, &client, &data_base_mutex, &dilers, &orders, 
    &generatin_time, &courier, &global_profit, &solve_today](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        Json::Value json;
        Json::Reader reader;
        generatin_time = 0;
        global_profit = 0;
        reader.parse(req.body, json);
        if (json.empty()) {
            return;
        }
        courier = json["courier"].asInt();
        auto session = client.start_session();
        try {
            session.start_transaction();
            drugs.delete_many({});
            dilers.delete_many({});
            orders.delete_many({});
            solve_today.delete_many({});
            courier = json["courier"].asInt();
            for (auto u : json["drugs"]) {
                drugs.insert_one(Drug(u).ToBson());
            }
            session.commit_transaction();    
            json["response"] = "OK";
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Post("/AddRequests", [&orders, &client, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        Json::Value json;
        Json::Reader reader;
        reader.parse(req.body, json);
        if (json.empty()) {
            return;
        }
        auto session = client.start_session();
        try {
            session.start_transaction();
            for (auto& u : json) {
                Dealer client(u);
                orders.insert_one(client.ToBson());
            }
            session.commit_transaction(); 
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Get("/GetAllOrders", [&orders, &client, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json(Json::arrayValue);
        auto session = client.start_session();
        try {
            session.start_transaction();
            auto cursor_all = orders.find({});
            std::vector<Dealer> d;
            for (auto doc : cursor_all) {
                d.push_back(Dealer(doc));
            }
            for (auto& u : d) {
                json.append(u.ToOrderJson());
            }
            session.commit_transaction();
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Get("/GetSolveToday", [&solve_today, &client, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json(Json::arrayValue);
        auto session = client.start_session();
        try {
            session.start_transaction();
            auto cursor_all = solve_today.find({});
            std::vector<Dealer> d;
            for (auto doc : cursor_all) {
                d.push_back(Dealer(doc));
            }
            for (auto& u : d) {
                json.append(u.ToOrderJson());
            }
            session.commit_transaction();
            JSON_RESPONSE(json);
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.Get("/NextDay", [&drugs, &generatin_time, &orders, &client, &data_base_mutex, 
    &solve_today, &global_profit, &courier](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        ++generatin_time;
        Json::Value json;
        auto session = client.start_session();
        std::cerr << "YES" << '\n';
        std::cerr << courier << '\n';
        try {
            session.start_transaction();

            solve_today.delete_many({});
            
            auto all_orders = orders.find({});
            std::vector<Dealer> orders_mas;
            for (auto doc : all_orders) {
                orders_mas.push_back(Dealer(doc));
            }

            std::vector<std::pair<int64_t, int>> drugs_profit_ind;
            for (int ind = 0; auto &client : orders_mas) {
                int64_t profit = 0;
                bool is_posible = true;
                for (auto &drug : client.drugs_) {
                    auto drug_data_base = drugs.find_one(drug.ToFindBson());
                    if (drug_data_base) {
                        auto doc = drug_data_base->view();
                        if (doc["quantity"].get_int32() >= drug.quantity_) {
                            profit += drug.quantity_ * doc["retail_price"].get_int32() / 4;
                        } else {
                            is_posible = false;
                        }
                    } else {
                        is_posible = false;
                        break;
                    }
                }
                if (is_posible) {
                    drugs_profit_ind.push_back({ profit, ind });
                }
                ++ind;
            }
            std::sort(drugs_profit_ind.rbegin(), drugs_profit_ind.rend());
            int cnt = 0;

            for (auto &[profit, ind] : drugs_profit_ind) {
                bool is_posible = true;
                if (cnt == courier) {
                    break;
                }
                for (auto &drug : orders_mas[ind].drugs_) {
                    auto drug_data_base = drugs.find_one(drug.ToFindBson());
                    if (drug_data_base) {
                        auto doc = drug_data_base->view();
                        if (doc["quantity"].get_int32() > drug.quantity_) {
                            bsoncxx::builder::stream::document update_builder;
                            update_builder << "$inc" << bsoncxx::builder::stream::open_document
                            << "quantity" << -drug.quantity_
                            << bsoncxx::builder::stream::close_document;

                            auto update_one_result = drugs.update_one(doc, update_builder.view());
                        } else if (doc["quantity"].get_int32() == drug.quantity_) {
                            drugs.delete_one(doc);
                        } else {
                            is_posible = false;
                        }
                    } else {
                        is_posible = false;
                        break;
                    }
                }
                if (is_posible) {
                    ++cnt;
                    global_profit += profit;
                    solve_today.insert_one(orders_mas[ind].ToBson());
                    orders.delete_one(orders_mas[ind].ToBson());
                }
            }

            session.commit_transaction();
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.listen("0.0.0.0", 8080);
}
