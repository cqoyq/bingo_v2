/*
 * tcp_svr_hdr_manager.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_
#define BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_

#include "../foreach_.h"
#include "tcp_handler_manager.h"
#include "tcp_error_code.h"

namespace bingo { namespace tcp {

template<class HANDLER>
struct find_all_of_handler_because_heartjump_timeout{
	bool operator()(tcp_handler_data& n){

		HANDLER* p = static_cast<HANDLER*>(n.handler_pointer);
		if(p){
			if(p->check_heartjump_timeout()) {
				return true;
			}
		}
		return false;
	}
};



template<class HANDLER>
class tcp_svr_hdr_manager : public tcp_handler_manager<HANDLER>{
public:
	tcp_svr_hdr_manager() : tcp_handler_manager<HANDLER>(){}

	// Check whether tcp_handler is heartjump timeout.
	void check_heartjump(){

		// lock part field.
		mutex::scoped_lock lock(this->mu_);


		foreach_(tcp_handler_data& n, this->sets_ | adaptors::filtered(find_all_of_handler_because_heartjump_timeout<HANDLER>())){
			HANDLER* p = static_cast<HANDLER*>(n.handler_pointer);
			if(p){
				p->catch_error(error_tcp_server_close_socket_because_heartjump);
				p->close_socket();
			}
		}
	}

	// Check whether tcp_handler is authentication pass.
	void check_authentication_pass(){

		// lock part field.
		mutex::scoped_lock lock(this->mu_);


		foreach_(tcp_handler_data& n, this->sets_){
			HANDLER* p = static_cast<HANDLER*>(n.handler_pointer);
			if(p){
				if(!p->check_authentication_pass()){
					p->catch_error(error_tcp_server_close_socket_because_authrication_pass);
					p->close_socket();
				}
			}
		}
	}
};

} }

#endif /* BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_ */
