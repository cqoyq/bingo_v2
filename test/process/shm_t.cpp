/*
 * t1.cpp
 *
 *  Created on: 2016-8-5
 *      Author: root
 */


#include <boost/test/unit_test.hpp>

#include "../../bingo/string.h"
#include "../../bingo/process/all.h"
using namespace bingo::process;

BOOST_AUTO_TEST_SUITE(test_process_shm_t1)

struct my_parser{

	static const char* shared_memory_name;		// Shared memory name
	static const u32_t message_size;			// Size of block
};

const char* my_parser::shared_memory_name = "myshm";
const u32_t my_parser::message_size = 12;

////////////////////////////////////////////// sender define ///////////////////////////////////////////////////

typedef process::task_message_data<my_parser> my_message_type;

typedef shm_sender<
			my_parser,
			my_message_type
		> my_sender_type;

typedef bingo::singleton_v1<
		my_sender_type,
		my_sender_type::snd_error_callback
		> my_sender;

void snd_error(error_what& e_what, interprocess_exception& ex){
	cout << "snd_error(), err_code:" << e_what.err_no() << ",exception:" << ex.what() << endl;
	message_out_with_time("snd_error");
}


#define MAX_NUMBER 1000000
u64_t send_total_num = 0;

void send_data(){

	char data[12];
	for (int i = 0; i < MAX_NUMBER; ++i) {
		memset(&data[0], 0x00, 12);
		memcpy(&data[0], &i, 4);

		my_message_type msg;
		bingo::error_what e_what;
		msg.data.copy(&data[0], 12, e_what);

		int result = my_sender::instance()->put(msg, e_what);


		send_total_num++;

		string_ex t;
		cout << "send result:"<< result << ",n:" << send_total_num << ",data:" << t.stream_to_string(msg.data.header(), 12) << endl;
	}

}

BOOST_AUTO_TEST_CASE(t_sendor){

	cout << "my_parser size:" << sizeof(my_parser) << endl;

	my_sender::construct(snd_error);

	send_data();

	while(send_total_num != MAX_NUMBER){
		this_thread::sleep(seconds(5));
	}

	my_sender::release();
}

////////////////////////////////////////////// receive define ///////////////////////////////////////////////////

typedef shm_receiver<
			my_parser,
			my_message_type
		> my_receiver_type;

typedef bingo::singleton_v2<
		my_receiver_type,
		my_receiver_type::rev_succ_callback,
		my_receiver_type::rev_error_callback
		> my_receiver;

u64_t receive_total_num = 0;

void rev(char*& data){
	string_ex t;
	cout << "rev(), data:" << t.stream_to_string(data, my_parser::message_size) << endl;

	receive_total_num++;
}

void rev_error(error_what& e_what, interprocess_exception& ex){
	cout << "rev_error(), err_code:" << e_what.err_no() << ",exception:" << ex.what() << endl;
}

BOOST_AUTO_TEST_CASE(t_receiver){

	my_receiver::construct(rev, rev_error);

	while(receive_total_num != MAX_NUMBER){
		this_thread::sleep(seconds(5));
	}

	my_receiver::release();

	cout << "receive_total_num:" << receive_total_num << endl;
}


BOOST_AUTO_TEST_SUITE_END()
