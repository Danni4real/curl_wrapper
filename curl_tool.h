//
// Created by dan on 25-2-7.
//

#ifndef CURL_TOOL_H
#define CURL_TOOL_H

#include <string>
#include <curl/curl.h>

class CurlTool {
public:
    bool reachable(const std::string &url);

    bool set_download_progress_callback(void (*callback)(int/*0~100*/));

    bool download(const std::string &file_url, const std::string &save_path);

    void logout();
    bool login(const std::string &usr, const std::string &passwd, const std::string &server_url);

private:
    CURL *m_curl_handle{nullptr};
};

#endif //CURL_TOOL_H
