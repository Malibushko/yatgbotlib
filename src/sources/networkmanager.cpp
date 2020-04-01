#include "headers/networkmanager.h"
using namespace telegram;

NetworkManager::NetworkManager(const char *host) : cli{host} {
  cli.set_follow_location(true);
}

std::shared_ptr<httplib::Response>
NetworkManager::post(const std::string &url, const httplib::Headers &headers,
                     const std::string &body, const std::string &content_type) {
  auto reply = cli.Post(url.data(), headers, body, content_type.data());
  if (reply && reply->status) {
    if (reply->status == std::clamp(reply->status,
                                    static_cast<int>(http::redirection),
                                    static_cast<int>(http::client_error))) {
      reply = cli.Post(url.data(), headers, body, content_type.data());
    }
    return reply;
  } else
    return {};
}

std::shared_ptr<httplib::Response>
NetworkManager::post(const std::string &url,
                     const httplib::MultipartFormDataItems &items) {
  auto reply = cli.Post(url.data(), items);
  if (reply && reply->status) {
    if (reply->status == std::clamp(reply->status,
                                    static_cast<int>(http::redirection),
                                    static_cast<int>(http::client_error))) {
      reply = cli.Post(url.data(), items);
    }
    return reply;
  } else
    return {};
}
