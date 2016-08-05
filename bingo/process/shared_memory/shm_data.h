/*
 * shm_data.h
 *
 *  Created on: 2016-7-5
 *      Author: root
 */

#ifndef BINGO_PROCESS_SHARED_MEMORY_SHM_DATA_HEADER_H_
#define BINGO_PROCESS_SHARED_MEMORY_SHM_DATA_HEADER_H_

#include "../../type.h"
#include "../../thread/thread_data_type.h"
using namespace bingo::thread;

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
using namespace boost::interprocess;

namespace bingo { namespace process { namespace shared_memory {

template<typename PARSER>
struct shm_data
{
   enum { LineSize = PARSER::message_size };

   shm_data()
      :  message_in(false), type(thread_data_type_data)
   {}

   //Mutex to protect access to the queue
   boost::interprocess::interprocess_mutex      mutex;

   //Condition to wait when the queue is empty
   boost::interprocess::interprocess_condition  cond_empty;

   //Condition to wait when the queue is full
   boost::interprocess::interprocess_condition  cond_full;

   //Items to fill
   char   items[LineSize];

   //Is there any message
   bool message_in;

   u8_t type;
};

} } }


#endif /* BINGO_PROCESS_SHARED_MEMORY_SHM_DATA_HEADER_H_ */
