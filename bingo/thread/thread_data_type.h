/*
 * thread_data_type.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_
#define BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_

#include "bingo/type.h"
#include "bingo/mem_guard.h"

namespace bingo { namespace thread {

enum{
	thread_data_type_data  			= 0x00,
	thread_data_type_exit_thread  	= 0x01,
};


template<typename MESSAGE>
struct task_message_data {
	u8_t type;							// Type of task data
	mem_guard<MESSAGE> data;			// Task message

	task_message_data():type(thread_data_type_data){}
	task_message_data(u8_t t):type(t){}
};

#pragma pack(1)
struct task_data_placeholder{
	char message[1];
};
#pragma pack()


struct task_exit_data : public task_message_data<task_data_placeholder> {
	task_exit_data():task_message_data<task_data_placeholder>(thread_data_type_exit_thread){}
};

} }

#endif /* BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_ */
