/*
 * define.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_DEFINE_HEADER_H_
#define BINGO_DEFINE_HEADER_H_

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/thread.hpp>
using namespace boost;

namespace bingo {

#define message_out_with_thread(str) cout << "thr_id:" << this_thread::get_id() << "," << str << endl;

#define message_out(str) cout << str << endl;
#define message_out_with_time(str) { \
		ptime p1 = second_clock::local_time(); \
		cout << str << ", time:" << to_iso_extended_string(p1) << endl; \
		}

}

#endif /* BINGO_DEFINE_HEADER_H_ */
