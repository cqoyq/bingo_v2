/*
 * many_to_one_1.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/string.h"
#include "../../bingo/singleton.h"
#include "../../bingo/thread/all.h"
using namespace bingo::thread;
using namespace bingo;

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost;
using namespace boost::posix_time;

BOOST_AUTO_TEST_SUITE(test_thread_many_to_many_t2)

// --------------------------- message ------------------------- //

#pragma pack(1)
struct mypackage{
	char message[256];
};
#pragma pack()

typedef task_message_data<mypackage> 	my_data_message;
typedef task_exit_data				    my_exit_message;

// --------------------------- many_to_one ------------------------- //

typedef many_to_many<my_data_message,
					my_exit_message
	> my_task_by_thread;
typedef bingo::singleton_v2<my_task_by_thread, my_task_by_thread::thr_top_callback, int> my_task;

void top(my_data_message*& data){
}

void run_input(){

	for (int i = 0; i < 4; ++i) {

		this_thread::sleep(seconds(2));

		my_data_message* msg = new my_data_message();
		int err_code = 0;

		msg->type = thread_data_type_exit_thread;


		if(my_task::instance()->put(msg, err_code) == -1){			// Input T into queue

			string_ex t;
			message_out_with_thread("put error, err_code:" << err_code << ",data:" << t.stream_to_string(&msg->data.object()->message[0], 4))

			delete msg;

		}else{

			string_ex t;
			message_out_with_thread("put success, data:" << t.stream_to_string(&msg->data.object()->message[0], 4))
		}
	}
}

BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top,					// thread_task queue top callback
			4						// amount of svc() thread
			);


	boost::thread t1(run_input);
	t1.join();

	this_thread::sleep(seconds(4));	// Must wait for thread exit!

	my_task::release();

	// output
	//	thr_id:7f54700,call svc()!
	//	thr_id:7f54700,start cond_get_.wait(mu_)!
	//	thr_id:7553700,call svc()!
	//	thr_id:7553700,start cond_get_.wait(mu_)!
	//	thr_id:8955700,call svc()!
	//	thr_id:8955700,start cond_get_.wait(mu_)!
	//	thr_id:9356700,call svc()!
	//	thr_id:9356700,start cond_get_.wait(mu_)!

	//	thr_id:9d57700,put success, data:00 00 00 00
	//	thr_id:7f54700,queue pop succes! type:1
	//	thr_id:7f54700,received thread_data_type_exit_thread, and exit!
	//	thr_id:9356700,start cond_get_.wait(mu_)!
	//	thr_id:8955700,start cond_get_.wait(mu_)!
	//	thr_id:7553700,start cond_get_.wait(mu_)!

	//	thr_id:9d57700,put success, data:00 00 00 00
	//	thr_id:9356700,queue pop succes! type:1
	//	thr_id:9356700,received thread_data_type_exit_thread, and exit!
	//	thr_id:8955700,start cond_get_.wait(mu_)!
	//	thr_id:7553700,start cond_get_.wait(mu_)!

	//	thr_id:9d57700,put success, data:00 00 00 00
	//	thr_id:8955700,queue pop succes! type:1
	//	thr_id:8955700,received thread_data_type_exit_thread, and exit!
	//	thr_id:7553700,start cond_get_.wait(mu_)!

	//	thr_id:9d57700,put success, data:00 00 00 00
	//	thr_id:7553700,queue pop succes! type:1
	//	thr_id:7553700,received thread_data_type_exit_thread, and exit!

	//	thr_id:674a100,destory ~many_to_many()


}

BOOST_AUTO_TEST_SUITE_END()


