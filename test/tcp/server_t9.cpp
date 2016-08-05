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

// Test that the client connect to server. the server will authenticate client, invalid to disconnect.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t9)

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
int my_parse::max_wait_for_authentication_pass_seconds = 15;

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
		size_t size = (u8_t)rev_data[3];
		remain_size = size + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				u8_t& err_code){

		if(rev_data[1] == 0x23){
			// heartjump
			p->set_heartjump_datetime();
			cout << "hdr:" << p.get() << ",do set_heartjump_datetime()" << endl;
		}else if(rev_data[1] == 0x30){

			// authentication is pass
			p->set_authentication_pass();
			cout << "hdr:" << p.get() << ",receive package do set_authentication_pass()" << endl;
		}

		return 0;
	}

	int active_send_in_ioservice_func(
			my_handler::pointer p,
			package*& pk,
			u8_t& err_code){

		char* snd_p = pk->header();
		if(snd_p[1] == 0x01){
			// authencation is pass
			p->set_authentication_pass();
			cout << "hdr:" << p.get() << ",do set_authentication_pass()" << endl;
		}

		return 0;
	}



	void catch_error_func(my_handler::pointer p, u8_t& err_code){
		cout << "hdr:" << p.get() << ",err_code:" << (int)err_code << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		u8_t err_code = 0;
		if(my_mgr::instance()->erase(p.get(), err_code) == 0){
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
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
//		cout << "hdr:" << ptr.get() << ",do accept_success_func() success" << endl;
		handler_pointers.push_back(ptr.get());
		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;



void run_thread(int& n){

	if(n == 1){

	}else{

		this_thread::sleep(seconds(10));

		char data[5] = {0x68, 0x01, 0x00, 0x00, 0x16};
		size_t data_size = 5;

		u8_t err_code = 0;
		if(handler_pointers.size() == 3){
			// Must 3 client connect.
			BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[1], &data[0], data_size, err_code) == 0);
		}
	}
}

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

//	boost::thread t1(run_thread, 1);
	boost::thread t2(run_thread, 2);	// Set second socket that authentication is pass.

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
	//	set heartjump time:2016-07-01T18:10:09.671763,hdr:0x676e530
	//	set authentication pass time:2016-07-01T18:10:09.787199,hdr:0x676e530
	//	thr:674a100,call accept_handler()
	//	set authentication pass time:2016-07-01T18:10:12.373997,hdr:0x676e530

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T18:10:12.395742,hdr:0x6770ab0
	//	set authentication pass time:2016-07-01T18:10:12.396277,hdr:0x6770ab0
	//	thr:674a100,call accept_handler()
	//	set authentication pass time:2016-07-01T18:10:12.411124,hdr:0x6770ab0

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T18:10:12.412260,hdr:0x67729c0
	//	set authentication pass time:2016-07-01T18:10:12.412904,hdr:0x67729c0

	//	set heartjump time:2016-07-01T18:10:19.836729,hdr:0x6770ab0
	//	hdr:0x6770ab0,do set_authentication_pass()
	//	start: check_authentication_pass, time:2016-07-01T18:10:24
	//	start: check_authentication_pass, time:2016-07-01T18:10:39

	//	hdr:0x676e530,err_code:8,do catch_error()		// first socket authenticate isn't pass, will close.
	//	handler:0x676e530,destory!

	//	start: check_authentication_pass, time:2016-07-01T18:10:54
	//	start: check_authentication_pass, time:2016-07-01T18:11:09

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

	 t1.join();
	 t2.join();


	 // output


}

BOOST_AUTO_TEST_SUITE_END()

