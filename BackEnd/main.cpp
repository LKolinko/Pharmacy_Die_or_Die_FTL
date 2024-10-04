#include "cpp-httplib/httplib.h"
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>

#include "Drug.h"

using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int64_t generatin_time = 0;

int main() {
    Server svr;
    mongocxx::instance instance{};
    
    const char* uri_string = std::getenv("MONGO_URI");
    if (!uri_string) {
        std::cerr << "MONGO_URI is not set" << std::endl;
        return EXIT_FAILURE;
    }

    mongocxx::client client{mongocxx::uri{uri_string}};
    auto db = client["mydb"];
    auto Drugs = db["drugs"];

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

    svr.Get("/GetAllDrugs", [&Drugs](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json;
        auto cursor_all = Drugs.find({});
        for (auto doc : cursor_all) {
            json.append(bsoncxx::to_json(doc));
        }
        JSON_RESPONSE(json);
    });

    svr.Post("/AddItem", [&Drugs](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        Json::Value json;
        Json::Reader reader;
        reader.parse(req.body, json);
        if (json.empty()) {
            return;
        }
        Drug e(json);
        auto result = Drugs.find_one(e.ToFindBson());
        if (!result) {
            json["answer"] = "New Drug added";
            auto insert_one_result = Drugs.insert_one(e.ToBson());
        } else {
            auto doc = result->view();
            bsoncxx::builder::stream::document update_builder;
            update_builder << "$inc" << bsoncxx::builder::stream::open_document
               << "quantity" << e.quantity_ 
               << bsoncxx::builder::stream::close_document;
            auto update_one_result = Drugs.update_one(doc, update_builder.view());
            json["answer"] = "number of drugs updated";
        }
        JSON_RESPONSE(json);
    });

    svr.listen("0.0.0.0", 8080);
}
