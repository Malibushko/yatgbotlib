#pragma once
#include <memory>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "cpp-httplib/httplib.h"
#undef CPPHTTPLIB_OPENSSL_SUPPORT

#include "utility/utility.h"

namespace telegram {
class network_manager {
    std::unique_ptr<httplib::Client> cli;
    network_manager(const char * host)
        : cli{std::make_unique<httplib::Client>(host)} {}
    enum class http_code {informational = 100,
                          success = 200,
                          redirection = 300,
                          client_error = 400,
                          server_error = 500};
    constexpr static std::string_view host_addr = "api.telegram.org";
public:
    static network_manager &i() {
        static network_manager inst{host_addr.data()};
        inst.cli->set_follow_location(true);
        return inst;
    }
    static std::string_view get_host_address() {
        return host_addr;
    }
    std::shared_ptr<httplib::Response>
    post(const std::string &url, const httplib::Headers &headers = {},
         const std::string &body = {"{}"},
         const std::string &content_type =
            utility::content_type.at(utility::content_types::application_json)) {
        auto reply = i().cli->Post(url.data(), headers, body, content_type.data());
        if (reply && reply->status) {
            if (reply->status == std::clamp(reply->status,
                                    static_cast<int>(http_code::redirection),
                                    static_cast<int>(http_code::client_error))) {
                reply = i().cli->Post(url.data(), headers, body, content_type.data());
            }
            return reply;
        } else
            return {};
    }
    std::shared_ptr<httplib::Response>
    post(const std::string &url, const httplib::MultipartFormDataItems &items) {
        auto reply = i().cli->Post(url.data(), items);
        if (reply && reply->status) {
            if (reply->status == std::clamp(reply->status,
                                            static_cast<int>(http_code::redirection),
                                            static_cast<int>(http_code::client_error))) {
                reply = i().cli->Post(url.data(), items);
            }
            return reply;
        } else
            return {};
    }
    network_manager(const network_manager &) = delete;
    network_manager(network_manager &&) = delete;
    network_manager &operator=(const network_manager &) = delete;
    network_manager &operator=(network_manager &&) = delete;
};
}
