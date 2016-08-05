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

// Test that the five client connect to server. the server will allow to connect max of three client,
// more connector will been diconnected.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t7)

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
				u8_t& err_code){

//		string_ex t;
//		cout << "hdr:" << p.get() << ",do read_pk_header_complete_func(), header data:" <<
//					t.stream_to_string(rev_data, rev_data_size) << endl;

		remain_size = 5 + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				u8_t& err_code){

//		string_ex t;
//		cout << "hdr:" << p.get() << ",do read_pk_full_complete_func(), data:" <<
//				t.stream_to_string(rev_data, rev_data_size) << endl;

		return 0;
	}


	void write_pk_full_complete_func(
				pointer p,
				char*& snd_p,
				size_t& snd_size,
				const boost::system::error_code& ec){
//		string_ex t;
//		cout << "hdr:" << p.get() << ",do write_pk_full_complete_func(), data:" <<
//				t.stream_to_string(snd_p, snd_size) << endl;
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

		if(my_mgr::instance()->size() == 3){

			cout << "hdr:" << ptr.get() << ",do accept_success_func() fail" << endl;
			err_code = error_tcp_handler_mgr_sequence_is_full;
			return -1;
		}else{
			my_mgr::instance()->push(ptr.get());
			cout << "hdr:" << ptr.get() << ",do accept_success_func() success" << endl;
			handler_pointers.push_back(ptr.get());
			return 0;
		}
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
	//	set heartjump time:2016-07-01T17:33:33.995226,hdr:0x676cad0
	//	set authentication pass time:2016-07-01T17:33:34.120066,hdr:0x676cad0
	//	thr:674a100,call accept_handler()
	//	hdr:0x676cad0,do accept_success_func() success
	//	set authentication pass time:2016-07-01T17:33:36.841819,hdr:0x676cad0

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T17:33:36.866792,hdr:0x676ef70
	//	set authentication pass time:2016-07-01T17:33:36.867315,hdr:0x676ef70
	//	thr:674a100,call accept_handler()
	//	hdr:0x676ef70,do accept_success_func() success
	//	set authentication pass time:2016-07-01T17:33:36.871637,hdr:0x676ef70

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T17:33:36.877746,hdr:0x6770e80
	//	set authentication pass time:2016-07-01T17:33:36.878432,hdr:0x6770e80
	//	thr:674a100,call accept_handler()
	//	hdr:0x6770e80,do accept_success_func() success
	//	set authentication pass time:2016-07-01T17:33:36.896836,hdr:0x6770e80

	//  --------- four client ------------ //
	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T17:33:36.898081,hdr:0x6772db0
	//	set authentication pass time:2016-07-01T17:33:36.898715,hdr:0x6772db0
	//	thr:674a100,call accept_handler()
	//	hdr:0x6772db0,do accept_success_func() fail
	//	hdr:0x6772db0,err_code:16,do catch_error()
	//	handler:0x6772db0,destory!

	//  --------- five client ------------ //
	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T17:33:36.909568,hdr:0x6774440
	//	set authentication pass time:2016-07-01T17:33:36.910115,hdr:0x6774440
	//	thr:674a100,call accept_handler()
	//	hdr:0x6774440,do accept_success_func() fail
	//	hdr:0x6774440,err_code:16,do catch_error()
	//	handler:0x6774440,destory!

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T17:33:36.914318,hdr:0x6775920
	//	set authentication pass time:2016-07-01T17:33:36.915067,hdr:0x6775920




}

void run_client(int& n){

	try
	{
		cout << "n:" << n << ",client start." << endl;

		io_service ios;
		ip::tcp::socket sock(ios);						//创建socket对象

		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 18000);	//创建连接端点


		sock.connect(ep);								//socket连接到端点

		cout << "n:" << n << ",client msg : remote_endpoint : " << sock.remote_endpoint().address() << endl;	//输出远程连接端点信息

		char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

		sock.write_some(buffer(&data[0], 16)); //发送数据

		while(true){
			vector<char> str(256, 0);						//定义一个vector缓冲区
			size_t received_size = sock.read_some(buffer(str));					//使用buffer()包装缓冲区数据

			bingo::string_ex tool;
			cout << "n:" << n << ",receviced data:" << tool.stream_to_string(&str[0], received_size) << endl;

	//		this_thread::sleep(seconds(10));
		}

		sock.close();

	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "n:" << n << ",exception:" << e.what() << endl;
	}

}

BOOST_AUTO_TEST_CASE(t_client){
	boost::thread t1(run_client, 1);
	boost::thread t2(run_client, 2);
	boost::thread t3(run_client, 3);
	boost::thread t4(run_client, 4);
	boost::thread t5(run_client, 5);

	 t1.join();
	 t2.join();
	 t3.join();
	 t4.join();
	 t5.join();

	 // output
	//	 n:2,client start.
	//	 n:1,client start.
	//	 n:3,client start.
	//	 n:3,client msg : remote_endpoint : 127.0.0.1
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:n:4,client start.
	//	 n:4,client msg : remote_endpoint : 127.0.0.1
	//	 5,client start.
	//	 n:5,client msg : remote_endpoint : 127.0.0.1
	//	 n:4,exception:read_some: Connection reset by peer
	//	 n:5,exception:read_some: Connection reset by peer

}

BOOST_AUTO_TEST_SUITE_END()

