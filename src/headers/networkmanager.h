#pragma once
#include <memory>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../third_party/httplib/httplib.h"
#undef CPPHTTPLIB_OPENSSL_SUPPORT

class NetworkManager {
    std::unique_ptr<httplib::Client> cli;
    NetworkManager(const std::string& host) : cli{std::make_unique<httplib::Client>(host.data())} {}
public:
    static NetworkManager& i() {
        static NetworkManager inst{"api.telegram.org"};
        inst.cli->set_follow_location(true);
        return inst;
    }
    std::shared_ptr<httplib::Response> post(const std::string& url,const httplib::Headers& headers = {},const std::string& body = {"{}"},
                     const std::string& content_type = {"application/json"}) {
        auto reply = i().cli->Post(url.data(),headers,body,content_type.data());
        if (reply && reply->status) {
            if (reply->status > 300 && reply->status < 400) {
                reply = i().cli->Post(url.data(),headers,body,content_type.data());
            }
            return reply;
        } else
            return {};

    }
    std::shared_ptr<httplib::Response> post(const std::string& url,const httplib::MultipartFormDataItems& items) {
        auto reply = i().cli->Post(url.data(),items);
        if (reply && reply->status) {
            if (reply->status > 300 && reply->status < 400) {
                reply = i().cli->Post(url.data(),items);
            }
            return reply;
        } else
            return {};
    }
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager(NetworkManager&&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    NetworkManager& operator=(NetworkManager&&) = delete;
};
