#pragma once
#include <string>
#include "rapidjson/document.h"

namespace telegram {

class QueryBuilder {
  rapidjson::Document doc{};

public:
  QueryBuilder() = default;
  QueryBuilder(rapidjson::Document::AllocatorType &allocator);

  template <class T>
  friend QueryBuilder &operator<<(QueryBuilder &builder,
                                  const std::pair<std::string_view, T> &pair);
  std::string getQuery() const noexcept;
  const rapidjson::Document &getDocument() const noexcept;
};

template <class T>
QueryBuilder &operator<<(QueryBuilder &builder,
                         const std::pair<std::string_view, T> &pair) {
  if (!builder.doc.IsObject())
    builder.doc.SetObject();

  rapidjson::Value val;
  valueToJson(pair.second, val, builder.doc.GetAllocator());
  if (!val.IsNull()) {
    rapidjson::Value name(rapidjson::kStringType);
    name.SetString(pair.first.data(), pair.first.size(),
                   builder.doc.GetAllocator());
    builder.doc.AddMember(name, val, builder.doc.GetAllocator());
  }
  return builder;
}
} // namespace telegram
