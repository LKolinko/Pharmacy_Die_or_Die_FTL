#include "cpp-httplib/httplib.h"
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/pool.hpp>
#include <mutex>
#include <algorithm>
#include <map>

#include "Dealer.h"

using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    int32_t generatin_time = 0, courier = 0, today_req_cnt = 0;

    Server svr;
    mongocxx::instance instance{};
    
    std::string uri_string = std::getenv("MONGO_URI");
    if (uri_string.empty()) {
        std::cerr << "MONGO_URI is not set" << std::endl;
        return EXIT_FAILURE;
    }

    mongocxx::pool pool{mongocxx::uri{uri_string}};
    
    std::jthread t([&]{
        while (true) {
            auto client = pool.acquire();
            auto drugs = (*client)["mydb"]["drugs"];
            try {
                auto all = drugs.find({});
                Client cli("http://Getmodul:5252");
                std::map<std::vector<std::string>, int> mp;
                for (auto &doc : all) {
                    auto u = Drug(doc);
                    auto all_this = drugs.find(u.ToFindBsonNoData());
                    if (mp.find({ u.name_, u.type_, u.group_ }) != mp.end()) {
                        continue;
                    }
                    mp[{ u.name_, u.type_, u.group_ }] += u.quantity_;
                    for (auto &doc_my : all_this) {
                        mp[{ u.name_, u.type_, u.group_ }] += doc_my["quantity"].get_int32();
                    }
                    if (mp[{ u.name_, u.type_, u.group_ }] <= 50) {
                        u.quantity_ = 256;
                        cli.Post("/ReqDrugs", u.ToJson().toStyledString(), JSON_CONTENT);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << ' ' << 8 << '\n';
            }
        }
    });
    t.detach();

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

    svr.Get("/GetAllDrugs", [&](const Request& req, Response& res) {
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        Json::Value json(Json::arrayValue);
        try {
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

            JSON_RESPONSE(json);
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 1 << '\n';
        }
    });

    svr.Post("/AddItem", [&](const Request& req, Response& res) {
        Json::Value json;
        Json::Reader reader;
        reader.parse(req.body, json);        
        if (json.empty()) {
            return;
        }
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        try {
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
        } catch (const mongocxx::operation_exception& e) {
            std::cerr << e.what() << ' ' << 2 << '\n';
        }
    });

    svr.Post("/SetGenData", [&](const Request& req, Response& res) {
        Json::Value json;
        Json::Reader reader;
        generatin_time = 0;
        reader.parse(req.body, json);
        if (json.empty()) {
            return;
        }
        courier = json["courier"].asInt();
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto dilers = (*client)["mydb"]["clients"];
        auto orders = (*client)["mydb"]["orders"];
        auto solve_today = (*client)["mydb"]["solve_today"];
        auto total_solve = (*client)["mydb"]["total_solve"];
        auto days_statistic = (*client)["mydb"]["days_statistic"];
        try {
            drugs.delete_many({});
            dilers.delete_many({});
            orders.delete_many({});
            solve_today.delete_many({});
            total_solve.delete_many({});
            days_statistic.delete_many({});

            courier = json["courier"].asInt();
            for (auto u : json["drugs"]) {
                drugs.insert_one(Drug(u).ToBson());
            }
            
            json["response"] = "OK";
            JSON_RESPONSE(json);
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 3 << '\n';
        }
    });

    svr.Get("/GetAllClients", [&](const Request& req, Response& res) {
        Json::Value json = Json::arrayValue;
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto dilers = (*client)["mydb"]["clients"];
        try {
            auto all_dilers = dilers.find({});
            for (auto diler : all_dilers) {
                Dealer dil(diler);
                json.append(dil.ToNameJson(generatin_time - dil.last_ <= 2));
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 4 << '\n';
        }
        JSON_RESPONSE(json);
    });

    svr.Post("/AddRequests", [&](const Request& req, Response& res) {
        Json::Value json;
        Json::Reader reader;
        reader.parse(req.body, json);
        if (json.empty()) {
            return;
        }
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto orders = (*client)["mydb"]["orders"];
        auto dilers = (*client)["mydb"]["clients"];
        try {
            for (auto& u : json) {
                Dealer client(u);
                orders.insert_one(client.ToBson());

                ++today_req_cnt;

                auto diler = dilers.find_one(client.ToFindBson());
                if (diler) {
                    auto doc = diler->view();
                    bsoncxx::builder::stream::document update_builder;
                    update_builder << "$set" << bsoncxx::builder::stream::open_document
                    << "last" << generatin_time 
                    << bsoncxx::builder::stream::close_document;
                    dilers.update_one(doc, update_builder.view());
                } else {
                    client.last_ = generatin_time;
                    dilers.insert_one(client.ToDataBson());
                }
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 5 << '\n';
        }
    });

    svr.Get("/GetAllOrders", [&](const Request& req, Response& res) {
        Json::Value json(Json::arrayValue);
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto orders = (*client)["mydb"]["orders"];
        try {
            auto cursor_all = orders.find({});
            std::vector<Dealer> d;
            for (auto doc : cursor_all) {
                d.push_back(Dealer(doc));
            }
            for (auto& u : d) {
                json.append(u.ToOrderJson());
            }
            JSON_RESPONSE(json);
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 6 << '\n';
        }
    });

    svr.Get("/GetTotalSolve", [&](const Request& req, Response& res) {
        Json::Value json(Json::arrayValue);
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto total_solve = (*client)["mydb"]["total_solve"];
        try {
            auto all_solve = total_solve.find({});
            for (auto &solve_req : all_solve) {
                json.append(Dealer(solve_req).ToOrderJson());
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        JSON_RESPONSE(json);
    });

    svr.Get("/GetSolveToday", [&](const Request& req, Response& res) {
        Json::Value json(Json::arrayValue);
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto solve_today = (*client)["mydb"]["solve_today"];
        try {
            auto cursor_all = solve_today.find({});
            std::vector<Dealer> d;
            for (auto doc : cursor_all) {
                d.push_back(Dealer(doc));
            }
            for (auto& u : d) {
                json.append(u.ToOrderJson());
            }
            JSON_RESPONSE(json);
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 7 << '\n';
        }
    });

    svr.Get("/GetDaysStatistic", [&](const Request& req, Response& res) {
        Json::Value json(Json::arrayValue);
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto days_statistic = (*client)["mydb"]["days_statistic"];
        try {
            auto all = days_statistic.find({});
            for (auto& day : all) {
                Json::Value day_json;
                day_json["number"] = day["number"].get_int32().value;
                day_json["solve"] = day["solve"].get_int32().value;
                day_json["profit"] = day["profit"].get_int32().value;
                day_json["cnt_req"] = day["cnt_req"].get_int32().value;
                json.append(day_json);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        JSON_RESPONSE(json);
    });

    svr.Get("/NextDay", [&](const Request& req, Response& res) {
        ++generatin_time;
        Json::Value json;
        auto client = pool.acquire();
        auto drugs = (*client)["mydb"]["drugs"];
        auto solve_today = (*client)["mydb"]["solve_today"];
        auto orders = (*client)["mydb"]["orders"];
        auto total_solve = (*client)["mydb"]["total_solve"];
        auto days_statistic = (*client)["mydb"]["days_statistic"];
        try {
            solve_today.delete_many({});
            
            auto all_orders = orders.find({});
            std::vector<Dealer> orders_mas;
            for (auto doc : all_orders) {
                orders_mas.push_back(Dealer(doc));
            }

            std::map<std::vector<std::string>, int> mp;

            std::vector<std::pair<int64_t, int>> drugs_profit_ind;
            for (int ind = 0; auto &client : orders_mas) {
                int64_t profit = 0;
                bool is_posible = true;
                for (auto &drug : client.drugs_) {
                    auto drug_data_base = drugs.find(drug.ToFindOrderBson());
                    for (auto& drug_bd : drug_data_base) {
                        auto u = Drug(drug_bd);
                        if (!u.time_validation(generatin_time)) {
                            drugs.delete_one(drug_bd);
                        }
                        if (mp.find({ u.name_, u.type_, u.group_ }) != mp.end()) {
                            continue;
                        }
                        mp[{ u.name_, u.type_, u.group_ }] += u.quantity_;
                    }
                    if (mp[{ drug.name_, drug.type_, drug.group_ }] >= drug.quantity_) {
                        is_posible = true;
                        profit += drug.retail_price_ * drug.quantity_ / 4;
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
            int cnt = 0, day_profit = 0;

            for (auto &[profit, ind] : drugs_profit_ind) {
                bool is_posible = true;
                if (cnt == courier) {
                    break;
                }
                for (auto &drug : orders_mas[ind].drugs_) {
                    if (mp[{ drug.name_, drug.type_, drug.group_ }] >= drug.quantity_) {
                        is_posible = true;
                    } else {
                        is_posible = false;
                    }
                }
                if (is_posible) {
                    for (auto drug : orders_mas[ind].drugs_) {

                        mp[{ drug.name_, drug.type_, drug.group_ }] -= drug.quantity_;

                        auto drugs_data_base = drugs.find(drug.ToFindOrderBson());
                        for (auto &doc : drugs_data_base) {
                            if (drug.quantity_ == 0) {
                                break;
                            }
                            if (doc["quantity"].get_int32() > drug.quantity_) {
                                bsoncxx::builder::stream::document update_builder;
                                update_builder << "$inc" << bsoncxx::builder::stream::open_document
                                << "quantity" << -drug.quantity_
                                << bsoncxx::builder::stream::close_document;
                                drug.quantity_ = 0;
                                auto update_one_result = drugs.update_one(doc, update_builder.view());
                            } else if (doc["quantity"].get_int32() <= drug.quantity_) {
                                drug.quantity_ -= doc["quantity"].get_int32();
                                drugs.delete_one(doc);
                            }
                        }
                        if (mp[{ drug.name_, drug.type_, drug.group_ }] == 0) {
                            Client cli("http://Getmodul:5252");
                            Drug req_drug(drug);
                            req_drug.quantity_ = 256;
                            cli.Post("/ReqDrugs", drug.ToJson().toStyledString(), JSON_CONTENT);
                        }
                    }
                    ++cnt;
                    day_profit += profit;
                    solve_today.insert_one(orders_mas[ind].ToBson());
                    total_solve.insert_one(orders_mas[ind].ToBson());
                    orders.delete_one(orders_mas[ind].ToBson());
                }
            }

            auto doc_value = make_document(
                kvp("number", generatin_time),
                kvp("solve", cnt),
                kvp("profit", day_profit),
                kvp("cnt_req", today_req_cnt)
            );
            days_statistic.insert_one(doc_value.view());

            today_req_cnt = 0;
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 9 << '\n';
        }
        std::cerr << "Solve Day № " << generatin_time << '\n';
    });

    svr.listen("0.0.0.0", 8080);
}
