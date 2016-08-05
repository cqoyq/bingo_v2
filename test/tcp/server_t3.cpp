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

// Test that the client connect success, the server disconnect after 15 seconds.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t3)

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

	void catch_error_func(my_handler::pointer p, u8_t& err_code){
		cout << "hdr:" << p.get() << ",err_code:" << (int)err_code << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		u8_t err_code = 0;
		if(my_mgr::instance()->erase(p.get(), err_code) == 0){
			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
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

	int accept_success_func(my_server::pointer ptr, u8_t& err_code){

		my_mgr::instance()->push(ptr.get());
		cout << "hdr:" << ptr.get() << ",do accept_success_insert_handler_mgr()" << endl;
		handler_pointers.push_back(ptr.get());

		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;



void run_thread(){

	this_thread::sleep(seconds(15));

	if(handler_pointers.size() > 0){
		u8_t err_code = 0;
		BOOST_CHECK(my_mgr::instance()->send_close(handler_pointers[0], err_code) == 0);
	}
};

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t(run_thread);

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
	//	set heartjump time:2016-06-30T21:17:43.826963,hdr:0x676a0e0
	//	set authentication pass time:2016-06-30T21:17:43.954469,hdr:0x676a0e0
	//
	//	thr:674a100,call accept_handler()
	//	hdr:0x676a0e0,do accept_success_check_handler_mgr()
	//	set authentication pass time:2016-06-30T21:17:53.988921,hdr:0x676a0e0

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-06-30T21:17:54.008422,hdr:0x676c580
	//	set authentication pass time:2016-06-30T21:17:54.009838,hdr:0x676c580
	//
	//
	//	hdr:0x676a0e0,err_code:9,do catch_error()					// err_code:9 = error_tcp_server_close_socket_because_server
	//	hdr:0x676a0e0,do close_completed_erase_hander_mgr(),success
	//	handler:0x676a0e0,destory!


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


	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "exception:" << e.what() << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()

