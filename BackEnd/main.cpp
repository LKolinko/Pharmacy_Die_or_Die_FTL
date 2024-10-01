#include "cpp-httplib/httplib.h"
#include <json/json.h>
#include <json/value.h>
using namespace httplib;

#define JSON_CONTENT "application/json"
#define JSON_RESPONSE(json) res.set_content(json.toStyledString(), "application/json")

int main() {
    Server svr;

    svr.Get("/ping", [](const auto& req, auto& res) {
        Json::Value json;
        json["response"] = "OK";
        JSON_RESPONSE(json);
    });
    
    svr.Options("/AddDrug", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "*");
    });

    svr.Post("/AddDrug", [](const Request& req, Response& res) {

    });

    svr.listen("0.0.0.0", 8080);
}