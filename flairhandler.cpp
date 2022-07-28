#include "flairhandler.h"

#include <redditcpp/api.h>

#include <spdlog/spdlog.h>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

FlairHandler::FlairHandler(std::shared_ptr<reddit::Api> api, std::string subreddit, spdlog::level::level_enum log_level)
	: _api(std::move(api))
	, _subreddit(std::move(subreddit)) {
	spdlog::set_level(log_level);
};

int FlairHandler::get_flairlistcsv(const std::string& filename) {
	std::ofstream file(filename, std::ios::trunc);
	file << "User,Flair Text,CSS Class\n";

	reddit::FlairListInput fl_input;
	fl_input.limit(1000);
	std::string after;
	int counter = 0;

	while(true) {
		fl_input.after(after);
		reddit::FlairListResponse fl_resp = _api->modflair(_subreddit).flairlist(fl_input);

		for(const auto& itr : fl_resp.entries) {
			const std::string current_fl_text = itr.flairtext;

			if(current_fl_text.empty() && itr.cssclass.empty()) {
				continue;
			}
			
			file << fmt::format("{},{},\"{}\"\n", itr.user, current_fl_text, itr.cssclass);
			spdlog::debug(" -[{}] {} - {} - {}", ++counter, itr.user, current_fl_text, itr.cssclass);
		}
		after = fl_resp.next;
		
		if(after.empty()) {
			break;
		}
	}

	file.close();
	return counter;
}

void FlairHandler::replace_flairtext(const std::unordered_map<std::string, std::string> flairmap) {
	spdlog::info("Replace Flairtext Task Started");

	const std::string filename = "flairs.txt";
	int counted = save_flairtext(filename, false);

	spdlog::info("List Retrieved, {} Flairs Counted", counted);

	std::ifstream file(filename);
	if(!file.is_open()) {
		spdlog::critical("Could not open the Flairlist file");
		return;
	}
	std::ofstream output_file("output.txt", std::ios::trunc);

	std::vector<reddit::FlairListResponse::Entry> flair_csv;
	flair_csv.reserve(100);
	std::string user, css_sink;
	int iterator_counter = 0;
	int entry_counter = 0;

	auto api_call = [&]() {
		auto resp = _api->modflair(_subreddit).flaircsv(flair_csv);		
		
		for(const auto& itr : resp.entries) {
			if(!itr.ok) {
				spdlog::error("{} Not Okay - {}", entry_counter, itr.status);
			}
			output_file << ++entry_counter << " " << itr.status << "\n";
		}
		flair_csv.clear();
		iterator_counter = 0;
		spdlog::info("{}/{} Completed", entry_counter, counted);
	};

	int row_counter = 0;
	while(std::getline(file, user)) {
		if(iterator_counter >= 100) {
			api_call();
		}
		reddit::FlairListResponse::Entry entry;
		entry.user = user;
		std::getline(file, entry.flairtext);

		const auto itr = flairmap.find(entry.flairtext);
		if(itr != flairmap.end()) {
			entry.flairtext = itr->second;
		}

		std::getline(file, entry.cssclass);

		flair_csv.emplace_back(entry);
		
		++iterator_counter;
		spdlog::debug(" -[{}] {} - {} - {}", ++row_counter, entry.user, entry.flairtext, entry.cssclass);
	}
	api_call();

	output_file.close();
	
	spdlog::info("Replace Flairtext Task Complete");
}

void FlairHandler::backup_flairs() {
	spdlog::info("Backup Task Started");

	int counted = save_flairtext("backup.txt", false);

	spdlog::info("Backup Task Complete, {} Flairs Counted", counted);
}

void FlairHandler::clear_user_css_classes() {
	spdlog::info("Delete Flair CSS Classes Task Started");

	const std::string filename = "flairs.txt";
	int counted = save_flairtext(filename, true);

	spdlog::info("List Retrieved, {} Flairs Counted", counted);

	std::ifstream file(filename);
	if(!file.is_open()) {
		spdlog::critical("Could not open the Flairlist file");
		return;
	}
	std::ofstream output_file("output.txt", std::ios::trunc);

	std::vector<reddit::FlairListResponse::Entry> flair_csv;
	flair_csv.reserve(100);
	std::string user, css_sink;
	int iterator_counter = 0;
	int entry_counter = 0;

	auto api_call = [&]() {
		auto resp = _api->modflair(_subreddit).flaircsv(flair_csv);		
		
		for(const auto& itr : resp.entries) {
			if(!itr.ok) {
				spdlog::error("{} Not Okay - {}", entry_counter, itr.status);
			}
			output_file << ++entry_counter << " " << itr.status << "\n";
		}
		flair_csv.clear();
		iterator_counter = 0;
		spdlog::info("{}/{} Completed", entry_counter, counted);
	};

	int row_counter = 0;
	while(std::getline(file, user)) {
		if(iterator_counter >= 100) {
			api_call();
		}
		reddit::FlairListResponse::Entry entry;
		entry.user = user;
		std::getline(file, entry.flairtext);
		std::getline(file, css_sink);

		flair_csv.emplace_back(entry);
		
		++iterator_counter;
		spdlog::debug(" -[{}] {} - {} - {}", ++row_counter, entry.user, entry.flairtext, entry.cssclass);
	}
	api_call();

	output_file.close();
	
	spdlog::info("Delete Flair CSS Classes Task Complete");
}

int FlairHandler::save_flairtext(const std::string& filename, bool css_class_empty_only) {
	reddit::FlairListInput fl_input;
	fl_input.limit(1000);
	std::string after;
	int counter = 0;

	std::ofstream file(filename, std::ios::trunc);

	while(true) {
		fl_input.after(after);
		reddit::FlairListResponse fl_resp = _api->modflair(_subreddit).flairlist(fl_input);

		for(const auto& itr : fl_resp.entries) {
			const std::string current_fl_text = itr.flairtext;

			if(current_fl_text.empty() && itr.cssclass.empty()) {
				continue;
			}
			if(css_class_empty_only && itr.cssclass.empty()) {
				continue;
			}
			
			file << itr.user << "\n" << current_fl_text  << "\n" << itr.cssclass << "\n";
			
			spdlog::debug(" -[{}] {} - {} - {}", ++counter, itr.user, current_fl_text, itr.cssclass);
		}
		after = fl_resp.next;
		
		if(after.empty()) {
			break;
		}
	}

	file.close();
	return counter;
}