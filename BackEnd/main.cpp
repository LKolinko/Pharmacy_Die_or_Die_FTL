#include "cpp-httplib/httplib.h"
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mutex>
#include <algorithm>
#include <map>

#include "Dealer.h"

using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    int32_t generatin_time = 0, courier = 0, today_req_cnt = 0;
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
    auto total_solve = db["total_solve"];
    auto days_statistic = db["days_statistic"];

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
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 1 << '\n';
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
            std::cerr << e.what() << ' ' << 2 << '\n';
        }
    });

    svr.Post("/SetGenData", [&drugs, &client, &data_base_mutex, &dilers, &orders, 
    &generatin_time, &courier, &solve_today, &total_solve](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        Json::Value json;
        Json::Reader reader;
        generatin_time = 0;
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
            total_solve.delete_many({});

            courier = json["courier"].asInt();
            for (auto u : json["drugs"]) {
                drugs.insert_one(Drug(u).ToBson());
            }
            
            session.commit_transaction();    
            json["response"] = "OK";
            JSON_RESPONSE(json);
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 3 << '\n';
        }
    });

    svr.Get("/GetAllClients", [&client, &dilers, &generatin_time, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        Json::Value json = Json::arrayValue;
        auto session = client.start_session();
        try {
            session.start_transaction();

            auto all_dilers = dilers.find({});
            for (auto diler : all_dilers) {
                Dealer dil(diler);
                json.append(dil.ToNameJson(generatin_time - dil.last_ <= 2));
            }
            session.commit_transaction();
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 4 << '\n';
            session.commit_transaction();
        }
        JSON_RESPONSE(json);
    });

    svr.Post("/AddRequests", [&orders, &client, &data_base_mutex, &dilers, 
    &generatin_time, &today_req_cnt](const Request& req, Response& res) {
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

            ++today_req_cnt;

            for (auto& u : json) {
                Dealer client(u);
                orders.insert_one(client.ToBson());

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
            session.commit_transaction(); 
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 5 << '\n';
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
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 6 << '\n';
        }
    });

    svr.Get("/GetTotalSolve", [&total_solve, &client, &data_base_mutex](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json(Json::arrayValue);
        auto session = client.start_session();
        try {
            session.start_transaction();

            auto all_solve = total_solve.find({});
            for (auto &solve_req : all_solve) {
                json.append(Dealer(solve_req).ToOrderJson());
            }

            session.commit_transaction();
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
        JSON_RESPONSE(json);
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
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 7 << '\n';
        }
    });

    svr.Get("/NextDay", [&drugs, &generatin_time, &orders, &client, &data_base_mutex, 
    &solve_today, &courier, &total_solve, &days_statistic, &today_req_cnt](const Request& req, Response& res) {
        std::lock_guard g(data_base_mutex);
        ++generatin_time;
        Json::Value json;
        auto session = client.start_session();
        try {
            session.start_transaction();

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


            session.commit_transaction();
        } catch (const std::exception& e) {
            std::cerr << e.what() << ' ' << 8 << '\n';
        }
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
                    auto drug_data_base = drugs.find_one(drug.ToFindOrderBson());
                    if (drug_data_base) {
                        auto doc = drug_data_base->view();
                        Drug doc_drug(doc);
                        doc_drug.time_validation(generatin_time);
                        if (doc_drug.quantity_ >= drug.quantity_) {
                            profit += drug.quantity_ * doc_drug.retail_price_ / 4;
                        } else {
                            is_posible = false;
                            break;
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
            int cnt = 0, day_profit = 0;

            for (auto &[profit, ind] : drugs_profit_ind) {
                bool is_posible = true;
                if (cnt == courier) {
                    break;
                }
                for (auto &drug : orders_mas[ind].drugs_) {
                    auto drug_data_base = drugs.find_one(drug.ToFindOrderBson());
                    if (drug_data_base) {
                        auto doc = drug_data_base->view();
                        if (doc["quantity"].get_int32() < drug.quantity_) {
                            is_posible = false;
                        }
                    } else {
                        is_posible = false;
                    }
                }
                if (is_posible) {
                    for (auto &drug : orders_mas[ind].drugs_) {
                        auto drug_data_base = drugs.find_one(drug.ToFindOrderBson());
                        auto doc = drug_data_base->view();
                        if (doc["quantity"].get_int32() > drug.quantity_) {
                            bsoncxx::builder::stream::document update_builder;
                            update_builder << "$inc" << bsoncxx::builder::stream::open_document
                            << "quantity" << -drug.quantity_
                            << bsoncxx::builder::stream::close_document;
                            auto update_one_result = drugs.update_one(doc, update_builder.view());
                        } else if (doc["quantity"].get_int32() == drug.quantity_) {
                            Client cli("http://Getmodul:5252");
                            Drug req_drug(drug);
                            req_drug.quantity_ = 256;
                            cli.Post("/ReqDrugs", drug.ToJson().toStyledString(), JSON_CONTENT);
                            drugs.delete_one(doc);
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

            session.commit_transaction();
        } catch (const std::exception& e) {
            session.abort_transaction();
            std::cerr << e.what() << ' ' << 9 << '\n';
        }
        std::cerr << "Solve Day № " << generatin_time << '\n';
    });

    svr.listen("0.0.0.0", 8080);
}
