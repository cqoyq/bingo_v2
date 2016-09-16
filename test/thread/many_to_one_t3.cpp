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

#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
using namespace std;

BOOST_AUTO_TEST_SUITE(test_thread_many_to_one_t3)

// --------------------------- message ------------------------- //

#pragma pack(1)
struct mypackage{
	char message[256];
};
#pragma pack()

typedef task_message_data<mypackage> 	my_data_message;
typedef task_exit_data				    my_exit_message;

// --------------------------- many_to_one ------------------------- //

typedef many_to_one<my_data_message,
					my_exit_message
	> my_task_by_thread;
typedef bingo::singleton_v1<my_task_by_thread, my_task_by_thread::thr_top_callback> my_task;

// Output to file './mydata'
void top(my_data_message*& msg){

	string_ex t;
	string out = t.stream_to_string(&msg->data.object()->message[0], 4);

	string d(".");
	string f("mydata");

	try {

		{


			// whether LOG_DIRECTORY is exist.
			if(access(d.c_str(), 0) == -1){
				// the directory isn't exist.
				// make new directory.
				mkdir(d.c_str(), S_IRWXU + S_IRGRP + S_IXGRP + S_IXOTH);
			}
		}


		// get filename.
		string filename(d.c_str());
		filename.append("/");
		filename.append(f.c_str());


		{


			// whether file is exist.
			if(access(filename.c_str(), 0) == -1){
				// the file isn't exist.
				// make new file.

				ofstream fs(filename.c_str());
				fs << out.c_str() << endl;
				fs.close();

			}else{

				ofstream fs(filename.c_str(), ios_base::app);
				fs << out.c_str() << endl;
				fs.close();
			}
		}


	}catch(std::exception& err){
	}
}

void run_input(int& n){

	for (int i = 0; i < 100000; ++i) {

		my_data_message* msg = new my_data_message();
		error_what e_what;

		msg->data.object()->message[0] = (u8_t)n;
//		msg->data.object()->message[1] = (u8_t)i;

		if(my_task::instance()->put(msg, e_what) == -1){			// Input T into queue

			bingo::string_ex t;
			message_out_with_thread("put error, err_code:" << e_what.err_no() << ",data:" << t.stream_to_string(&msg->data.object()->message[0], 4))

			delete msg;

		}else{

			bingo::string_ex t;
			message_out_with_thread("put success, data:" << t.stream_to_string(&msg->data.object()->message[0], 4))
		}
	}
}

BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top					// thread_task queue top callback
			);

	boost::thread t1(run_input, 1);
	boost::thread t2(run_input, 2);
	boost::thread t3(run_input, 3);
	boost::thread t4(run_input, 4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	this_thread::sleep(seconds(4));

	my_task::release();

	// output
	// There are 100000 record every thread in log's file.
}

BOOST_AUTO_TEST_SUITE_END()


