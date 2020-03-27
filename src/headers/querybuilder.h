#pragma once
#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "apimanager.h"
#include "utility/utility.h"
namespace telegram {

class query_builder {
    rapidjson::Document doc;
public:
    query_builder() = default;
    query_builder(rapidjson::Document::AllocatorType & allocator) : doc{&allocator} {}

    template<class T>
    friend query_builder& operator<<(query_builder& builder,const std::pair<std::string_view,T>& pair) {
        if (!builder.doc.IsObject())
            builder.doc.SetObject();

        rapidjson::Value val;
        value_to_json(pair.second,val,builder.doc.GetAllocator());
        if (!val.IsNull()) {
            rapidjson::Value name(rapidjson::kStringType);
            name.SetString(pair.first.data(),pair.first.size(),builder.doc.GetAllocator());
            builder.doc.AddMember(name,val,builder.doc.GetAllocator());
        }
        return builder;
    }
    std::string get_query() const noexcept {
        return utility::to_string(doc);
    }
    const rapidjson::Document& get_document() const noexcept {
        return doc;
    }
};
}
