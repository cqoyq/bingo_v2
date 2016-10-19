/*
 * mysql_connector.h
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_
#define BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_

#include "bingo/error_what.h"
#include "bingo/database/db_connector.h"
#include "bingo/database/db_visitor.h"

#include <mysql.h>

namespace bingo { namespace database { namespace mysql {

// Connector handle class.
class mysql_connector : public db_visitor {
public:
	mysql_connector(db_connector* conn_info);
	virtual ~mysql_connector();

	// Connect success return 0. otherwise -1, to check error.
	int connect();

	// Return error information.
	error_what& err();

	// Obtain mysql*.
	MYSQL*& get_mysql();

	// Whether connect success, if success that value is true.
	// default is false.
	bool is_connect_success;

	// Query single result, return 0 if success.
	// @sql is single sql-string, @result is query result, @e_what is error.
	int query_result(const char* sql, db_result*& result, error_what& e_what);

	// Query multi-result,  return 0 if success.
	int query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results, error_what& e_what);

	// Execute multi-sql-string,  return 0 if success.
	int query_result(vector<string>& sqls, error_what& e_what);

protected:
	db_connector* conn_info_;
	error_what err_;

	MYSQL* mysql_conn_;


};

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_ */
