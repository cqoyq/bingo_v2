/*
 * client_t1.cpp
 *
 *  Created on: 2016-7-3
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

// It is success that the client connect to server, the message is correct both from server and to server.
// the server use server_t8.
BOOST_AUTO_TEST_SUITE(test_tcp_client_t3)

// ----------------------------- HEARTJUMP_PACKAGE ------------------------------ //

#pragma pack(1)
struct my_heartjump{
	static size_t data_size;
	static char data[11];
};
#pragma pack()
size_t 	my_heartjump::data_size = 11;
char 	my_heartjump::data[11] = {0x68, 0x23, 0x00, 0x00, 0x01, 0x00, 0x8f, 0x72, 0xd8, 0x0d, 0x16};


// ----------------------------- TCP_MESSAGE_PACKAGE ------------------------------ //

#pragma pack(1)
struct my_package{
	char data[256];
};
#pragma pack()

// ----------------------------- PARSER ------------------------------ //
struct my_parse{
	static int retry_delay_seconds;				// When Client connect fail, don't reconnect if the value is 0.
	static int max_retry_delay_seconds;

	static const size_t header_size;				// Parse size of package's header.
	static int max_interval_seconds_on_heartjump;	// Client don't send heartjump if the value is 0.
};
int my_parse::retry_delay_seconds = 3;
int my_parse::max_retry_delay_seconds = 60;

const size_t my_parse::header_size = 4;
int my_parse::max_interval_seconds_on_heartjump = 0;

// ----------------------------- SOCKET_MANAGER ------------------------------ //
class my_handler;
typedef bingo::singleton_v0<tcp_cet_hdr_manager<my_handler> > my_mgr;

// ----------------------------- HANDLER ------------------------------ //

int rev_amount = 0;
int snd_amount = 0;

class my_handler : public tcp_cet_handler<my_heartjump, my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service, boost::function<void()> f) :
		tcp_cet_handler<my_heartjump, my_parse, my_package>(io_service, f){

	}

	int read_pk_header_complete_func(
				pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				size_t& remain_size,
				error_what& e_what){

		remain_size = 5 + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				error_what& e_what){
		rev_amount++;
		string_ex t;
		cout << "hdr:" << p.get() << ",rev_amount:" << rev_amount << ",read message:" <<
				t.stream_to_string(rev_data, rev_data_size) << endl;

		return 0;
	}

	void write_pk_full_complete_func(
			pointer p,
			char*& snd_p,
			size_t& snd_size,
			const boost::system::error_code& ec){
		snd_amount++;
		string_ex t;
		cout << "hdr:" << p.get() << ",snd_amount:" << snd_amount << ",write message:" <<
				t.stream_to_string(snd_p, snd_size) << endl;
	}



	void catch_error_func(pointer p, error_what& e_what){
		cout << "hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		error_what e_what;
		if(my_mgr::instance()->erase(p.get(), e_what) == 0){
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
		}
	}
};

// ----------------------------- CLIENT ------------------------------ //

void* handler_pointer = 0;

class my_client : public tcp_client<my_handler, my_parse> {
public:
	my_client(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_client<my_handler, my_parse>(io_service, ipv4, port){

	}

	int connet_success_func(pointer ptr, error_what& e_what){
		message_out_with_time("hdr:" << ptr.get() << ",connect success!")

		my_mgr::instance()->push(ptr.get());
		handler_pointer = ptr.get();
		return 0;
	}

	void connet_fail_func(pointer ptr, int& retry_delay_seconds){
		cout << "hdr:" << ptr.get() << ",reconnect after " << retry_delay_seconds << " seconds." << endl;
	}
};




void run_client(){

	this_thread::sleep(seconds(4));

	char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};
	size_t data_size = 16;

	error_what e_what;
	int i = 0;
	while(i < 4){
		this_thread::sleep(milliseconds(10));

		my_mgr::instance()->send_data(handler_pointer, data, data_size, e_what);

		i++;
	}
};

BOOST_AUTO_TEST_CASE(t_client){

	my_mgr::construct();

	thread t(run_client);

	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_client c(io_service, ipv4, port);

		 io_service.run();

		 t.join();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	// output
	//	hdr:0x6770960,connect success!, time:2016-07-03T23:25:38
	//
	//	hdr:0x6770960,snd_amount:1,write message:68 33 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,snd_amount:2,write message:68 33 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,snd_amount:3,write message:68 33 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,snd_amount:4,write message:68 33 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//
	//	hdr:0x6770960,rev_amount:1,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:2,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:3,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:4,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:5,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:6,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:7,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:8,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:9,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:10,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:11,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:12,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:13,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:14,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:15,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:16,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:17,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:18,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:19,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:20,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:21,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:22,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:23,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:24,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:25,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:26,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:27,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:28,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:29,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:30,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:31,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:32,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:33,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:34,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:35,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:36,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:37,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:38,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:39,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:40,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:41,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:42,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:43,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:44,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:45,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:46,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:47,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:48,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:49,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:50,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:51,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:52,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:53,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:54,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:55,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:56,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:57,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:58,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:59,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:60,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:61,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:62,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:63,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:64,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:65,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:66,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:67,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:68,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:69,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:70,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:71,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:72,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:73,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:74,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:75,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:76,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:77,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:78,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:79,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:80,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:81,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:82,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:83,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:84,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:85,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:86,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:87,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:88,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:89,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:90,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:91,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:92,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:93,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:94,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:95,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:96,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:97,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:98,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:99,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,rev_amount:100,read message:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	hdr:0x6770960,err_code:10,do catch_error()
	//	hdr:0x6770960,destory!, time:2016-07-03T23:25:47
	//	hdr:0x6780cd0,connect success!, time:2016-07-03T23:25:47

}

BOOST_AUTO_TEST_SUITE_END()
