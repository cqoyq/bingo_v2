/*
 * tcp_server.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_SERVER_HEADER_H_
#define BINGO_TCP_SERVER_HEADER_H_

#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

namespace bingo { namespace tcp {

template<typename HANDLER,
		 typename SOCKET_MANAGER,
		 typename PARSER>
class tcp_server {


public:
	typedef boost::shared_ptr<HANDLER> pointer;

	tcp_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port)
		:  ios_(io_service),
		   heartjump_timer_(io_service),
		   authentication_timer_(io_service),
		   acceptor_(io_service,
					 ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ipv4), port)){

		start_accept();

		// Start to inspect heartjump
		check_heartjump();

		// Start to inspect authentication pass
		check_authentication();
	}
	virtual ~tcp_server(){}


	void start_accept(){

#ifdef BINGO_TCP_SERVER_DEBUG
		std::stringstream stream;
		stream << this_thread::get_id();
		string id = stream.str();
		message_out("thr:" << id << ",call start_accept()")
#endif

		// Make new tcp_svr_handler object.
		pointer new_handler(new HANDLER(ios_));

		// Start to wait for connect.
		acceptor_.async_accept(new_handler->socket(),
				boost::bind(&tcp_server::accept_handler, this, new_handler,
				boost::asio::placeholders::error));
	}


private:

	void accept_handler(pointer new_handler,
				const boost::system::error_code& ec){

		if (!ec)
		{

#ifdef BINGO_TCP_SERVER_DEBUG
			std::stringstream stream;
			stream << this_thread::get_id();
			string id = stream.str();
			message_out("thr:" << id << ",call accept_handler()")
#endif

			// Call accept_handle_success_func()
			error_what e_what;
			if(accept_success_func(new_handler, e_what) == 0)
				// Start to aync-read.
				new_handler->start();
			else{

				new_handler->catch_error(e_what);

				// Active close socket.
				new_handler->close_socket();
			}


		}else{
			// Call accept_error_func()
			accept_error_func(ec);
		}

		// Start another one.
		start_accept();
	}

	void check_authentication(){
		if(PARSER::max_wait_for_authentication_pass_seconds > 0){
			boost::posix_time::seconds s(PARSER::max_wait_for_authentication_pass_seconds);
			boost::posix_time::time_duration td = s;
			authentication_timer_.expires_from_now(td);
			authentication_timer_.async_wait(bind(&tcp_server::authentication_handler,
					this,
					boost::asio::placeholders::error));
		}
	}
	void authentication_handler(const system::error_code& ec){
		if(!ec){
#ifdef BINGO_TCP_SERVER_DEBUG
			message_out_with_time("start: check_authentication_pass")
#endif
			SOCKET_MANAGER::instance()->check_authentication_pass();
			check_authentication();
		}
	}

	void check_heartjump(){
		if(PARSER::max_wait_for_heartjump_seconds > 0){
			boost::posix_time::seconds s(PARSER::max_wait_for_heartjump_seconds);
			boost::posix_time::time_duration td = s;
			heartjump_timer_.expires_from_now(td);
			heartjump_timer_.async_wait(bind(&tcp_server::heartjump_handler,
					this,
					boost::asio::placeholders::error));
		}
	}
	void heartjump_handler(const system::error_code& ec){
		if(!ec){
#ifdef BINGO_TCP_SERVER_DEBUG
			message_out_with_time("start: check_heartjump")
#endif
			SOCKET_MANAGER::instance()->check_heartjump();
			check_heartjump();
		}
	}
public:

	virtual int accept_success_func(pointer /*ptr*/, error_what& /*e_what*/){
		return 0;
	}

	virtual void accept_error_func(const system::error_code& /*ec*/){
	}

protected:
	boost::asio::io_service& ios_;
	ip::tcp::acceptor acceptor_;

	deadline_timer heartjump_timer_;
	deadline_timer authentication_timer_;
};

} }

#endif /* BINGO_TCP_SERVER_HEADER_H_ */
