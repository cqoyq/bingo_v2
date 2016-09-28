/*
 * shm_rep.h
 *
 *  Created on: 2016-7-5
 *      Author: root
 */

#ifndef BINGO_PROCESS_SHARED_MEMORY_SHM_RECEIVER_HEADER_H_
#define BINGO_PROCESS_SHARED_MEMORY_SHM_RECEIVER_HEADER_H_

#include "bingo/define.h"
#include "bingo/error_what.h"
#include "bingo/process/process_data_type.h"
using namespace bingo;

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
class shm_receiver {
public:
	typedef boost::function<void(char*&)> 								rev_succ_callback;
	typedef boost::function<void(error_what&,  interprocess_exception&)> 	rev_error_callback;

	shm_receiver(rev_succ_callback& f1, rev_error_callback& f2):
		f1_(f1),
		f2_(f2),
		thr_(boost::bind(&shm_receiver::svc, this)){

	}

	virtual ~shm_receiver(){

		send_exit_thread();
		thr_.join();

#ifdef BINGO_PROCESS_SHARED_MEMORY_DEBUG
		message_out("thr: exit success")
#endif
		remove();
	}

protected:

	int remove(){

		bool suc = shared_memory_object::remove(PARSER::shared_memory_name);

#ifdef BINGO_PROCESS_SHARED_MEMORY_DEBUG
		message_out("remove() return " << suc)
#endif
		return (suc)? 0 : -1;
	}

	void send_exit_thread(){

		// Send exit thread message.
		error_what e_what;
		put(exit_data_, e_what);
	}

	int put(TASK_MESSAGE_DATA& msg, error_what& e_what){

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

				counter++;
			}

		}

		// Give up after 5 order
		if(counter == 5)
			 return -1;
		else
			return 0;

	}

	void svc(){

#ifdef BINGO_PROCESS_SHARED_MEMORY_DEBUG
		message_out("thr: start")
#endif

		try{
			//Create a shared memory object.
			shared_memory_object shm
			  (open_or_create                	//create
			  ,PARSER::shared_memory_name    	//name
			  ,read_write                     	//read-write mode
			  );

			 //Set size
			 shm.truncate(sizeof(shm_data<PARSER>));

			//Map the whole shared memory in this process
			mapped_region region
					 (shm                       //What to map
					 ,read_write 				//Map it as read-write
					 );

			//Get the address of the mapped region
			void * addr       = region.get_address();

			//Construct the shared structure in memory
			shm_data<PARSER> * data = new (addr) shm_data<PARSER>;

#ifdef BINGO_PROCESS_SHARED_MEMORY_DEBUG
			message_out("thr: shm create success!")
#endif

			//Print messages until the other process marks the end
			bool end_loop = false;
			do{
				scoped_lock<interprocess_mutex> lock(data->mutex);
				if(!data->message_in){
					data->cond_empty.wait(lock);
				}

				if(data->type == thread_data_type_exit_thread){
					end_loop = true;
				}
				else{
					//Call rev_callback.
					char* p = &data->items[0];
					if(f1_) f1_(p);
					//Notify the other process that the buffer is empty
					data->message_in = false;
					data->cond_full.notify_one();
				}
			}
			while(!end_loop);
		}
		catch(interprocess_exception &ex){

			error_what e_what;
			e_what.err_no(error_process_task_receive_data_fail);
			e_what.err_message(error_process_task_receive_data_fail_message);

			if(f2_) f2_(e_what, ex);
		}

	}

private:
	process::task_exit_data<PARSER> exit_data_;
	boost::thread thr_;

	rev_succ_callback  f1_;
	rev_error_callback f2_;
};

} } }

#endif /* BINGO_PROCESS_SHARED_MEMORY_SHM_RECEIVER_HEADER_H_ */
