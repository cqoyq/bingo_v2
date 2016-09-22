/*
 * db_factory.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_FACTORY_H_
#define BINGO_DATABASE_DB_FACTORY_H_

#include <string>
using namespace std;

#include "db_connector.h"

namespace bingo { namespace database {

class db_factory {
public:
	db_factory();
	virtual ~db_factory();

	// Create mysql connector. return true if success. fail to call err() check error.
	// @num is number of connector.
	bool make_mysql_connector(int num, db_connector* conn);

	// Return error information.
	string& err();

private:
	string err_;
};

} }

#endif /* BINGO_DATABASE_DB_FACTORY_H_ */
