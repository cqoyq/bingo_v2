/*
 * error_code.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_ERROR_CODE_H_
#define BINGO_DATABASE_ERROR_CODE_H_

namespace bingo { namespace database {

enum {
	error_database_connect_fail			= 0x01,
	error_database_query_fail				= 0x02,
};

#define error_database_connect_fail_message "database connect fail!"
#define error_database_query_fail_message "database query fail!"

} }


#endif /* BINGO_DATABASE_ERROR_CODE_H_ */
