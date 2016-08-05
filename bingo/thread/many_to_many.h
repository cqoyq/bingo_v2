/*
 * one_to_one.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_THREAD_MANY_TO_MANY_HEADER_H_
#define BINGO_THREAD_MANY_TO_MANY_HEADER_H_

#include "../define.h"
#include "thread_error_code.h"
#include "thread_data_type.h"

#include <queue>
using namespace std;

#include <boost/function.hpp>
#include <boost/thread.hpp>
using namespace boost;

namespace bingo { namespace thread {

template<typename TASK_MESSAGE_DATA,
		 typename TASK_EXIT_DATA>
class many_to_many{
public:
	typedef function<void(TASK_MESSAGE_DATA*& data)> 		thr_top_callback;

	many_to_many(thr_top_callback& f, int thread_n):
		f_(f),
		exit_data_(0),
		thread_active_num_(thread_n),
		is_thread_exit_(false){

		// Make thread_n new thread to read queue.
		for(int i = 0;i < thread_n; i++)
			thr_group_.create_thread(bind(&many_to_many::svc, this));
	}

	virtual ~many_to_many(){
		// Send message to exit thread.
		int err_code = 0;
		int suc = -1;
		TASK_MESSAGE_DATA* p = 0;

		for (int i = 0; i < thread_active_num_; ++i) {

			do{
				p = (TASK_MESSAGE_DATA*)(new TASK_EXIT_DATA());
				suc = put(p, err_code);
				if(suc == -1 && err_code == error_thread_svc_has_exited)
					break;
			}while(suc == -1);

			if(suc == -1){
				free_message(p);
			}
		}

		// Block until the thread exit.
		if(suc == 0){
#ifdef BINGO_THREAD_TASK_DEBUG
			message_out_with_thread("wait for all thread exit,  ~many_to_many()")
#endif
			thr_group_.join_all();
#ifdef BINGO_THREAD_TASK_DEBUG
			message_out_with_thread("all thread exit,  ~many_to_many()")
#endif
		}else{

#ifdef BINGO_THREAD_TASK_DEBUG
			message_out_with_thread("all thread has exit,  ~many_to_many()")
#endif
		}


	}

	int put(TASK_MESSAGE_DATA*& data, int& err_code){

		{
			// lock part field.
			mutex::scoped_lock lock(mu_);

			// Check is_thread_exit, the value is true that thread exit.
			if(is_thread_exit_) {
				err_code = error_thread_svc_has_exited;
				return -1;
			}

			// Condition is satisfy, then stop to wait.
			queue_.push(data);
		}

		// notify to read data from queue.
		cond_get_.notify_all();

		return 0;
	}

private:



	// Thread call the method to output xhm_base_message_block from queue.
	void svc(){

#ifdef BINGO_THREAD_TASK_DEBUG
		message_out_with_thread("call svc()!")
#endif

		while(true){

			TASK_MESSAGE_DATA* data_p = 0;

			{

				// lock part field.
				mutex::scoped_lock lock(mu_);

				// Check buffer whether empty.
				bool is_empty = (queue_.size() ==0)?true:false;
				while(is_empty)
				{
					if(is_thread_exit_){
#ifdef BINGO_THREAD_TASK_DEBUG
						message_out_with_thread("is_thread_exit_ is true in is_empty()")
#endif
						return;
					}

#ifdef BINGO_THREAD_TASK_DEBUG
					message_out_with_thread("start cond_get_.wait(mu_)!")
#endif
					// wait for notify.
					cond_get_.wait(mu_);

					is_empty = (queue_.size() ==0)?true:false;
				}

				// Condition is satisfy, then stop to wait.
				// TASK_DATA top from queue.
				data_p = queue_.front();
				queue_.pop();

				if(data_p->type == thread_data_type_exit_thread){
					thread_active_num_--;
					is_thread_exit_ = (thread_active_num_ == 0)?true:false;
#ifdef BINGO_THREAD_TASK_DEBUG
					message_out_with_thread("received thread_data_type_exit_thread, and exit!")
#endif
					free_message(data_p);
					break;
				}
			}

			if(data_p->type != thread_data_type_exit_thread){

				// Call top_callback method.
				f_(data_p);

				free_message(data_p);
			}
		}
	}

	void free_message(TASK_MESSAGE_DATA*& p){

		delete p;
		p = 0;
	}

private:
	thr_top_callback f_;

	// The thread call svc().
	thread_group thr_group_;
	TASK_EXIT_DATA* exit_data_;
	bool is_thread_exit_;
	int thread_active_num_;			// Amount of active thread.

	queue<TASK_MESSAGE_DATA*> queue_;

	mutex mu_;
	condition_variable_any cond_get_;

};

} }


#endif /* BINGO_THREAD_MANY_TO_MANY_HEADER_H_ */
