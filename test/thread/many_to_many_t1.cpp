/*
 * many_to_one_1.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/singleton.h"
#include "../../bingo/thread/all.h"
using namespace bingo::thread;

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost;
using namespace boost::posix_time;

BOOST_AUTO_TEST_SUITE(test_thread_many_to_many_t1)

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

BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top,					// thread_task queue top callback
			4						// amount of svc() thread
			);

	this_thread::sleep(seconds(4));

	my_task::release();

	// output
	//	thr_id:8955700,call svc()!
	//	thr_id:8955700,start cond_get_.wait(mu_)!
	//	thr_id:7553700,call svc()!
	//	thr_id:7553700,start cond_get_.wait(mu_)!
	//	thr_id:7f54700,call svc()!
	//	thr_id:7f54700,start cond_get_.wait(mu_)!
	//	thr_id:9356700,call svc()!
	//	thr_id:9356700,start cond_get_.wait(mu_)!

	//	thr_id:674a100,destory ~many_to_many()


}

BOOST_AUTO_TEST_SUITE_END()


