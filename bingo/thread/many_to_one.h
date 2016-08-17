/*
 * one_to_one.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_THREAD_MANY_TO_ONE_HEADER_H_
#define BINGO_THREAD_MANY_TO_ONE_HEADER_H_

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
class many_to_one{
public:
	typedef function<void(TASK_MESSAGE_DATA*& data)> 		thr_top_callback;
	typedef function<void()> 								thr_init_callback;

	many_to_one(thr_top_callback& f):
		f_(f),
		f1_(0),
		exit_data_(0),
		is_thread_exit_(false),
		thr_(bind(&many_to_one::svc, this)){}

	many_to_one(thr_top_callback& f, thr_init_callback& f1):
		f_(f),
		f1_(f1),
		exit_data_(0),
		is_thread_exit_(false),
		thr_(bind(&many_to_one::svc, this)){}

	virtual ~many_to_one(){

		// Send message to exit thread.
		int err_code = 0;
		int suc = -1;
		TASK_MESSAGE_DATA* p = 0;
		do{
			p = (TASK_MESSAGE_DATA*)(new TASK_EXIT_DATA());
			suc = put(p, err_code);
			if(suc == -1 && err_code == error_thread_svc_has_exited)
				break;
		}while(suc == -1);

		// Block until the thread exit.
		if(suc == 0){
#ifdef BINGO_THREAD_TASK_DEBUG
			message_out_with_thread("wait for thread exit,  ~one_to_one()")
#endif
			thr_.join();
#ifdef BINGO_THREAD_TASK_DEBUG
			message_out_with_thread("thread exit,  ~one_to_one()")
#endif
		}else{
			free_message(p);
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
		cond_get_.notify_one();

		return 0;
	}

private:



	// Thread call the method to output xhm_base_message_block from queue.
	void svc(){

#ifdef BINGO_THREAD_TASK_DEBUG
		message_out_with_thread("call svc()!")
#endif

		if(f1_) f1_();

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

#ifdef BINGO_THREAD_TASK_DEBUG
				message_out_with_thread("queue pop succes! type:" << (int)data_p->type)
#endif

				if(data_p->type == thread_data_type_exit_thread){
					is_thread_exit_ = true;
#ifdef BINGO_THREAD_TASK_DEBUG
					message_out_with_thread("received thread_data_type_exit_thread")
#endif
				}
			}

			if(data_p->type != thread_data_type_exit_thread){

				// Call top_callback method.
				f_(data_p);
			}

			free_message(data_p);
		}
	}

	void free_message(TASK_MESSAGE_DATA*& p){

		delete p;
		p = 0;
	}

private:
	thr_top_callback f_;
	thr_init_callback f1_;

	// The thread call svc().
	boost::thread thr_;
	TASK_EXIT_DATA* exit_data_;
	bool is_thread_exit_;

	queue<TASK_MESSAGE_DATA*> queue_;

	mutex mu_;
	condition_variable_any cond_get_;

};

} }


#endif /* BINGO_THREAD_MANY_TO_ONE_HEADER_H_ */
