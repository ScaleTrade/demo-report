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
    // Структура накопления итогов
    struct Total {
        double volume;
        double commission;
        double profit;
        std::string currency;
    };

    std::unordered_map<std::string, Total> totals_map;

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
    std::vector<GroupRecord> groups_vector;

    try {
        server->GetCloseTradesByGroup(group_mask, from, to, &trades_vector);
        server->GetAllGroups(&groups_vector);
    } catch (const std::exception& e) {
        std::cerr << "[TradesHistoryReportInterface]: " << e.what() << std::endl;
    }

    JSONObject id_column_props = {
        {"name", JSONValue("ID")},
        {"filter", JSONObject{{"type", JSONValue("search")}}}
    };

    JSONObject order_column_props = {
        {"name", JSONValue("ORDER")},
        {"filter", JSONObject{{"type", JSONValue("search")}}}
    };

    JSONArray table_data;
    for (size_t i = 0; i < trades_vector.size(); i++) {
        const auto& trade = trades_vector[i];

        table_data.emplace_back(JSONObject{
            {"id", JSONValue(std::to_string(i))},
            {"order", JSONValue(std::to_string(trade.order))}
        });
    }

    JSONObject table_props = props({
        {"name", "MarginCallTable"},
        {"idCol", "id"},
        {"data", table_data},
        {"orderBy", JSONArray{JSONValue("id"), JSONValue("DESC")}},
        {"structure", JSONObject{
            {"id", JSONValue(id_column_props)},
            {"order", JSONValue(order_column_props)}
        }}
    });

    Node table = Table({}, table_props);

    // Total report
    const Node report = div({
        h1({text("Trades History Report")}),
        table
    });

    utils::CreateUI(report, response, allocator);
}
