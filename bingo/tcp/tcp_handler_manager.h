/*
 * tcp_handler_manager.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_HANDLER_MANAGER_HEADER_H_
#define BINGO_TCP_HANDLER_MANAGER_HEADER_H_

#include "../type.h"

#include "tcp_handler_data.h"
#include "tcp_error_code.h"

#include <vector>
#include <iostream>
using namespace std;

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread/thread.hpp>
using namespace boost;

namespace bingo { namespace tcp {

// tcp_handler_manager's filters.
struct find_first_tcp_handler_same_pointer{
	find_first_tcp_handler_same_pointer(const void* p):p_(p){}

	bool operator()(tcp_handler_data& n){
		return (n.handler_pointer == p_)?true: false;
	}
private:
	const void* p_;
};

template<class HANDLER>
class tcp_handler_manager {
public:

	tcp_handler_manager(){}

	// Push tcp_handler pointer into the container,
	void push(void* hdr){

		// lock part field.
		mutex::scoped_lock lock(mu_);

		sets_.push_back(new tcp_handler_data(hdr));
	}

	// Erase tcp_handler pointer from the container,
	// return 0 if do success, otherwise return -1.
	int erase(const void* hdr, u8_t& err_code){

		// lock part field.
		mutex::scoped_lock lock(mu_);

		// Find the first element which member handler_pointer is same with p.
		set_iterator iter = boost::find_if(sets_, find_first_tcp_handler_same_pointer(hdr));

		if(iter != sets_.end()){
			sets_.erase(iter);

			err_code = 0;
			return 0;
		}else{

			err_code = error_tcp_handler_mgr_element_no_exist;
			return  -1;
		}
	}

	// Send data in another thread, return 0 if do success,
	// otherwise return -1.
	int send_data(void* hdr, const char* data, size_t data_size, u8_t& err_code){

		// lock part field.
		mutex::scoped_lock lock(mu_);


		// Find the first element which member handler_pointer is same with p.
		set_iterator iter = boost::find_if(sets_, find_first_tcp_handler_same_pointer(hdr));

		if(iter != sets_.end()){
			HANDLER* p0 = static_cast<HANDLER*>((*iter).handler_pointer);

			if(p0){
				char* d = const_cast<char*>(data);
				p0->send_data_in_thread(d, data_size);
			}
			return 0;
		}else{

			err_code = error_tcp_handler_mgr_element_no_exist;
			return  -1;
		}
	}

	// Call handler's send_close() in another thread, return 0 if do success,
	// otherwise return -1.
	int send_close(void* hdr, u8_t& err_code){

		// lock part field.
		mutex::scoped_lock lock(mu_);


		// Find the first element which member handler_pointer is same with p.
		set_iterator iter = boost::find_if(sets_, find_first_tcp_handler_same_pointer(hdr));

		if(iter != sets_.end()){

			HANDLER* p0 = static_cast<HANDLER*>((*iter).handler_pointer);

			if(p0){
				p0->send_close_in_thread(error_tcp_server_close_socket_because_server);
			}
			return 0;
		}else{

			err_code = error_tcp_handler_mgr_element_no_exist;
			return  -1;
		}
	}

	size_t size(){

		// lock part field.
		mutex::scoped_lock lock(mu_);

		return sets_.size();
	}

protected:
	mutex mu_;

	boost::ptr_vector<tcp_handler_data> sets_;
	typedef boost::ptr_vector<tcp_handler_data>::iterator set_iterator;

};

} }


#endif /* BINGO_TCP_HANDLER_MANAGER_HEADER_H_ */
