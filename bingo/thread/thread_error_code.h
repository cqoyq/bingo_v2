/*
 * thread_error_code.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_THREAD_THREAD_ERROR_CODE_HEADER_H_
#define BINGO_THREAD_THREAD_ERROR_CODE_HEADER_H_

namespace bingo { namespace thread {

enum {
	error_thread_svc_has_exited 	= 0x01,
};

#define error_thread_svc_has_exited_message "the thread has exited!"

} }


#endif /* BINGO_THREAD_THREAD_ERROR_CODE_HEADER_H_ */
