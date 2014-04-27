#ifndef _FF_THRIFT_ECHO_H_
#define _FF_THRIFT_ECHO_H_

#include "echo_types.h"
#include "rpc/ffrpc.h"

namespace ff
{

struct thrift_service_t
{
    //! echo�ӿڣ���������ķ��͵���Ϣffreq_t�����ṩ����ģ���������һ����ʾ�������Ϣ�������߷��͵ģ�
    //! �ڶ���ģ�������ʾ�ýӿ�Ҫ���صĽ����Ϣ����
    void echo(ffreq_t<echo_thrift_in_t, echo_thrift_out_t>& req_)
    {
        LOGINFO(("XX", "foo_t::echo: recv data=%s", req_.msg.data));
        echo_thrift_out_t out;
        out.data = "123456";
        //sleep(1);
        req_.response(out);
    }
};

struct thrift_client_t
{
    //! Զ�̵��ýӿڣ�����ָ���ص�������Ҳ�������գ���ͬ��ʹ��ffreq_tָ��������Ϣ���ͣ����ҿ���ʹ��lambda�󶨲���
    void echo_callback(ffreq_t<echo_thrift_out_t>& req_, int index, ffrpc_t* ffrpc_client)
    {
        if (req_.error())
        {
            LOGERROR(("XX", "error_msg <%s>", req_.error_msg()));
            return;
        }
        else if (index < 1)
        {
            echo_thrift_in_t in;
            in.data = "Ohnice";
            LOGINFO(("XX", "%s data=%s index=%d callback...", __FUNCTION__, req_.msg.data, index));
            sleep(1);
            ffrpc_client->call("echo", in, ffrpc_ops_t::gen_callback(&thrift_client_t::echo_callback, this, ++index, ffrpc_client));
        }
        else
        {
            LOGINFO(("XX", "%s %d callback end", __FUNCTION__, index));
        }
    }
};

static  int run_thrift_test(arg_helper_t& arg_helper)
{
    thrift_service_t foo;
    //! broker�ͻ��ˣ�����ע�ᵽbroker����ע������Լ��ӿڣ�Ҳ����Զ�̵��������ڵ�Ľӿ�
    ffrpc_t ffrpc_service("echo");
    ffrpc_service.reg(&thrift_service_t::echo, &foo);

    if (ffrpc_service.open(arg_helper))
    {
        return -1;
    }
    
    ffrpc_t ffrpc_client;
    if (ffrpc_client.open(arg_helper))
    {
        return -1;
    }
    
	thrift_client_t client;
    echo_thrift_in_t in;
    in.data = "Ohnice";

    ffrpc_client.call("echo", in, ffrpc_ops_t::gen_callback(&thrift_client_t::echo_callback, &client, 1, &ffrpc_client));


    //sleep(3);
    
    signal_helper_t::wait();
    ffrpc_client.close();
    ffrpc_service.close();
    return 0;
}

}


#endif

