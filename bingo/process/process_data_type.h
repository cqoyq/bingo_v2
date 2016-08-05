/*
 * process_data_type.h
 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#ifndef BINGO_PROCESS_PROCESS_DATA_TYPE_H_
#define BINGO_PROCESS_PROCESS_DATA_TYPE_H_

#include "../type.h"
#include "../mem_guard.h"

namespace bingo { namespace process {

enum{
	process_data_type_data  			= 0x00,
	process_data_type_exit_thread  		= 0x01,
};

#pragma pack(1)
template<typename PARSER>
struct task_data{
	char message[PARSER::message_size];		// Task message
};
#pragma pack()


template<typename PARSER>
struct task_message_data {
	u8_t type;									// Type of task data
	mem_guard<task_data<PARSER> > data;			// Task message

	task_message_data():type(process_data_type_data){}
	task_message_data(u8_t t):type(t){}
};

template<typename PARSER>
struct task_exit_data : public task_message_data<PARSER> {
	task_exit_data():task_message_data<PARSER>(process_data_type_exit_thread){}
};

} }


#endif /* BINGO_PROCESS_PROCESS_DATA_TYPE_H_ */
