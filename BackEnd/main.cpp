#include "cpp-httplib/httplib.h"
#include "DataBase.h"
using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    Server svr;
    DataBase bd;

    svr.Get("/ping", [](const auto& req, auto& res) {
        Json::Value json;
        json["response"] = "OK";
        JSON_RESPONSE(json);
    });
    
    svr.Options("/api", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "*");
    });

    svr.listen("0.0.0.0", 8080);
}