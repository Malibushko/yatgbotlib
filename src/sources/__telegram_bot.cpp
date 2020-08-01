#include "telegram_bot.h"
#include "querybuilder.h"
#include "apimanager.h"
#include "utility.h"

namespace telegram {

Bot::Bot(const std::string &token, std::size_t thread_number) noexcept
    : api{std::make_unique<ApiManager>("https://api.telegram.org/bot" + token +
                                       '/')},
      updater(std::max(thread_number,std::size_t{2})) {}

void Bot::onUpdate(UpdateCallback &&cb) {
  updater.setUpdateCallback(std::move(cb));
}

void Bot::onChosenInlineResult(std::string_view cmd,
                         ChosenInlineResultCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onQueryCallback(std::string_view cmd, QueryCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onMessage(std::string_view cmd, MessageCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onPreCheckoutQuery(std::string_view cmd,PreCheckoutQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}

void Bot::onInlineQuery(std::string_view cmd, InlineQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}

void Bot::onShippingQuery(std::string_view cmd,ShippingQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}
void Bot::stopSequence(int64_t user_id) { updater.removeSequence(user_id); }

void Bot::start(std::optional<int64_t> timeout, std::optional<int64_t> offset, std::optional<int8_t> limit,
                std::optional<std::vector<std::string_view>> allowed_updates) {
  if (auto &&[webhook, Error] = getWebhookInfo();
      Error || webhook.url.size() || webhookSet) {
    if (Error)
      utility::Logger::warn(Error.value());
    utility::Logger::critical("You must remove webhook before using long polling method.");
    return;
  }
  stopPolling = false;
  updater.setOffset(offset.value_or(0));
  while (!stopPolling) {
    updater.routeCallback(getUpdatesRawJson(updater.getOffset(), limit, timeout,
                                            allowed_updates));
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout.value_or(1)));
  }
}
std::string Bot::getUpdatesRawJson(
    std::optional<uint32_t> offset, std::optional<uint8_t> limit,
    std::optional<uint32_t> timeout,
    std::optional<std::vector<std::string_view>> allowed_updates) {
  QueryBuilder builder;
  builder << make_named_pair(offset) << make_named_pair(limit)
          << make_named_pair(timeout) << make_named_pair(allowed_updates);
  return api->ApiCallRaw("getUpdates", builder);
}
void Bot::stop() { stopPolling = true; }

std::pair<Message, opt_error> Bot::reply(const Message &msg,
                                         const std::string &message) const {
  return sendMessage(msg.chat.id, message, {}, {}, {}, msg.message_id);
}

bool Bot::setWebhookServer(const std::string &url, uint16_t port,
                           const std::string &cert_path,
                           const std::string &key_path) {

  webhookSet = true;

  // Send Telegram Bot Api request first
  auto [result, Error] =
      setWebhook(url + ':' + std::to_string(port), cert_path);
 // if result is not true
 if (!result)
    return false;
  // or if there is error
  if (Error) {
    utility::Logger::warn(Error->toString());
    return false;
  }

  httplib::SSLServer server(cert_path.data(), key_path.data());
  if (!server.is_valid()) {
    utility::Logger::critical("Server is not valid. Check certificate paths.");
    return false;
  }
  // one rule for routing
  server.Post("/", [&](const httplib::Request &req, httplib::Response &) {
    if (auto host = req.headers.find("REMOTE_ADDR");
        host != req.headers.end()) {
      uint32_t host_ip = NetworkManager::ipv4(host->second.data());
      // check if address is in one of two telegram subnets (filtering)
      if (host_ip != std::clamp(host_ip,
                                utility::telegram_first_subnet_range_begin,
                                utility::telegram_second_subned_range_end) &&
          host_ip != std::clamp(host_ip,
                                utility::teleram_second_subnet_range_begin,
                                utility::telegram_second_subned_range_end)) {
        utility::Logger::info("Request from unknown subnet", host_ip);
        return;
      }
    } else {
      utility::Logger::warn("Cannot resolve host ip");
      return;
    }
    updater.routeCallback(req.body);
  });
  return server.listen("0.0.0.0", port);
}

// --------------------------- AUTOGENERAGED CODE ------------------------------

