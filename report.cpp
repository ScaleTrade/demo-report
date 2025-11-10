// plugin.cpp
#include "Structures.hpp"
#include "ast.hpp"
#include <rapidjson/document.h>
#include <sstream>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

using namespace rapidjson;
using namespace ast;

extern "C" void AboutReport(Value& req, Value& res,  Document::AllocatorType& aloc,  CServerInterface* server) {
    res.AddMember("version", 123, aloc);
}

extern "C" void DestroyReport() {

}

extern "C" void CreateReport(Value& req, Value& res,  Document::AllocatorType& aloc,  CServerInterface* server) {

    std::vector<TradeRecord> trades;
    int ret = server->GetAllOpenTrades( &trades);

    Node chart = ResponsiveContainer({
    LineChart({
        XAxis({}, props({{"dataKey", "day"}})),
        YAxis(),
        Tooltip(),
        Legend(),
        Line({}, props({
            {"type", "monotone"},
            {"dataKey", "pw"},
            {"stroke", "#8884d8"}
        })),
        Bar({}, props({
            {"dataKey", "pu"},
            {"fill", "#82ca9d"}
        }))
    }, props({
        {"data", "[{\"day\":\"Mon\",\"pw\":12,\"pu\":2},{\"day\":\"Tue\",\"pw\":14,\"pu\":3}]"}
    }))
}, props({{"width", "100%"}, {"height", "300"}}));

    // пример текста и таблицы
    Node report = div({
        h1({text("Energy Report")}, props({{"className", "title"}})),
        chart
    }, props({{"className", "report"}}));

    to_json(report, res, aloc);
}
