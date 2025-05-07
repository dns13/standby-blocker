#define NOMINMAX // Prevent windows.h from defining min and max macros
#include <windows.h>

#include "crow.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "cxxopts.hpp"

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

auto main(int argc, char **argv) -> int {
	unsigned int port = 8080u;
	
	{
		cxxopts::Options options(argv[0], "Standby Blocker");

		// clang-format off
		options.add_options()
			("h,help", "Print usage")
			("p,port", "HTTP port", cxxopts::value<unsigned int>()->default_value(std::to_string(port)))
		;
		// clang-format on

		auto result = options.parse(argc, argv);
		
		if (result.count("help") != 0) {
			spdlog::info(options.help());
			return EXIT_SUCCESS;
		}

		if (result.count("port") != 0) {
			port = result["port"].as<unsigned int>();
		}
	}

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

	app.port(port).run();
}