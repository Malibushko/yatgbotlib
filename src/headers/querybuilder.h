#pragma once
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
namespace telegram {

class QueryBuilder {
    rapidjson::Document doc;
public:
    QueryBuilder() = default;
    QueryBuilder(rapidjson::Document::AllocatorType & allocator) : doc{&allocator} {}

    template<class T>
    friend QueryBuilder& operator<<(QueryBuilder& builder,const std::pair<std::string_view,T>& pair) {
        if (!builder.doc.IsObject())
            builder.doc.SetObject();

        rapidjson::Value val;
        valueToJson(pair.second,val,builder.doc.GetAllocator());
        if (!val.IsNull()) {
            rapidjson::Value name(rapidjson::kStringType);
            name.SetString(pair.first.data(),pair.first.size(),builder.doc.GetAllocator());
            builder.doc.AddMember(name,val,builder.doc.GetAllocator());
        }
        return builder;
    }
    std::string getQuery() const {
        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
        doc.Accept(writer);
        return buff.GetString();
    }
    const rapidjson::Document& getDoc() const {
        return doc;
    }
};
}
