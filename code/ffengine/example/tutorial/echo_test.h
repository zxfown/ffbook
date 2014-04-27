#ifndef _FF_ECHO_TEST_H_
#define _FF_ECHO_TEST_H_

namespace ff
{

//! ����echo �ӿڵ���Ϣ�� in_t����������Ϣ��out_t����Ľ����Ϣ
//! ���Ѵ�ҵ��ǣ�����û��Ϊecho_t��������cmd��Ҳû���ƶ������ƣ�ffmsg_t���Զ��ܹ���ȡecho_t������
struct echo_t
{
    struct in_t: public ffmsg_t<in_t>
    {
        void encode()
        {
            encoder() << data;
        }
        void decode()
        {
            decoder() >> data;
        }
        string data;
    };
    struct out_t: public ffmsg_t<out_t>
    {
        void encode()
        {
            encoder() << data;
        }
        void decode()
        {
            decoder() >> data;
        }
        string data;
    };
};

struct echo_service_t
{
    //! echo�ӿڣ���������ķ��͵���Ϣffreq_t�����ṩ����ģ���������һ����ʾ�������Ϣ�������߷��͵ģ�
    //! �ڶ���ģ�������ʾ�ýӿ�Ҫ���صĽ����Ϣ����
    void echo(ffreq_t<echo_t::in_t, echo_t::out_t>& req_)
    {
        echo_t::out_t out;
        out.data = req_.msg.data;
        LOGINFO(("XX", "foo_t::echo: recv %s", req_.msg.data.c_str()));

        req_.response(out);
    }
};
struct echo_client_t
{
    //! Զ�̵��ýӿڣ�����ָ���ص�������Ҳ�������գ���ͬ��ʹ��ffreq_tָ��������Ϣ���ͣ����ҿ���ʹ��lambda�󶨲���
    void echo_callback(ffreq_t<echo_t::out_t>& req_, int index, ffrpc_t* ffrpc_client)
    {
        if (req_.error())
        {
            LOGERROR(("XX", "error_msg <%s>", req_.error_msg()));
            return;
        }
        else if (index < 10)
        {
            echo_t::in_t in;
            in.data = "helloworld";
            LOGINFO(("XX", "%s %s index=%d callback...", __FUNCTION__, req_.msg.data.c_str(), index));
            sleep(1);
            ffrpc_client->call("echo", in, ffrpc_ops_t::gen_callback(&echo_client_t::echo_callback, this, ++index, ffrpc_client));
        }
        else
        {
            LOGINFO(("XX", "%s %s %d callback end", __FUNCTION__, req_.msg.data.c_str(), index));
        }
    }
};

static  int run_echo_test(arg_helper_t& arg_helper)
{
    echo_service_t foo;
    //! broker�ͻ��ˣ�����ע�ᵽbroker����ע������Լ��ӿڣ�Ҳ����Զ�̵��������ڵ�Ľӿ�
    ffrpc_t ffrpc_service("echo");
    ffrpc_service.reg(&echo_service_t::echo, &foo);

    if (ffrpc_service.open(arg_helper))
    {
        return -1;
    }
    
    ffrpc_t ffrpc_client;
    if (ffrpc_client.open(arg_helper))
    {
        return -1;
    }
    
    echo_t::in_t in;
    in.data = "helloworld";
	echo_client_t client;
    ffrpc_client.call("echo", in, ffrpc_ops_t::gen_callback(&echo_client_t::echo_callback, &client, 1, &ffrpc_client));


    signal_helper_t::wait();
    ffrpc_client.close();
    ffrpc_service.close();
    return 0;
}

}

#endif

