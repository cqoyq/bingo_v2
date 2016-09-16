/*
 * server_t1.cpp
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::asio::ip;

#include "../../bingo/tcp/all.h"
using namespace bingo::tcp;
#include "../../bingo/type.h"
#include "../../bingo/string.h"
#include "../../bingo/singleton.h"
using namespace bingo;

// Test that the client connect success, and then send message to server. the server receive full message,
// to find that message is wrong, and to disconnect
BOOST_AUTO_TEST_SUITE(test_tcp_server_t5)

// ----------------------------- TCP_MESSAGE_PACKAGE ------------------------------ //
#pragma pack(1)
struct my_package{
	char data[256];
};
#pragma pack()


// ----------------------------- PARSER ------------------------------ //
struct my_parse{
	static const size_t header_size;								// Parse size of package's header.
	static int max_wait_for_heartjump_seconds;						// If the value is 0, then server don't check heartjump.
	static int max_wait_for_authentication_pass_seconds;			// If the value is 0, then server don't check authentication pass.


};
const size_t my_parse::header_size = 4;
int my_parse::max_wait_for_heartjump_seconds = 0;
int my_parse::max_wait_for_authentication_pass_seconds = 0;

// ----------------------------- SOCKET_MANAGER ------------------------------ //
class my_handler;
typedef bingo::singleton_v0<tcp_svr_hdr_manager<my_handler> > my_mgr;

// ----------------------------- HANDLER ------------------------------ //
//typedef tcp_svr_handler<my_parse,			// Define static header_size
//	 	 				my_package			// Message
//	 	 	> my_handler;
class my_handler : public tcp_svr_handler<my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service) :
		tcp_svr_handler<my_parse,my_package>(io_service){

	}

	int read_pk_header_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				size_t& remain_size,
				error_what& e_what){

		string_ex t;
		cout << "hdr:" << p.get() << ",do read_pk_header_complete_func(), header data:" <<
					t.stream_to_string(rev_data, rev_data_size) << endl;

		remain_size = 5 + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				error_what& e_what){

		string_ex t;
		cout << "hdr:" << p.get() << ",do read_pk_full_complete_func(), data:" <<
				t.stream_to_string(rev_data, rev_data_size) << endl;

		e_what.err_no(error_tcp_package_body_is_wrong);
		e_what.err_message(error_tcp_package_body_is_wrong_message);

		return -1;
	}


	void catch_error_func(my_handler::pointer p, error_what& e_what){
		cout << "hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		error_what e_what;
		if(my_mgr::instance()->erase(p.get(), e_what) == 0){
			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << e_what.err_no() << endl;
		}
	}
};





// ----------------------------- SERVER ------------------------------ //

vector<void*> handler_pointers;

class my_server : public tcp_server<my_handler, my_mgr, my_parse>{
public:
	my_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_server<my_handler, my_mgr, my_parse>(io_service, ipv4, port){

	}

	int accept_success_func(my_server::pointer ptr, error_what& e_what){

		my_mgr::instance()->push(ptr.get());
		cout << "hdr:" << ptr.get() << ",do accept_success_insert_handler_mgr()" << endl;
		handler_pointers.push_back(ptr.get());

		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;


BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_server server(io_service, ipv4, port);

		 io_service.run();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	// ouput:
	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T11:43:03.186651,hdr:0x676b390
	//	set authentication pass time:2016-07-01T11:43:03.309289,hdr:0x676b390
	//
	//	thr:674a100,call accept_handler()
	//	hdr:0x676b390,do accept_success_insert_handler_mgr()
	//	set authentication pass time:2016-07-01T11:43:07.013515,hdr:0x676b390

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T11:43:07.034596,hdr:0x676d830
	//	set authentication pass time:2016-07-01T11:43:07.035152,hdr:0x676d830

	//	hdr:0x676b390,do read_pk_header_complete_func(), header data:68 33 00 05
	//	hdr:0x676b390,do read_pk_full_complete_func(), data:68 33 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x676b390,err_code:4,do catch_error()	// err_code:4 = error_tcp_package_body_is_wrong
	//	hdr:0x676b390,do close_completed_erase_hander_mgr(),success
	//	handler:0x676b390,destory!


}

BOOST_AUTO_TEST_CASE(t_client){
	try
	{
		cout << "client start." << endl;

		io_service ios;
		ip::tcp::socket sock(ios);						//创建socket对象

		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 18000);	//创建连接端点


		sock.connect(ep);								//socket连接到端点

		cout << "client msg : remote_endpoint : " << sock.remote_endpoint().address() << endl;	//输出远程连接端点信息

		char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

		sock.write_some(buffer(&data[0], 16)); //发送数据

		while(true){
			vector<char> str(256, 0);						//定义一个vector缓冲区
			size_t received_size = sock.read_some(buffer(str));					//使用buffer()包装缓冲区数据

			bingo::string_ex tool;
			cout << "receviced data:" << tool.stream_to_string(&str[0], received_size) << endl;
		}

		sock.close();

		// output:
		//		client start.
		//		client msg : remote_endpoint : 127.0.0.1
		//		exception:read_some: Connection reset by peer


	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "exception:" << e.what() << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()

