#pragma once
#include <string>
#include "json_parser.h"
#include "rapidjson/document.h"
#include "utility/utility.h"
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
  if constexpr (std::is_enum_v<traits::optional_or_value<T>>) {
      std::string_view mappedValue;
      if constexpr (traits::is_optional_v<std::decay_t<T>>)
        mappedValue = utility::toString(pair.second.value_or(traits::optional_or_value<T>{}));
      else
        mappedValue = utility::toString(pair.second);

      val.SetString(mappedValue.data(),mappedValue.size());
  }
  else
    JsonParser::i().valueToJson(pair.second, val, builder.doc.GetAllocator());

  if (!val.IsNull()) {
    rapidjson::Value name(rapidjson::kStringType);
    name.SetString(pair.first.data(), pair.first.size(),
                   builder.doc.GetAllocator());
    builder.doc.AddMember(name, val, builder.doc.GetAllocator());
  }
  return builder;
}
} // namespace telegram
