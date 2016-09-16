/*
 * error_code.h
 *
 *  Created on: 2016-7-5
 *      Author: root
 */

#ifndef BINGO_PROCESS_SHARED_MEMORY_ERROR_CODE_HEADER_H_
#define BINGO_PROCESS_SHARED_MEMORY_ERROR_CODE_HEADER_H_

namespace bingo { namespace process { namespace shared_memory {

enum {
	error_process_task_send_data_fail		= 0x01,
	error_process_task_receive_data_fail	= 0x02,
};

#define error_process_task_send_data_fail_message "the process task send data fail!"
#define error_process_task_receive_data_fail_message "the process taks receive data fail!"

} } }


#endif /* BINGO_PROCESS_SHARED_MEMORY_ERROR_CODE_HEADER_H_ */
