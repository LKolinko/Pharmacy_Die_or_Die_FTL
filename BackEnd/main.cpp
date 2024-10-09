#include "cpp-httplib/httplib.h"
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mutex>

#include "Dealer.h"

using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    int64_t generatin_time = 0;
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
                u.time_validation(generatin_time);
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

    svr.Post("/SetGenData", [&drugs, &client, &data_base_mutex, &dilers, &orders](const Request& req, Response& res) {
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
            drugs.delete_many({});
            dilers.delete_many({});
            orders.delete_many({});
            for (auto u : json["drugs"]) {
                drugs.insert_one(Drug(u).ToBson());
            }
            session.commit_transaction();
        } catch (const std::runtime_error& e) {
            session.abort_transaction();
            std::cerr << e.what() << '\n';
        }
    });

    svr.listen("0.0.0.0", 8080);
}
