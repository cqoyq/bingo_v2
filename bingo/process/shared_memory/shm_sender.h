/*
 * shm_rep.h
 *
 *  Created on: 2016-7-5
 *      Author: root
 */

#ifndef BINGO_PROCESS_SHARED_MEMORY_SHM_SENDER_HEADER_H_
#define BINGO_PROCESS_SHARED_MEMORY_SHM_SENDER_HEADER_H_

#include "bingo/define.h"
#include "bingo/error_what.h"

#include "shm_data.h"
using namespace bingo::process::shared_memory;

#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
using namespace boost;
using namespace boost::interprocess;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

namespace bingo { namespace process { namespace shared_memory {

// Note: The class is no thread-saft, use only in single thread.
template<typename PARSER,
		 typename TASK_MESSAGE_DATA
		 >
class shm_sender {
public:
	typedef boost::function<void(error_what&, interprocess_exception&)> 			snd_error_callback;

	// Construct object on the side of sender.
	shm_sender(snd_error_callback& f):
		f_(f){};

	virtual ~shm_sender(){}

	int put(TASK_MESSAGE_DATA& msg, error_what& e_what){

		// Avoid to occupy cpu-time.
		this_thread::sleep(milliseconds(1));

		int counter = 0;
		while(counter < 5){

			try{
				//Create a shared memory object.
				shared_memory_object shm
				  (open_only               				//open
				  ,PARSER::shared_memory_name       	//name
				  ,read_write                			//read-write mode
				  );

				//Map the whole shared memory in this process
				mapped_region region
					 (shm                       		//What to map
					 ,read_write 						//Map it as read-write
					 );

				//Get the address of the mapped region
				void * addr       = region.get_address();

				//Obtain a pointer to the shared structure
				shm_data<PARSER>* data = static_cast<shm_data<PARSER>* >(addr);

#ifdef BINGO_PROCESS_SHARED_MEMORY_DEBUG
				message_out("create shm success")
#endif

				{
					 scoped_lock<interprocess_mutex> lock(data->mutex);

					 if(data->message_in){
						 if(!data->cond_full.timed_wait(lock, get_system_time() + milliseconds(500))){

							 e_what.err_no(error_process_task_send_data_fail);
							 e_what.err_message(error_process_task_send_data_fail_message);

							 counter++;
							 continue;
						 }
					 }



					 // Put message into shared_memory.
					 memset(&data->items[0], 0x00, PARSER::message_size);
					 memcpy(&data->items[0], msg.data.header(), msg.data.length());
					 data->type = msg.type;

					 //Mark message buffer as full
					 data->message_in = true;

					 //Notify to the other process that there is a message
					 data->cond_empty.notify_one();

					 break;
				}
			}catch(interprocess_exception &ex){
				e_what.err_no(error_process_task_send_data_fail);
				e_what.err_message(error_process_task_send_data_fail_message);

				if(f_) f_(e_what, ex);

				counter++;
			}

		}

		// Give up after 5 order
		if(counter == 5)
			 return -1;
		else
			return 0;

	}

private:

	snd_error_callback f_;
};

} } }


#endif /* BINGO_PROCESS_SHARED_MEMORY_SHM_SENDER_HEADER_H_ */
