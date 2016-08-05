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

// Test that the four client connect success. the server will send message to every client in 100 times
// and interval 20ms.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t8)

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

int snd_amount_1 = 0;
int snd_amount_2 = 0;
int snd_amount_3 = 0;
int snd_amount_4 = 0;

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

		remain_size = 5 + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				u8_t& err_code){

		string_ex t;
		cout << "hdr:" << p.get() << ",get data:" <<  t.stream_to_string(rev_data, rev_data_size) << endl;


		return 0;
	}


	void write_pk_full_complete_func(
				pointer p,
				char*& snd_p,
				size_t& snd_size,
				const boost::system::error_code& ec){
		if(snd_p[1] == 1){
			snd_amount_1++;
			cout << "hdr:" << p.get() << ",amount1:" << snd_amount_1 << endl;
		}else if(snd_p[1] == 2){
			snd_amount_2++;
			cout << "hdr:" << p.get() << ",amount2:" << snd_amount_2 << endl;
		}else if(snd_p[1] == 3){
			snd_amount_3++;
			cout << "hdr:" << p.get() << ",amount3:" << snd_amount_3 << endl;
		}else if(snd_p[1] == 4){
			snd_amount_4++;
			cout << "hdr:" << p.get() << ",amount4:" << snd_amount_4 << endl;
		}
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

	this_thread::sleep(seconds(10));

	if(handler_pointers.size() > 0){

		u32_t idx = n -1;
		u8_t err_code = 0;

		char data[16] = {0x68, 0x01, 0x00, 0x00, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};
		size_t data_size = 16;


//		while(true){
		for (int i = 0; i < 100; ++i) {

			data[1] = (u8_t)n;
//			data[3] = (u8)i;

			BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[idx], &data[0], data_size, err_code) == 0);

			this_thread::sleep(milliseconds(20));
		}

		BOOST_CHECK(my_mgr::instance()->send_close(handler_pointers[idx], err_code) == 0);
	}
}

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t1(run_thread, 1);
	boost::thread t2(run_thread, 2);
	boost::thread t3(run_thread, 3);
	boost::thread t4(run_thread, 4);

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
	//	hdr:0x6772940,amount3:100
	//	hdr:0x6772940,err_code:9,do catch_error()
	//	handler:0x6772940,destory!

	//	hdr:0x6770a30,amount2:100
	//	hdr:0x6770a30,err_code:9,do catch_error()
	//	handler:0x6770a30,destory!

	//	hdr:0x6774870,amount4:100
	//	hdr:0x6774870,err_code:9,do catch_error()
	//	handler:0x6774870,destory!

	//	hdr:0x676eab0,amount1:100
	//	hdr:0x676eab0,err_code:9,do catch_error()
	//	handler:0x676eab0,destory!





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

//		char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};
//
//		sock.write_some(buffer(&data[0], 16)); //发送数据

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

	 t1.join();
	 t2.join();
	 t3.join();
	 t4.join();

	 // output


}

BOOST_AUTO_TEST_SUITE_END()

