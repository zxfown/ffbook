#ifndef _FF_PROTOBUF_H_
#define _FF_PROTOBUF_H_

#ifdef FF_ENABLE_PROTOCOLBUF

#include "echo.pb.h"
#include "rpc/ffrpc.h"

namespace ff
{

struct protobuf_service_t
{
    //! echo�ӿڣ���������ķ��͵���Ϣffreq_t�����ṩ����ģ���������һ����ʾ�������Ϣ�������߷��͵ģ�
    //! �ڶ���ģ�������ʾ�ýӿ�Ҫ���صĽ����Ϣ����
    void echo(ffreq_pb_t<pb_echo_in_t, pb_echo_out_t>& req_)
    {
        LOGINFO(("XX", "foo_t::echo: recv data=%s", req_.msg.data()));
        pb_echo_out_t out;
        out.set_data("123456");
        req_.response(out);
    }
};

struct protobuf_client_t
{
    //! Զ�̵��ýӿڣ�����ָ���ص�������Ҳ�������գ���ͬ��ʹ��ffreq_tָ��������Ϣ���ͣ����ҿ���ʹ��lambda�󶨲���
    void echo_callback(ffreq_pb_t<pb_echo_out_t>& req_, int index, ffrpc_t* ffrpc_client)
    {
        if (req_.error())
        {
            LOGERROR(("XX", "error_msg <%s>", req_.error_msg()));
            return;
        }
        else if (index < 10)
        {
            pb_echo_in_t in;
            in.set_data("Ohnice");
            LOGINFO(("XX", "%s data=%s index=%d callback...", __FUNCTION__, req_.msg.data(), index));
            sleep(1);
            ffrpc_client->call_pb("echo", in, ffrpc_ops_t::gen_callback(&protobuf_client_t::echo_callback, this, ++index, ffrpc_client));
        }
        else
        {
            LOGINFO(("XX", "%s %d callback end", __FUNCTION__, index));
        }
    }
};

static  int run_protobuf_test(arg_helper_t& arg_helper)
{
    protobuf_service_t foo;
    //! broker�ͻ��ˣ�����ע�ᵽbroker����ע������Լ��ӿڣ�Ҳ����Զ�̵��������ڵ�Ľӿ�
    ffrpc_t ffrpc_service("echo");
    ffrpc_service.reg(&protobuf_service_t::echo, &foo);

    if (ffrpc_service.open(arg_helper))
    {
        return -1;
    }
    
    ffrpc_t ffrpc_client;
    if (ffrpc_client.open(arg_helper))
    {
        return -1;
    }
    
	protobuf_client_t client;
    pb_echo_in_t in;
    in.set_data("Ohnice");

    ffrpc_client.call_pb("echo", in, ffrpc_ops_t::gen_callback(&protobuf_client_t::echo_callback, &client, 1, &ffrpc_client));


    signal_helper_t::wait();
    ffrpc_client.close();
    ffrpc_service.close();
    return 0;
}

}
#else
static  int run_protobuf_test(arg_helper_t& arg_helper)
{
    printf("please add FF_ENABLE_PROTOBUF to makefile, make -f protobuf.Makefile\n");
    return 0;
}
#endif
#endif

