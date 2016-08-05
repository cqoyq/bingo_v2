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

// It is success that the client connect to server, and the server disconnect, the client connect again.
// the server use server_t3.
BOOST_AUTO_TEST_SUITE(test_tcp_client_t2)

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

// ----------------------------- HANDLER ------------------------------ //
class my_handler : public tcp_cet_handler<my_heartjump, my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service, function<void()> f) :
		tcp_cet_handler<my_heartjump, my_parse, my_package>(io_service, f){

	}

	void catch_error_func(pointer p, int& err_code){
		cout << "hdr:" << p.get() << ",err_code:" << (int)err_code << ",do catch_error()" << endl;
	}
};

// ----------------------------- SOCKET_MANAGER ------------------------------ //
typedef bingo::singleton_v0<tcp_cet_hdr_manager<my_handler> > my_mgr;

// ----------------------------- CLIENT ------------------------------ //
class my_client : public tcp_client<my_handler, my_parse> {
public:
	my_client(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_client<my_handler, my_parse>(io_service, ipv4, port){

	}

	int connet_success_func(pointer ptr, u8_t& err_code){
		message_out_with_time("hdr:" << ptr.get() << ",connect success!")
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
	//	hdr:0x676fb70,connect success!, time:2016-07-03T22:43:29
	//	hdr:0x676fb70,err_code:10,do catch_error()    // err_code:10 = error_tcp_client_close_socket_because_server
	//	hdr:0x676fb70,destory!, time:2016-07-03T22:43:41
	//	hdr:0x6770db0,connect success!, time:2016-07-03T22:43:41
}

BOOST_AUTO_TEST_SUITE_END()
