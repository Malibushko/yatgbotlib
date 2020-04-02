#pragma once
#include <memory>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "cpp-httplib/out/httplib.h"
#undef CPPHTTPLIB_OPENSSL_SUPPORT
#include "utility/utility.h"

namespace telegram {

class NetworkManager {
  httplib::Client cli;
  enum class http {
    informational = 100,
    success = 200,
    redirection = 300,
    client_error = 400,
    server_error = 500
  };
public:
  NetworkManager(const char *host);
  NetworkManager(const NetworkManager &) = delete;
  NetworkManager(NetworkManager &&) = delete;
  NetworkManager &operator=(const NetworkManager &) = delete;
  NetworkManager &operator=(NetworkManager &&) = delete;
  static uint32_t ipv4(const std::string& s);

  std::shared_ptr<httplib::Response>
  post(const std::string &url, const httplib::Headers &headers = {},
       const std::string &body = "{}",
       const std::string &content_type =
           utility::toString(ContentTypes::application_json).data());

  std::shared_ptr<httplib::Response>
  post(const std::string &url, const httplib::MultipartFormDataItems &items);
};

} // namespace telegram
