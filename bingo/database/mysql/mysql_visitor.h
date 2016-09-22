/*
 * mysql_visitor.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_VISITOR_H_
#define BINGO_DATABASE_MYSQL_MYSQL_VISITOR_H_

#include "../db_visitor.h"
#include "../db_connector.h"

namespace bingo { namespace database { namespace mysql {

// Call free_database() when progress is destroyed.
void free_mysql_database();

class mysql_visitor : public db_visitor {
public:
	mysql_visitor(db_connector*& conn);
	virtual ~mysql_visitor();

	int query_result(const char* sql, db_result*& result, error_what& e_what);

	int query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results, error_what& e_what);

	int query_result(vector<string>& sqls, error_what& e_what);

private:
	db_connector* conn_;
};

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_VISITOR_H_ */
