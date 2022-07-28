#include "flairhandler.h"

#include <redditcpp/api.h>

#include <spdlog/spdlog.h>
#include <fstream>

int main() {
	const std::string client_id = "client_id";
	const std::string client_secret = "client_secret";
	const std::string redirect_uri = "http://localhost:8080";
    const std::string scope = "modflair";
	const std::string user_agent = "flair tool by /u/whytro";

    reddit::AuthInfo oa2info {
        client_id, client_secret, redirect_uri, scope, "", user_agent
    };

    reddit::Api reddit_api(oa2info, spdlog::level::debug);
    const std::string token = reddit_api.browser_get_token();
    reddit_api.authenticate(token);

	FlairHandler fl_handler(std::make_shared<reddit::Api>(reddit_api), "subreddit here", spdlog::level::debug);
	fl_handler.backup_flairs();
	fl_handler.clear_user_css_classes();
}
