#ifndef FLAIR_HANDLER_H
#define FLAIR_HANDLER_H

#include <redditcpp/api.h>

#include <spdlog/spdlog.h>
#include <memory>
#include <unordered_map>
#include <vector>

class FlairHandler {
public:
    FlairHandler(std::shared_ptr<reddit::Api> api, std::string subreddit, spdlog::level::level_enum log_level);
    int get_flairlistcsv(const std::string& filename);
    void replace_flairtext(const std::unordered_map<std::string, std::string> flairmap);
    void backup_flairs();
    void clear_user_css_classes();
private:
    std::shared_ptr<reddit::Api> _api;
    std::string _subreddit;
    int save_flairtext(const std::string& filename, bool css_class_empty_only = false);
};

#endif //FLAIR_HANDLER_H