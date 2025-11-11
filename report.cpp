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
    res.AddMember("name", Value().SetString("Demo report", aloc), aloc);
    res.AddMember("description", Value().SetString("Demo report with line and table", aloc), aloc);
    res.AddMember("type", REPORT_DAILY_TYPE, aloc);
}


extern "C" void DestroyReport() {

}

extern "C" void CreateReport(Value& req, Value& res,  Document::AllocatorType& aloc,  CServerInterface* server) {

    std::vector<TradeRecord> trades;
    int ret = server->GetAllOpenTrades( &trades);

    struct DayData { std::string day; double commission; double profit; };
    std::vector<DayData> data = {
        {"Mon", 120.5, 80.2},
        {"Tue", 150.3, 95.6},
        {"Wed", 130.1, 88.0},
        {"Thu", 170.2, 110.3},
        {"Fri", 160.8, 102.4}
    };

    // ---------- (Recharts LineChart) ----------
    Node chart = ResponsiveContainer({
        LineChart({
            XAxis({}, props({{"dataKey", "day"}})),
            YAxis(),
            Tooltip(),
            Legend(),
            Line({}, props({
                {"type", "monotone"},
                {"dataKey", "commission"},
                {"stroke", "#8884d8"}
            })),
            Line({}, props({
                {"type", "monotone"},
                {"dataKey", "profit"},
                {"stroke", "#82ca9d"}
            }))
        }, props({
            {"data", JSONArray{}}
        }))
    }, props({
        {"width", "100%"},
        {"height", 300.0}
    }));

    // Заполняем данные для графика
    JSONArray jsonData;
    for (const auto& d : data) {
        jsonData.push_back(JSONObject{
            {"day", d.day},
            {"commission", d.commission},
            {"profit", d.profit}
        });
    }
    chart.children[0].props["data"] = JSONValue(jsonData);

    // ---------- Table ----------
    auto makeTable = [&](const std::vector<DayData>& rows) -> Node {
        std::vector<Node> tableRows;

        // Заголовок
        tableRows.push_back(tr({
            th({ text("Day") }),
            th({ text("Commission") }),
            th({ text("P/L") })
        }));

        // Динамические строки (через лямбду)
        for (const auto& row : rows) {
            tableRows.push_back(tr({
                td({ text(row.day) }),
                td({ text(std::to_string(row.commission)) }),
                td({ text(std::to_string(row.profit)) })
            }));
        }

        return table(tableRows, props({{"className", "data-table"}}));
    };

    Node report = div({
        h1({ text("Profit and Commission Report") }),
        chart,
        makeTable(data)
    }, props({{"className", "report"}}));


    to_json(report, res, aloc);
}


