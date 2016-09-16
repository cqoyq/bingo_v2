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

// It is success that the client connect to server, the client auto to send heart-jump.
// the server use server_t2.
BOOST_AUTO_TEST_SUITE(test_tcp_client_t4)

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
int my_parse::max_interval_seconds_on_heartjump = 5;

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
		message_out_with_time("hdr:" << p.get() << ",snd_amount:" << snd_amount << ",write message:" <<
				t.stream_to_string(snd_p, snd_size))
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


BOOST_AUTO_TEST_CASE(t_client){

	my_mgr::construct();


	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_client c(io_service, ipv4, port);

		 io_service.run();


	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	// output
	//	hdr:0x6770a10,connect success!, time:2016-07-03T23:31:46
	//	hdr:0x6770a10,snd_amount:1,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:31:51
	//	hdr:0x6770a10,snd_amount:2,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:31:56
	//	hdr:0x6770a10,snd_amount:3,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:01
	//	hdr:0x6770a10,snd_amount:4,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:06
	//	hdr:0x6770a10,snd_amount:5,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:11
	//	hdr:0x6770a10,snd_amount:6,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:16
	//	hdr:0x6770a10,snd_amount:7,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:21
	//	hdr:0x6770a10,snd_amount:8,write message:68 23 00 00 01 00 8f 72 d8 0d 16 , time:2016-07-03T23:32:26


}

BOOST_AUTO_TEST_SUITE_END()
