#include "PluginInterface.hpp"

#include <iomanip>

extern "C" void AboutReport(rapidjson::Value& request,
                            rapidjson::Value& response,
                            rapidjson::Document::AllocatorType& allocator,
                            CServerInterface* server) {
    response.AddMember("version", 1, allocator);
    response.AddMember("name", Value().SetString("Demo report", allocator), allocator);
    response.AddMember("description",
    Value().SetString("Development only",
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
        std::cerr << "[DemoReportInterface]: " << e.what() << std::endl;
    }

    TableBuilder table_builder("Demo Report");

    table_builder.SetIdColumn("order");
    table_builder.SetOrderBy("order", "DESC");
    table_builder.EnableRefreshButton(false);
    table_builder.EnableBookmarksButton(false);
    table_builder.EnableExportButton(true);

    table_builder.AddColumn({"order", "ORDER"});
    table_builder.AddColumn({"login", "LOGIN"});
    table_builder.AddColumn({"open_time", "OPEN_TIME"});
    table_builder.AddColumn({"close_time", "CLOSE_TIME"});

    for (const auto& trade : trades_vector) {
        table_builder.AddRow({
            {"order", std::to_string(trade.order)},
            {"login", std::to_string(trade.login)},
            {"open_time", utils::FormatTimestampToString(trade.open_time)},
            {"close_time", utils::FormatTimestampToString(trade.close_time)}
        });
    }

    const JSONObject table_props = table_builder.CreateTableProps();

    Node table_node = Table({}, table_props);

    // Total report
    const Node report = div({
        h1({text("Demo Report")}),
        table_node
    });

    utils::CreateUI(report, response, allocator);
}
