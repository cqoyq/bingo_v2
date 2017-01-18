/*
 * tcp_svr_handler.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_SVR_HANDLER_HEADER_H_
#define BINGO_TCP_SVR_HANDLER_HEADER_H_

#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/mem_guard.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

namespace bingo { namespace tcp {

template<typename PARSER,
		 typename TCP_MESSAGE_PACKAGE
		>
class tcp_svr_handler
		:public boost::enable_shared_from_this<tcp_svr_handler<PARSER, TCP_MESSAGE_PACKAGE> > {

public:

	typedef mem_guard<TCP_MESSAGE_PACKAGE> package;
	typedef boost::shared_ptr<tcp_svr_handler> pointer;

	tcp_svr_handler(boost::asio::io_service& io_service)
		: ios_(io_service),
		  socket_(io_service),
		  is_valid_(true),
		  is_authentication_pass_ (false),
		  package_size_(sizeof(TCP_MESSAGE_PACKAGE)){

		set_heartjump_datetime();

		set_authentication_pass_datetime();
	}

	virtual ~tcp_svr_handler(){
#ifdef BINGO_TCP_SERVER_DEBUG
		message_out("handler:" << this << ",destory!");
#endif
	}

	ip::tcp::socket& socket(){
		return socket_;
	}

	void start(){

		// Set authentication start to time.
		set_authentication_pass_datetime();

		// Start async read data, read finish to call read_handler().
		boost::asio::async_read(socket_,
				boost::asio::buffer(rev_mgr_.current(), PARSER::header_size),
				boost::bind(&tcp_svr_handler::read_handler,
							this->shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							PARSER::header_size));


	}

	void close_socket(){
		// This function is used to close the socket. Any asynchronous send, receive
		// or connect operations will be cancelled immediately, and will complete
		// with the boost::asio::error::operation_aborted error.
		socket_.close();
	}

	void catch_error(error_what& e_what){
		catch_error_func(this->shared_from_this(), e_what);
	}


	// Call this from tcp_handler_manager.
	bool check_heartjump_timeout(){
		if(is_authentication_pass_){
			ptime now = boost::posix_time::microsec_clock::local_time();
			ptime p1 = now - seconds(PARSER::max_wait_for_heartjump_seconds);

#ifdef BINGO_TCP_SERVER_DEBUG
			message_out("check heartjump time, p1_:" << to_iso_extended_string(p1_) <<
					",p1:" << to_iso_extended_string(p1) <<
					",p1_< p1:" << (p1_ < p1) <<
					",hdr:" << this);
#endif

			if(p1_ < p1){
				return true;
			}else{
				return false;
			}

		}else{
			return false;
		}

	}

	// Call this to update p1_ when detect the heartjump package.
	void set_heartjump_datetime(){
		p1_ = boost::posix_time::microsec_clock::local_time();

#ifdef BINGO_TCP_SERVER_DEBUG
		message_out("set heartjump time:" << to_iso_extended_string(p1_) << ",hdr:" << this);
#endif
	}



	// Call this from tcp_handler_manager.
	bool check_authentication_pass(){

		if(!is_authentication_pass_){
			// Check whether authentication pass is timeout.
			ptime now = boost::posix_time::microsec_clock::local_time();
			ptime p2 = now - seconds(PARSER::max_wait_for_authentication_pass_seconds);

			if(p2_ < p2)
				return false;
			else
				return true;

		}else{
			return true;
		}
	}

	// Call this to update is_authentication_pass_ is true, and then update heartjump datetime.
	// when detect authentication is pass.
	void set_authentication_pass(){
		is_authentication_pass_ = true;

		// Set heart-jump start to time.
		set_heartjump_datetime();
	}

	// Get is_authentication_pass_ status.
	bool get_authentication_pass(){
		return is_authentication_pass_;
	}



	void send_data_in_thread(char*& sdata, size_t& sdata_size){

		error_what e_what;
		package* new_data = 0;

		// Malloc send buffer.
		if(malloc_snd_buffer(sdata, sdata_size, new_data, e_what) == 0){

			ios_.post(bind(&tcp_svr_handler::active_send, this->shared_from_this(), new_data));
		}else{

			catch_error(e_what);

			send_close_in_thread();

		}


	}

	void send_close_in_thread(){

		ios_.post(bind(&tcp_svr_handler::active_close, this->shared_from_this()));
	}

	void active_send(package*& pk){

		if(is_valid_){


			error_what e_what;
			if(active_send_in_ioservice_func(this->shared_from_this(), pk, e_what) == -1){

				catch_error(e_what);

				free_snd_buffer(pk);

				return;
			}

			boost::asio::async_write(socket_,
					buffer(pk->header(), pk->length()),
					boost::bind(&tcp_svr_handler::write_handler,
							this->shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							pk));

		}else{

			free_snd_buffer(pk);
		}
	}



protected:


	void read_handler(const boost::system::error_code& ec,
			size_t bytes_transferred,
			size_t bytes_requested){

		if(!is_valid_)
			return;

		// Check error.
		if(ec)
		{
			close(ec);
			return;
		}

		// Set mblk's reader data length, and move rd_ptr().
		error_what e_what;
		if( rev_mgr_.change_length(bytes_transferred, e_what) == -1){

			e_what.err_no(error_tcp_package_rev_exceed_max_size);
			e_what.err_message(error_tcp_package_rev_exceed_max_size_message);
			catch_error(e_what);

			close_socket();
			close_completed(ec.value());

			return;
		}

		if(bytes_transferred < bytes_requested){

			// Continue read remain data.
			size_t remain = bytes_requested - bytes_transferred;

			boost::asio::async_read(socket_,
					buffer(rev_mgr_.current(), remain),
					bind(&tcp_svr_handler::read_handler,
							this->shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							remain));

		}else if (rev_mgr_.length() == PARSER::header_size){

			size_t remain = 0;


			char* p = rev_mgr_.header();
			if(read_pk_header_complete_func(
					this->shared_from_this(),
					p,
					rev_mgr_.length(),
					remain,
					e_what) == -1){

				// Throw error, and err_code is error_tcp_package_header_is_wrong.
				catch_error(e_what);

				close_socket();
				close_completed(ec.value());

				return;
			}


			// Check whether remain size is valid.
			if(!rev_mgr_.check_block_remain_space(remain)){

				e_what.err_no(error_tcp_package_rev_exceed_max_size);
				e_what.err_message(error_tcp_package_rev_exceed_max_size_message);
				catch_error(e_what);

				close_socket();
				close_completed(ec.value());

				return;
			}

			// Continue read other data.
			boost::asio::async_read(socket_,
					buffer(rev_mgr_.current(), remain),
					bind(&tcp_svr_handler::read_handler,
							this->shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							remain));


		}else{
			// finish to recevie Message block.

			char* p = rev_mgr_.header();
			if(read_pk_full_complete_func(
					this->shared_from_this(),
					p,
					rev_mgr_.length(),
					e_what) == -1){

				//  Throw error, and err_code is error_tcp_package_body_is_wrong.
				catch_error(e_what);

				close_socket();
				close_completed(ec.value());

				return;
			}


			rev_mgr_.clear();

			// Start new async read data.
			boost::asio::async_read(socket_,
						boost::asio::buffer(rev_mgr_.current(), PARSER::header_size),
						boost::bind(&tcp_svr_handler::read_handler,
									this->shared_from_this(),
									boost::asio::placeholders::error,
									boost::asio::placeholders::bytes_transferred,
									PARSER::header_size));
		}
	}

	void write_handler(const boost::system::error_code& ec, size_t bytes_transferred, package*& pk){


		char* p =  pk->header();
		write_pk_full_complete_func(this->shared_from_this(), p, bytes_transferred, ec);


		free_snd_buffer(pk);
	}




	void active_close(){

		if(is_valid_){
			//  Throw error, and err_code is error_tcp_server_close_socket_because_server.
			error_what e_what;
			e_what.err_no(error_tcp_server_close_socket_because_server);
			e_what.err_message(error_tcp_server_close_socket_because_server_message);

			catch_error(e_what);

			close_socket();
		}
	}



	void close(const boost::system::error_code& ec){

		using namespace boost::system::errc;
		if(ec.value() != operation_canceled){

			error_what e_what;
			e_what.err_no(error_tcp_server_close_socket_because_client);
			e_what.err_message(error_tcp_server_close_socket_because_client_message);

			catch_error(e_what);

			// Passive close
			close_socket();
		}

		close_completed(ec.value());
	}

	void close_completed(int ec_value){

		is_valid_ = false;

		close_complete_func(this->shared_from_this(), ec_value);
	}





	int malloc_snd_buffer(
			char*& ori_data,
			size_t& ori_data_size,
			package*& pk,
			error_what& e_what){

		if(ori_data_size > package_size_){

#ifdef BINGO_CHECK_TCP_MALLOC_SND_BUFFER_ERROR
			{
				bingo::string_ex t;
				bingo::string_append ap;
				ap.add("package_size_:")->add(package_size_)->add(",ori_data_size:")->add(ori_data_size)->add(",data:")->add(t.stream_to_string(ori_data, ori_data_size));
				message_out_with_time(ap.to_string())
			}
#endif

			e_what.err_no(error_tcp_package_snd_exceed_max_size);
			e_what.err_message(error_tcp_package_snd_exceed_max_size_message);
			return -1;
		}

		pk = new package();
		pk->copy(ori_data, ori_data_size, e_what);

		return 0;
	}

	void free_snd_buffer(package*& pk){

		delete pk;
		pk = 0;
	}



	// Call this to update p2_.
	void set_authentication_pass_datetime(){
		p2_ = boost::posix_time::microsec_clock::local_time();

#ifdef BINGO_TCP_SERVER_DEBUG
		message_out("set authentication pass time:" << to_iso_extended_string(p2_) << ",hdr:" << this);
#endif
	}

public:
	virtual void catch_error_func(pointer /*p*/, error_what& /*e_what*/){

	}

	virtual void close_complete_func(pointer /*p*/, int& /*ec_value*/){

	}

	virtual int read_pk_header_complete_func(
			pointer /*p*/,
			char*& /*rev_data*/,
			size_t& /*rev_data_size*/,
			size_t& /*remain_size*/,
			error_what& /*e_what*/){

		return 0;
	}

	virtual int read_pk_full_complete_func(
			pointer /*p*/,
			char*& /*rev_data*/,
			size_t& /*rev_data_size*/,
			error_what& /*e_what*/){
		return 0;
	}

	virtual void write_pk_full_complete_func(
			pointer /*p*/,
			char*& /*snd_p*/,
			size_t& /*snd_size*/,
			const boost::system::error_code& /*ec*/){

	}

	virtual int active_send_in_ioservice_func(
			pointer /*p*/,
			package*& /*pk*/,
			error_what& /*e_what*/){

		return 0;
	}

private:
	boost::asio::io_service& ios_;
	ip::tcp::socket socket_;

	bool is_valid_;

	package rev_mgr_;
	size_t package_size_;

	ptime p1_;						// Save heart jump datetime.

	ptime p2_;						// Save authentication pass datetime.
	bool is_authentication_pass_;   // When authentication pass, this is true, default is false.
};

} }

#endif /* BINGO_TCP_SVR_HANDLER_HEADER_H_ */
