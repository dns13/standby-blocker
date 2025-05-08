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

auto check_auth_token(const crow::request& req, const std::string& auth_token) -> bool {
	if (auth_token.empty()) {
		return true;
	}

	const auto sent_header = req.get_header_value("Authorization");
	if (sent_header.size() < 7) {
		return false;
	}

	const auto sent_token = sent_header.substr(7); 

	spdlog::debug("Authorization token: {}", sent_token);

	return sent_token == auth_token;
}

auto main(int argc, char **argv) -> int {
	unsigned int port = 8080u;
	std::string auth_token{};
	bool verbose = false;
	
	{
		cxxopts::Options options(argv[0], "Standby Blocker");

		// clang-format off
		options.add_options()
			("h,help", "Print usage")
			("p,port", "HTTP port", cxxopts::value<unsigned int>()->default_value(std::to_string(port)))
			("t,token", "Authentication token", cxxopts::value<std::string>())
			("v,verbose", "verbose output")
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

		if (result.count("token") != 0) {
			auth_token = result["token"].as<std::string>();
		}

		if (result.count("verbose") == 1u) {
			spdlog::set_level(spdlog::level::debug);
			spdlog::info("verbose output enabled");
			verbose = true;
		}
	}

	if (!auth_token.empty()) {
		spdlog::info("Using authentication token: \"{}\"", auth_token);
	} else {
		spdlog::info("No authentication token provided, starting without authentication");
	}

	spdlog::info("Listening on 0.0.0.0:{}", port);

	crow::SimpleApp app{};
	app.loglevel(verbose ? crow::LogLevel::Debug : crow::LogLevel::Warning).port(port);

	auto standby_blocked{false};

	CROW_ROUTE(app, "/")([&standby_blocked, auth_token](const crow::request& req){
		if (!check_auth_token(req, auth_token)) {
			spdlog::warn("Unauthorized access attempt to / endpoint from {}", req.remote_ip_address);
			return crow::response(401, "Unauthorized");
		}

		return crow::response(200, print_output(standby_blocked));
	});

	CROW_ROUTE(app, "/enable")([&standby_blocked, auth_token](const crow::request& req){
		if (!check_auth_token(req, auth_token)) {
			spdlog::warn("Unauthorized access attempt to /enable endpoint from {}", req.remote_ip_address);
			return crow::response(401, "Unauthorized");
		}

		block_standby();
		standby_blocked = true;
		return crow::response(200, print_output(standby_blocked));
	});

	CROW_ROUTE(app, "/disable")([&standby_blocked, auth_token](const crow::request& req){
		if (!check_auth_token(req, auth_token)) {
			spdlog::warn("Unauthorized access attempt to /disable endpoint from {}", req.remote_ip_address);
			return crow::response(401, "Unauthorized");
		}

		unblock_standby();
		standby_blocked = false;
		return crow::response(200, print_output(standby_blocked));
	});

	app.run();
}