/*
 * tcp_client.h
 *
 *  Created on: 2016-7-1
 *      Author: root
 */

#ifndef BINGO_TCP_CLIENT_HEADER_H_
#define BINGO_TCP_CLIENT_HEADER_H_

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
		 typename PARSER>
class tcp_client {
public:
	typedef boost::shared_ptr<HANDLER> pointer;

	tcp_client(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
						ios_(io_service), ipv4_(ipv4), port_(port),
						timer_(io_service),
						retry_delay_(PARSER::retry_delay_seconds){
		start_connect();
	}

	virtual ~tcp_client(){}

	void start_connect(){

		// Make new tcp_connection object.
		pointer new_handler(new HANDLER(ios_,
				boost::bind(&tcp_client::reconnect, this)));

		// Start to connect.
		new_handler->socket().async_connect(
				ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ipv4_), port_),
				boost::bind(&tcp_client::connect_handler, this, new_handler,
				boost::asio::placeholders::error));
	}

private:

	// Call the function when connect in handler again.
	void reconnect(){
		retry_delay_ = PARSER::retry_delay_seconds;
		start_connect();
	}

	void connect_handler(pointer new_handler,
			const boost::system::error_code& ec){
		if (!ec){

			// Call connet_success_func()
			error_what e_what;
			if(connet_success_func(new_handler, e_what) == 0)
				// Start to aync-read.
				new_handler->start();
			else{

				new_handler->catch_error(e_what);

				// Active close socket.
				new_handler->close_socket();
			}

		}else{

			// If retry_delay_seconds is 0, then don't reconnect.
			if(PARSER::retry_delay_seconds == 0)
				return;

			// Begin to reconnect.
			retry_delay_ *= 2;
			if (retry_delay_ > PARSER::max_retry_delay_seconds)
				retry_delay_ = PARSER::max_retry_delay_seconds;

			connet_fail_func(new_handler, retry_delay_);

			// Start to reconnet.
			schedule_timer(retry_delay_);

			return;
		}
	}

	void schedule_timer(int& expire_second){

		boost::posix_time::seconds s(expire_second);
		boost::posix_time::time_duration td = s;
		timer_.expires_from_now(td);
		timer_.async_wait(bind(&tcp_client::time_out_handler, this, boost::asio::placeholders::error));
	}

	void time_out_handler(const system::error_code& ec){

		if(!ec)
			start_connect();
	}

public:
	virtual int connet_success_func(pointer /*ptr*/, error_what& /*err_code*/){
		return 0;
	}

	virtual void connet_fail_func(pointer /*ptr*/, int& /*retry_delay_seconds*/){

	}

protected:
	io_service 	&ios_;
	pointer 	handler_;

	string 		ipv4_;
	u16_t    	port_;

	int retry_delay_;				// reconnect delay seconds.
	deadline_timer timer_;			// reconnect timer.
};

} }


#endif /* BINGO_TCP_CLIENT_HEADER_H_ */
