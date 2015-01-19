// Copyright (c) 2014, Jack.
// All rights reserved.
//
// Author: feimat <512284622@qq.com>
// Created: 07/10/14
// Description:

#include "rpc_client.h"
#include "echo.pb.h"
#include "xcore_thread.h"
#include "xcore_base64.h"
#include <string>
#include <list>
#include <string.h>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

#include "pbext.h"
#include "xcore_atomic.h"
#include "codeconverter.h"
#include "scopelocale.h"

using std::map;

XAtomic32 recv_c = 0;

class Test {
public:
	void echo_done(echo::EchoRequest* request,
		echo::EchoResponse* response,
		::google::protobuf::Closure* done) {
			std::string res = response->response();
			//if (!res.empty())
			//    printf("async: %s\n", response->response().c_str());
			//printf("async: %s\n", request->message().c_str());
			if (done) {
				done->Run();
			}
			delete request; // �ص�Ҫע���ڴ��ͷ�
			delete response;
			if (++recv_c == 5) {
				std::cout << "success\n";
			}
	}
};

void echo_done(echo::EchoRequest* request,
	echo::EchoResponse* response,
	::google::protobuf::Closure* done) {
		std::string res = response->response();
		//if (!res.empty())
		//    printf("async: %s\n", response->response().c_str());
		//printf("async: %s\n", request->message().c_str());
		if (done) {
			done->Run();
		}
		delete request; // �ص�Ҫע���ڴ��ͷ�
		delete response;
		if (++recv_c == 5) {
			std::cout << "success\n";
		}
}

int ext_processer(CHttpParser& ps, std::string& data, void* param) {
	std::string mes_name = ps.get_head_field("mes_name");
	return 0;
}

void close_handler(void* param) {
}

int main(int argc, char *argv[]) {
    //RpcClient client(10, "192.168.1.13", 8998, 1000); // 1:����sock�� 2:host 3:ip 4:��ʱʱ��
    //echo::EchoService::Stub stub(&client);

	Test test;
    ::google::protobuf::RpcChannel* client;
    echo::EchoService_Stub::Stub* stub;
    client = new RpcClient(2, "192.168.1.13", 8999, 5000); // 1:����sock�� 2:host 3:ip 4:��ʱʱ��
	if (!((RpcClient*)client)->IsConnected()) {
		delete client;
		exit(0);
	} else {
		std::cout << "connect success\n";
	}
	((RpcClient*)client)->RegiExtProcesser(ext_processer, NULL); // ��������������Ƶ���Ϣ
	((RpcClient*)client)->RegiCloseHandler(close_handler, NULL); // �Ͽ��¼�����
	int i =0;
	while (++i < 5) {
		//xcore::sleep(1000);
	}
    stub = new echo::EchoService_Stub::Stub(client);

    for (int i =0; i < 5; ++i) {
        echo::EchoRequest* request = new echo::EchoRequest();
        request->set_message("client_hello");
        echo::EchoResponse* response = new echo::EchoResponse();
        ::google::protobuf::Closure* callback_callback = NULL; // ���Եݹ����޻ص�
        ::google::protobuf::Closure *callback = pbext::NewCallback(&test,
			                                                       &Test::echo_done,
                                                                   request,
                                                                   response,
                                                                   callback_callback);
		//::google::protobuf::Closure *callback = pbext::NewCallback(&echo_done,
		//	request,
		//	response,
		//	callback_callback);

        stub->Echo(NULL, request, response, callback); // �첽, callbackΪ������ͬ��
    }

    echo::EchoRequest req;
    req.set_message("cli hello 2");
    echo::EchoResponse res;
    stub->Echo(NULL, &req, &res, NULL); // ͬ��
    std::cout << "sync: " << res.response() << "\n";

	xcore::sleep(5000);
	delete client;
    delete stub;


    return 0;
}

