/*
 * tcp_cet_hdr_manager.h
 *
 *  Created on: 2016-7-1
 *      Author: root
 */

#ifndef BINGO_TCP_CET_HDR_MANAGER_HEADER_H_
#define BINGO_TCP_CET_HDR_MANAGER_HEADER_H_

#include "tcp_handler_manager.h"

namespace bingo { namespace tcp {

template<class HANDLER>
class tcp_cet_hdr_manager : public tcp_handler_manager<HANDLER>{
public:
	tcp_cet_hdr_manager() : tcp_handler_manager<HANDLER>(){}
	virtual ~tcp_cet_hdr_manager(){}
};

} }

#endif /* BINGO_TCP_CET_HDR_MANAGER_HEADER_H_ */
