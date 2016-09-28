/*
 * error_what.h
 *
 *  Created on: 2016-9-16
 *      Author: root
 */

#ifndef BINGO_ERROR_WHAT_H_
#define BINGO_ERROR_WHAT_H_

#include <string>
using namespace std;

namespace bingo {

enum {
		// -------------------------------- tcp-package ------------------------------ //
		error_tcp_package_rev_exceed_max_size									= 0x01,
		error_tcp_package_queue_is_empty											= 0x02,
		error_tcp_package_header_is_wrong											= 0x03,
		error_tcp_package_body_is_wrong												= 0x04,
		error_tcp_package_snd_exceed_max_size									= 0x05,
		// -------------------------------- tcp-server ------------------------------ //
		error_tcp_server_close_socket_because_client								= 0x06,
		error_tcp_server_close_socket_because_heartjump						= 0x07,
		error_tcp_server_close_socket_because_authrication_pass			= 0x08,
		error_tcp_server_close_socket_because_server							= 0x09,
		// -------------------------------- tcp-client ------------------------------ //
		error_tcp_client_close_socket_because_server								= 0x0A,
		error_tcp_client_close_socket_because_self									= 0x0B,
		error_tcp_client_close_socket_because_make_first_package		= 0x0C,
		// -------------------------------- tcp-handler-manager ------------------------------ //
		error_tcp_handler_mgr_sequence_is_full   		 							= 0x10,
		error_tcp_handler_mgr_element_no_exist									= 0x11,

		// -------------------------------- database ------------------------------ //
		error_database_connect_fail														= 0x20,
		error_database_query_fail															= 0x21,

		// -------------------------------- process ------------------------------ //
		error_process_task_send_data_fail												= 0x30,
		error_process_task_receive_data_fail											= 0x31,

		// -------------------------------- thread ------------------------------ //
		error_thread_svc_has_exited 														= 0x38,

};
// -------------------------------- tcp-package ------------------------------ //
#define error_tcp_package_rev_exceed_max_size_message "the receive-package is exceed max size of message!"
#define error_tcp_package_queue_is_empty_message "the package queue is empty!"
#define error_tcp_package_header_is_wrong_message "the package header has error!"
#define error_tcp_package_body_is_wrong_message "the package body has error!"
#define error_tcp_package_snd_exceed_max_size_message "the send-package is exceed max size of message!"
// -------------------------------- tcp-server ------------------------------ //
#define error_tcp_server_close_socket_because_client_message "the server close socket passive!"
#define error_tcp_server_close_socket_because_heartjump_message "the server close socket because heart-jump!"
#define error_tcp_server_close_socket_because_authrication_pass_message "the server close socket because authentication!"
#define error_tcp_server_close_socket_because_server_message "the server close socket active!"
// -------------------------------- tcp-client ------------------------------ //
#define error_tcp_client_close_socket_because_server_message "the client close socket passive!"
#define error_tcp_client_close_socket_because_self_message "the client close socket active!"
#define error_tcp_client_close_socket_because_make_first_package_message " the client close socket because make first package error!"
// -------------------------------- tcp-handler-manager ------------------------------ //
#define error_tcp_handler_mgr_sequence_is_full_message "the handler manager queue is full!"
#define error_tcp_handler_mgr_element_no_exist_message "the handler isn't exist in handler manager queue!"

// -------------------------------- database ------------------------------ //
#define error_database_connect_fail_message 	"database connect fail!"
#define error_database_query_fail_message 	"database query fail!"

// -------------------------------- process ------------------------------ //
#define error_process_task_send_data_fail_message "the process task send data fail!"
#define error_process_task_receive_data_fail_message "the process taks receive data fail!"

// -------------------------------- thread ------------------------------ //
#define error_thread_svc_has_exited_message "the thread has exited!"


// Error information class.
class error_what {
public:
	error_what();
	virtual ~error_what();

	string& err_message();

	void err_message(const char* err);

	int err_no();

	void err_no(int n);

private:
	string err_;
	int err_no_;
};
}


#endif /* BINGO_ERROR_WHAT_H_ */
