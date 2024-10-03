#include "cpp-httplib/httplib.h"
#include <iostream>
#include <json/json.h>
#include <json/value.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using namespace httplib;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

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
    auto collection = db["test"];
    auto doc_value = make_document(
        kvp("name", "MongoDB"),
        kvp("type", "database"),
        kvp("count", 1),
        kvp("versions", make_array("v6.0", "v5.0", "v4.4", "v4.2", "v4.0", "v3.6")),
        kvp("info", make_document(kvp("x", 203), kvp("y", 102)))
    );
    auto insert_one_result = collection.insert_one(doc_value.view());

    svr.Get("/ping", [&collection](const auto& req, auto& res) {
        Json::Value json;

        auto cursor_all = collection.find({});
    
        for (auto doc : cursor_all) {
            std::cout << bsoncxx::to_json(doc, bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;
        }

        json["response"] = "OK";
        JSON_RESPONSE(json);
    });
    
    svr.Options("/GetAllDrug", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "*");
    });

    svr.Get("/GetAllDrug", [](const Request& req, Response& res) {
        Json::Value json;

        json["response"] = "OK";
        JSON_RESPONSE(json);
    });

    svr.listen("0.0.0.0", 8080);
}