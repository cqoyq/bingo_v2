/*
 * mysql_pool.h
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_POOL_H_
#define BINGO_DATABASE_MYSQL_MYSQL_POOL_H_

#include "bingo/singleton.h"
#include "bingo/error_what.h"
#include "bingo/database/db_connector.h"
#include "mysql_connector.h"

#include <queue>
using namespace std;

#include <boost/thread.hpp>
#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio;

namespace bingo { namespace database { namespace mysql {

class mysql_pool {
public:
	typedef bingo::database::mysql::mysql_connector connector;
	mysql_pool();
	virtual ~mysql_pool();

	// Make pool success to return 0, otherwise return -1, fail to check err().
	int make_connector_pool(size_t n, db_connector* conn_info);

	// Return error information.
	error_what& err();

	// Pop connector from pool.
	connector* get_connector();

	//  Push connector to pool.
	void free_connector(connector*& conn);

	// Obtain pool size.
	size_t size();

	// Check heart-jump.
	void check_heartjump();

protected:
	mutex mu_;

	queue<connector*> conns_;
	io_service ios_;

	error_what err_;
};

typedef bingo::singleton_v0<mysql_pool> MYSQL_POOL_TYPE;

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_POOL_H_ */
