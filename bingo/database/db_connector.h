/*
 * db_connector.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_CONNECTOR_H_
#define BINGO_DATABASE_DB_CONNECTOR_H_

#include <string>
using namespace std;

#include "bingo/type.h"

namespace bingo { namespace database {

class db_connector {
public:
	db_connector();
	virtual ~db_connector();

	const char* ip();
	void ip(const char* s);

	const char* user();
	void user(const char* s);

	const char* pwd();
	void pwd(const char* s);

	const char* dbname();
	void dbname(const char* s);

	u32_t port();
	void port(u32_t v);

protected:
	string ip_;
	string user_;
	string pwd_;
	string dbname_;
	u32_t port_;
};

} }

#endif /* BINGO_DATABASE_DB_CONNECTOR_H_ */
