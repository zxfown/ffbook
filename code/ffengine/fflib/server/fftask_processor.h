#ifndef _FF_TASK_PROCESSOR_H_
#define _FF_TASK_PROCESSOR_H_

//线程间传递数据!

#include "rapidjson/document.h"     // rapidjson's DOM-style API                                                                                             
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/stringbuffer.h"   // wrapper of C stream for prettywriter as output

#include "base/smart_ptr.h"

typedef runtime_error       msg_exception_t;
typedef rapidjson::Document json_dom_t;
typedef rapidjson::Value    json_value_t;

namespace ff{

struct ffjson_tool_t{
    ffjson_tool_t():
        allocator(new rapidjson::Document::AllocatorType()),
        jval(new json_dom_t())
    {}
    ffjson_tool_t(const ffjson_tool_t& src):
        allocator(src.allocator),
        jval(src.jval)
    {}
    ffjson_tool_t& operator=(const ffjson_tool_t& src)
    {
        allocator = src.allocator;
        jval = src.jval;
        return *this;
    }
    
    string encode()
    {
        if (false == this->jval->IsObject() && false == this->jval->IsArray())
        {
            return "null";
        }
        typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::Document::AllocatorType> FFStringBuffer;
        FFStringBuffer            str_buff(this->allocator.get());
        rapidjson::Writer<FFStringBuffer> writer(str_buff, this->allocator.get());
        this->jval->Accept(writer);
        string output(str_buff.GetString(), str_buff.Size());
        //printf("output=%s\n", output.c_str());
        return output;
    }
    bool decode(const string& src)
    {
        if (false == this->jval->Parse<0>(src.c_str()).HasParseError())
        {
            return false;
        }
        return true;
    }
    shared_ptr_t<rapidjson::Document::AllocatorType>  allocator;
    shared_ptr_t<json_dom_t>                          jval;
};

class task_processor_i{
public:
    virtual ~task_processor_i(){}
    //! 线程间传递消息
    virtual    void post(const string& task_name, const ffjson_tool_t& task_args,
                         const string& from_name, long callback_id) {}
    virtual    void callback(const ffjson_tool_t& task_args, long callback_id) {}
};


class task_processor_mgr_t{
public:
    void add(const string& name, task_processor_i* d){ m_task_register[name] = d; }
    void del(const string& name) { m_task_register.erase(name); }
    task_processor_i* get(const string& name)
    {
        map<string, task_processor_i*>::iterator it = m_task_register.find(name);
        if (it != m_task_register.end())
        {
            return it->second;
        }
        return NULL;
    }
    map<string, task_processor_i*>  m_task_register;
};


}
#endif

