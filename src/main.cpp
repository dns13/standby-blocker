#include <windows.h>

#include "crow.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

auto block_standby() -> void {
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
    spdlog::info("Standby blocked");
}

auto unblock_standby() -> void {
    SetThreadExecutionState(ES_CONTINUOUS);
    spdlog::info("Standby unblocked");
}


auto print_output(bool blocked) -> std::string {
    return fmt::format("{}", blocked ? "ENABLED" : "DISABLED");
}

auto main() -> int {
    crow::SimpleApp app;

    auto standby_blocked{false};

    CROW_ROUTE(app, "/")([&standby_blocked](){
        return print_output(standby_blocked);
    });

    CROW_ROUTE(app, "/enable")([&standby_blocked](){
        block_standby();
        standby_blocked = true;
        return print_output(standby_blocked);
    });

    CROW_ROUTE(app, "/disable")([&standby_blocked](){
        unblock_standby();
        standby_blocked = false;
        return print_output(standby_blocked);
    });

    app.port(8080).run();
}