#include "PluginInterface.hpp"

#include <iomanip>

extern "C" void AboutReport(rapidjson::Value& request,
                            rapidjson::Value& response,
                            rapidjson::Document::AllocatorType& allocator,
                            CServerInterface* server) {
    response.AddMember("version", 1, allocator);
    response.AddMember("name", Value().SetString("Demo report", allocator), allocator);
    response.AddMember("description",
    Value().SetString("Development only!",
             allocator), allocator);
    response.AddMember("type", REPORT_RANGE_GROUP_TYPE, allocator);
}

extern "C" void DestroyReport() {}

extern "C" void CreateReport(rapidjson::Value& request,
                             rapidjson::Value& response,
                             rapidjson::Document::AllocatorType& allocator,
                             CServerInterface* server) {
    std::string group_mask;
    int from;
    int to;
    if (request.HasMember("group") && request["group"].IsString()) {
        group_mask = request["group"].GetString();
    }
    if (request.HasMember("from") && request["from"].IsNumber()) {
        from = request["from"].GetInt();
    }
    if (request.HasMember("to") && request["to"].IsNumber()) {
        to = request["to"].GetInt();
    }

    std::vector<TradeRecord> trades_vector;

    try {
        server->GetCloseTradesByGroup(group_mask, from, to, &trades_vector);
    } catch (const std::exception& e) {
        std::cerr << "[TradesHistoryReportInterface]: " << e.what() << std::endl;
    }

    JSONObject order_column_props = {
        {"name", JSONValue("ORDER")},
        {"filter", JSONObject{{"type", JSONValue("search")}}},
        {"export", JSONValue(true)},
        {"sort", JSONValue(true)}
    };

    JSONObject login_column_pros = {
        {"name", JSONValue("LOGIN")},
        {"filter", JSONObject{{"type", JSONValue("search")}}},
        {"export", JSONValue(true)},
        {"sort", JSONValue(true)}
    };

    JSONObject open_time_column_props = {
        {"name", JSONValue("OPEN_TIME")},
        {"filter", JSONObject{{"type", JSONValue("search")}}},
        {"export", JSONValue(true)},
        {"sort", JSONValue(true)}
    };

    JSONObject close_time_column_props = {
        {"name", JSONValue("CLOSE_TIME")},
        {"filter", JSONObject{{"type", JSONValue("search")}}},
        {"export", JSONValue(true)},
        {"sort", JSONValue(true)}
    };

    JSONArray table_data;
    for (const auto& trade : trades_vector) {
        table_data.emplace_back(JSONObject{
        {"order", JSONValue(std::to_string(trade.order))},
        {"login", JSONValue(std::to_string(trade.login))},
        {"open_time", JSONValue(utils::FormatTimestampToString(trade.open_time))},
        {"close_time", JSONValue(utils::FormatTimestampToString(trade.close_time))},
        });
    }

    JSONObject table_props = props({
        {"name", "DemoReport"},
        {"idCol", "order"},
        {"data", table_data},
        {"orderBy", JSONArray{JSONValue("order"), JSONValue("DESC")}},
        {"showExportBtn", JSONValue(true)},
        {"showRefreshBtn", JSONValue(false)},
        {"showBookmarksBtn", JSONValue(false)},
        {"structure", JSONObject{
            {"login", (JSONValue(login_column_pros))},
            {"order", JSONValue(order_column_props)},
            {"open_time", JSONValue(open_time_column_props)},
            {"close_time", JSONValue(close_time_column_props)}
        }},
    });

    Node table_node = Table({}, table_props);

    // Total report
    const Node report = div({
        h1({text("Trades History Report")}),
        table_node
    });

    utils::CreateUI(report, response, allocator);
}
