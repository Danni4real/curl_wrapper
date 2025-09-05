//
// Created by dan on 25-2-7.
//

#include <cstddef>
#include <fstream>
#include <iostream>

#include <curl/curl.h>

#include "curl_tool.h"
#include "ThreadLog.h"

void (*g_download_progress_callback)(int) = nullptr;

int progress_callback(double total_to_download, double now_downloaded, double total_to_upload, double now_uploaded) {
    if (total_to_download > 0) {
        int percentage = static_cast<int>((now_downloaded / total_to_download) * 100);

        if (g_download_progress_callback) {
            g_download_progress_callback(percentage);
        }
    }
    return 0;
}

bool CurlTool::reachable(const std::string &url) {
    LOG_CALL(url);

    if (!m_curl_handle) {
        LOG_ERROR("not login yet!!!");
        return false;
    }

    curl_easy_setopt(m_curl_handle, CURLOPT_URL, url.c_str());

    curl_easy_setopt(m_curl_handle, CURLOPT_NOBODY, 1L); // only request head info, not download
    CURLcode res = curl_easy_perform(m_curl_handle);
    curl_easy_setopt(m_curl_handle, CURLOPT_NOBODY, 0L);

    return res == CURLE_OK;
}

bool CurlTool::download(const std::string &file_url, const std::string &save_path) {
    LOG_CALL(file_url, save_path);

    if (!m_curl_handle) {
        LOG_ERROR("not login yet!!!");
        return false;
    }

    FILE *fp = fopen(save_path.c_str(), "wb");
    if (!fp) {
        LOG_ERROR("fopen failed!!!");
        return false;
    }

    curl_easy_setopt(m_curl_handle, CURLOPT_URL, file_url.c_str());
    curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, nullptr);
    curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, fp);

    CURLcode res = curl_easy_perform(m_curl_handle);

    fclose(fp);

    if (res != CURLE_OK) {
        LOG_ERROR("download failed: %s", curl_easy_strerror(res));
        return false;
    }
    return true;
}

bool CurlTool::login(const std::string &usr, const std::string &passwd, const std::string &server_url) {
    LOG_CALL(usr, passwd, server_url);
    logout(); // logout last account if needed

    m_curl_handle = curl_easy_init();
    if (!m_curl_handle) {
        LOG_ERROR("curl_easy_init failed!!!");
        return false;
    }

    curl_easy_setopt(m_curl_handle, CURLOPT_USERPWD, (usr + ":" + passwd).c_str());
    curl_easy_setopt(m_curl_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(m_curl_handle, CURLOPT_NOPROGRESS, 0L);

    if (!reachable(server_url)) {
        LOG_ERROR("%s not reachable!!!", server_url.c_str());
        logout();
        return false;
    }
    return true;
}

void CurlTool::logout() {
    LOG_CALL_0();

    if (m_curl_handle) {
        curl_easy_cleanup(m_curl_handle);
        m_curl_handle = nullptr;
    }
}

bool CurlTool::set_download_progress_callback(void (*callback)(int)) {
    LOG_CALL_0();

    if (!m_curl_handle) {
        LOG_ERROR("not login yet!!!");
        return false;
    }
    g_download_progress_callback = callback;
    return true;
}
/* for test
void print_download_progress(int percentage) {
    std::cout << "Download progress: " << percentage << "%\r";
    std::cout.flush();
}

int main() {
    CurlTool curl_tool;

    if (!curl_tool.login("dan", "1", "ftp://localhost/")) {
        std::cerr << "login failed!!!" << std::endl;
        goto END;
    }

    if (!curl_tool.reachable("ftp://localhost/big_file.iso")) {
        std::cerr << "file url not reachable!!!" << std::endl;
        goto END;
    }

    // optional
    if (!curl_tool.set_download_progress_callback(print_download_progress)) {
        std::cerr << "set_download_progress_callback failed!!!" << std::endl;
        goto END;
    }

    if (!curl_tool.download("ftp://localhost/big_file.iso", "/home/dan/projects/big_file.iso")) {
        std::cerr << "download failed!!!" << std::endl;
    }

END:
    curl_tool.logout();
    return 0;
}
*/
