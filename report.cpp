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

    // Пример исходных данных
    struct DayData { std::string day; double usage; double peak; };
    std::vector<DayData> data = {
        {"Mon", 120.5, 80.2},
        {"Tue", 150.3, 95.6},
        {"Wed", 130.1, 88.0},
        {"Thu", 170.2, 110.3},
        {"Fri", 160.8, 102.4}
    };

    // ---------- График (Recharts LineChart) ----------
    Node chart = ResponsiveContainer({
        LineChart({
            XAxis({}, props({{"dataKey", "day"}})),
            YAxis(),
            Tooltip(),
            Legend(),
            Line({}, props({
                {"type", "monotone"},
                {"dataKey", "usage"},
                {"stroke", "#8884d8"}
            })),
            Line({}, props({
                {"type", "monotone"},
                {"dataKey", "peak"},
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
            {"usage", d.usage},
            {"peak", d.peak}
        });
    }
    chart.children[0].props["data"] = JSONValue(jsonData);

    // ---------- Таблица (с генерацией через лямбду) ----------
    auto makeTable = [&](const std::vector<DayData>& rows) -> Node {
        std::vector<Node> tableRows;

        // Заголовок
        tableRows.push_back(tr({
            th({ text("Day") }),
            th({ text("Usage (kWh)") }),
            th({ text("Peak Load (kW)") })
        }));

        // Динамические строки (через лямбду)
        for (const auto& row : rows) {
            tableRows.push_back(tr({
                td({ text(row.day) }),
                td({ text(std::to_string(row.usage)) }),
                td({ text(std::to_string(row.peak)) })
            }));
        }

        return table(tableRows, props({{"className", "data-table"}}));
    };

    Node report = div({
        h1({ text("Weekly Energy Report") }),
        chart,
        makeTable(data)
    }, props({{"className", "report"}}));

    to_json(report, res, aloc);
}


// ---------- basic HTML tags ----------

TAG(div)
TAG(span)
TAG(p)
TAG(h1)
TAG(h2)
TAG(h3)
TAG(h4)
TAG(h5)
TAG(h6)
TAG(ul)
TAG(ol)
TAG(li)
TAG(table)
TAG(thead)
TAG(tbody)
TAG(tr)
TAG(td)
TAG(th)
TAG(img)
TAG(a)
TAG(button)
TAG(input)
TAG(label)
TAG(form)
TAG(select)
TAG(option)
TAG(section)
TAG(article)
TAG(header)
TAG(footer)
TAG(main)
TAG(nav)
TAG(svg)
TAG(path)
TAG(rect)
TAG(circle)
TAG(line)
TAG(g)

// ---------- Recharts tags ----------

TAG(ResponsiveContainer)
TAG(LineChart)
TAG(BarChart)
TAG(PieChart)
TAG(AreaChart)
TAG(XAxis)
TAG(YAxis)
TAG(ZAxis)
TAG(Tooltip)
TAG(Legend)
TAG(Line)
TAG(Bar)
TAG(Pie)
TAG(Area)
TAG(Cell)
TAG(CartesianGrid)
TAG(Brush)
TAG(ReferenceLine)
TAG(ReferenceDot)
TAG(ComposedChart)
TAG(ScatterChart)
TAG(Scatter)
TAG(RadarChart)
TAG(Radar)
TAG(PolarGrid)
TAG(PolarAngleAxis)
TAG(PolarRadiusAxis)