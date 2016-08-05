/*
 * tcp_handler_data.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_HANDLER_DATA_HEADER_H_
#define BINGO_TCP_HANDLER_DATA_HEADER_H_

namespace bingo { namespace tcp {

// tcp_handler_data: The struct save handler information.
struct tcp_handler_data{
	void* handler_pointer;		// Pointer to tcp_connection.

	tcp_handler_data(void* p):handler_pointer(p){}
};

} }


#endif /* BINGO_TCP_HANDLER_DATA_HEADER_H_ */
