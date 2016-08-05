/*
 * error_code.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_ERROR_CODE_HEADER_H_
#define BINGO_TCP_ERROR_CODE_HEADER_H_

namespace bingo { namespace tcp {

enum {
		error_tcp_package_rev_exceed_max_size						= 0x01,
		error_tcp_package_queue_is_empty							= 0x02,
		error_tcp_package_header_is_wrong							= 0x03,
		error_tcp_package_body_is_wrong								= 0x04,
		error_tcp_package_snd_exceed_max_size						= 0x05,

		error_tcp_server_close_socket_because_client				= 0x06,
		error_tcp_server_close_socket_because_heartjump				= 0x07,
		error_tcp_server_close_socket_because_authrication_pass		= 0x08,
		error_tcp_server_close_socket_because_server				= 0x09,

		error_tcp_client_close_socket_because_server				= 0x0A,
		error_tcp_client_close_socket_because_self					= 0x0B,
		error_tcp_client_close_socket_because_make_first_package	= 0x0C,



		error_tcp_handler_mgr_sequence_is_full   		 			= 0x10,
		error_tcp_handler_mgr_element_no_exist						= 0x11,

	};

} }


#endif /* BINGO_TCP_ERROR_CODE_HEADER_H_ */
