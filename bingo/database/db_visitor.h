/*
 * db_visitor.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_VISITOR_H_
#define BINGO_DATABASE_DB_VISITOR_H_

#include <boost/ptr_container/ptr_vector.hpp>

#include "db_result.h"
#include "bingo/error_what.h"

namespace bingo { namespace database {

class db_visitor {
public:
	virtual ~db_visitor(){};

	// Query single result, return 0 if success.
	// @sql is single sql-string, @result is query result, @e_what is error.
	virtual int query_result(const char* sql, db_result*& result, error_what& e_what) = 0;

	// Query multi-result,  return 0 if success.
	virtual int query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results, error_what& e_what) = 0;

	// Execute multi-sql-string,  return 0 if success.
	virtual int query_result(vector<string>& sqls, error_what& e_what) = 0;
};

} }


#endif /* BINGO_DATABASE_DB_VISITOR_H_ */
