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

// Test that the client connect to server. the server will check heart-jump interval 10 seconds, if the
// heart-jump is timeout, server will close socket.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t10)

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
int my_parse::max_wait_for_heartjump_seconds = 10;
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



void run_thread(){

	this_thread::sleep(seconds(10));

	char data[5] = {0x68, 0x01, 0x00, 0x00, 0x16};
	size_t data_size = 5;

	u8_t err_code = 0;
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[0], &data[0], data_size, err_code) == 0);
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[1], &data[0], data_size, err_code) == 0);
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[2], &data[0], data_size, err_code) == 0);

}

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t1(run_thread);		// Set socket that authentication is pass.

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
	//	set heartjump time:2016-07-01T18:35:56.255795,hdr:0x676f9d0
	//	set authentication pass time:2016-07-01T18:35:56.312543,hdr:0x676f9d0
	//	thr:674a100,call accept_handler()
	//	set authentication pass time:2016-07-01T18:35:59.623954,hdr:0x676f9d0

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T18:35:59.642809,hdr:0x6771b20
	//	set authentication pass time:2016-07-01T18:35:59.643296,hdr:0x6771b20
	//	thr:674a100,call accept_handler()
	//	set authentication pass time:2016-07-01T18:36:01.482393,hdr:0x6771b20

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T18:36:01.483552,hdr:0x6773a30
	//	set authentication pass time:2016-07-01T18:36:01.484029,hdr:0x6773a30
	//	thr:674a100,call accept_handler()
	//	set authentication pass time:2016-07-01T18:36:03.558160,hdr:0x6773a30

	//	thr:674a100,call start_accept()
	//	set heartjump time:2016-07-01T18:36:03.558847,hdr:0x6775960
	//	set authentication pass time:2016-07-01T18:36:03.559290,hdr:0x6775960
	//
	//
	//	set heartjump time:2016-07-01T18:36:06.220678,hdr:0x676f9d0
	//	hdr:0x676f9d0,do set_authentication_pass()						// first socket authenticate is pass
	//	set heartjump time:2016-07-01T18:36:06.235756,hdr:0x6771b20
	//	hdr:0x6771b20,do set_authentication_pass()						// second socket authenticate is pass
	//	set heartjump time:2016-07-01T18:36:06.245135,hdr:0x6773a30
	//	hdr:0x6773a30,do set_authentication_pass()						// third socket authenticate is pass

	//	set heartjump time:2016-07-01T18:36:06.259960,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()						// receive heart-jump second socket
	//	set heartjump time:2016-07-01T18:36:06.261564,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()						// receive heart-jump third socket

	//	start: check_heartjump, time:2016-07-01T18:36:06
	//	check heartjump time, p1_:2016-07-01T18:36:06.220678,p1:2016-07-01T18:35:56.342427,p1_< p1:0,hdr:0x676f9d0
	//	check heartjump time, p1_:2016-07-01T18:36:06.259960,p1:2016-07-01T18:35:56.348663,p1_< p1:0,hdr:0x6771b20
	//	check heartjump time, p1_:2016-07-01T18:36:06.261564,p1:2016-07-01T18:35:56.349484,p1_< p1:0,hdr:0x6773a30

	//	set heartjump time:2016-07-01T18:36:10.262552,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:10.263126,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()

	//	start: check_authentication_pass, time:2016-07-01T18:36:11

	//	set heartjump time:2016-07-01T18:36:14.262567,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:14.263302,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()

	//	start: check_heartjump, time:2016-07-01T18:36:16
	//	check heartjump time, p1_:2016-07-01T18:36:06.220678,p1:2016-07-01T18:36:06.358129,p1_< p1:1,hdr:0x676f9d0
	//	hdr:0x676f9d0,err_code:7,do catch_error()						// close socket third socket
	//	check heartjump time, p1_:2016-07-01T18:36:14.262567,p1:2016-07-01T18:36:06.368239,p1_< p1:0,hdr:0x6771b20
	//	check heartjump time, p1_:2016-07-01T18:36:14.263302,p1:2016-07-01T18:36:06.369179,p1_< p1:0,hdr:0x6773a30
	//	handler:0x676f9d0,destory!

	//	set heartjump time:2016-07-01T18:36:18.262978,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:18.263668,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()

	//	set heartjump time:2016-07-01T18:36:22.263408,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:22.264179,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()

	//	set heartjump time:2016-07-01T18:36:26.266089,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:26.266671,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()

	//	start: check_authentication_pass, time:2016-07-01T18:36:26

	//	start: check_heartjump, time:2016-07-01T18:36:26
	//	check heartjump time, p1_:2016-07-01T18:36:26.266671,p1:2016-07-01T18:36:16.371733,p1_< p1:0,hdr:0x6771b20
	//	check heartjump time, p1_:2016-07-01T18:36:26.266089,p1:2016-07-01T18:36:16.372788,p1_< p1:0,hdr:0x6773a30

	//	set heartjump time:2016-07-01T18:36:30.267214,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:30.267919,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()

	//	set heartjump time:2016-07-01T18:36:34.268421,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:34.269153,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()

	//	start: check_heartjump, time:2016-07-01T18:36:36
	//	check heartjump time, p1_:2016-07-01T18:36:34.269153,p1:2016-07-01T18:36:26.377921,p1_< p1:0,hdr:0x6771b20
	//	check heartjump time, p1_:2016-07-01T18:36:34.268421,p1:2016-07-01T18:36:26.378683,p1_< p1:0,hdr:0x6773a30
	//	set heartjump time:2016-07-01T18:36:38.269754,hdr:0x6773a30

	//	hdr:0x6773a30,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:38.270482,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()

	//	start: check_authentication_pass, time:2016-07-01T18:36:41

	//	set heartjump time:2016-07-01T18:36:42.270343,hdr:0x6773a30
	//	hdr:0x6773a30,do set_heartjump_datetime()
	//	set heartjump time:2016-07-01T18:36:42.271076,hdr:0x6771b20
	//	hdr:0x6771b20,do set_heartjump_datetime()


}

void run_client(int& n){

	if(n == 2)
		this_thread::sleep(seconds(2));
	if(n == 3)
		this_thread::sleep(seconds(4));

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

			if(n != 1)
				while(true){
					char data2[11] = {0x68, 0x23, 0x00, 0x00, 0x01, 0x00, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

					sock.write_some(buffer(&data2[0], 11)); //发送数据

					this_thread::sleep(seconds(4));
				}

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

	t1.join();
	t2.join();
	t3.join();


	// output
	//	 n:1,client start.
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:2,client start.
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:3,client start.
	//	 n:3,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,receviced data:68 01 00 00 16
	//	 n:2,receviced data:68 01 00 00 16
	//	 n:3,receviced data:68 01 00 00 16
	//	 n:1,exception:read_some: End of file      // first socket has closed by server.

}

BOOST_AUTO_TEST_SUITE_END()

