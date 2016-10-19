/*
 * mysql_timer.h
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_TIMER_H_
#define BINGO_DATABASE_MYSQL_MYSQL_TIMER_H_

#include "mysql_pool.h"

#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio;

namespace bingo { namespace database { namespace mysql {

class mysql_timer {
public:
	mysql_timer(io_service& io, mysql_pool* pool);
	virtual ~mysql_timer();

	// Start to timer.
	void schedule_timer();

protected:
	// Time out handle method.
	void time_out_handler(const system::error_code& ec);

	// Call schedule_timer() to Start Timer.
	deadline_timer timer_;

	enum{
		MAX_TIME_INTERVAL_MINUTE = 1
	};

	// Save pool.
	mysql_pool* pool_;
};

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_TIMER_H_ */
