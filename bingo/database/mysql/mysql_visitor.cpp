/*
 * mysqlvisitor.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include <mysql.h>
#include "bingo/database/db_result.h"

#include "mysql_visitor.h"

using namespace bingo::database::mysql;

// Call free_database() when progress is destroyed.
void bingo::database::mysql::free_mysql_database(){
	mysql_library_end();
}

mysql_visitor::mysql_visitor(db_connector*& conn) {
	// TODO Auto-generated constructor stub
	conn_ = conn;
}

mysql_visitor::~mysql_visitor() {
	// TODO Auto-generated destructor stub
}

int mysql_visitor::query_result(const char* sql, db_result*& result, error_what& e_what){
	// Sql string valid.
	MYSQL *conn = mysql_init(NULL);

	my_bool reconnect = 0;
	mysql_options(conn, MYSQL_SECURE_AUTH, &reconnect);

	// Connect mysql's server.
	if (!mysql_real_connect(conn, conn_->ip(), conn_->user(), conn_->pwd(), conn_->dbname(), conn_->port(), NULL, 0)) {

		e_what.err_no(error_database_connect_fail);
		e_what.err_message(error_database_connect_fail_message);

		// free Mysql.
		mysql_close(conn);

		mysql_thread_end();

		return -1;
	}

	mysql_set_character_set(conn, "utf8");

	// Execute sql statement.
	mysql_query(conn, sql);
	const char* er = mysql_error(conn);
	if((*er) != 0x00){

		string err_info = error_database_query_fail_message;
		err_info.append(" error:");
		err_info.append(er);
		e_what.err_no(error_database_query_fail);
		e_what.err_message(err_info.c_str());

		mysql_close(conn);

		mysql_thread_end();

		return -1;
	}


	// Starting to handle return of result.
	MYSQL_RES *res;
	MYSQL_ROW row;

	// Get MYSQL_RES.
	res = mysql_store_result(conn);

	// Get row number and column number in MYSQL_RES.
	result->row_amount 	= mysql_num_rows(res);
	result->column_amount = mysql_num_fields(res);


	// Get column data in MYSQL_RES.
	MYSQL_FIELD *columns;
	columns = mysql_fetch_fields(res);
	for(int i = 0; i < result->column_amount; i++)
	{
//	   printf("Field %u is %s\n", i, fields[i].name);
//		result->fields_[i] = columns[i].name;
		result->add_header(new db_field_header(columns[i].name));
	}

	// Get row data in MYSQL_RES.
	while((row = mysql_fetch_row(res))!= NULL) {
//		result->result[index] = row;

		u64_t *FieldLength = mysql_fetch_lengths(res);
//		for(int i = 0;i<column_num;i++ )
//		{
//			int nDataLen = (int)FieldLength[i];
//			printf("index=%d column index:%d ,length:%d\n", index, i, nDataLen);
//		}

		db_row* d_row = new db_row(result);

		// Save field data.
		for(int i = 0; i<result->column_amount; i++ ){
			// Get field length.
			u64_t field_size  = (u64_t)FieldLength[i];

			// Get field value.
			char* field_value = row[i];

			d_row->add_field(new db_field(field_value, field_size));
		}

		// Save row data.
		result->add_row(d_row);
	}

	mysql_free_result(res);


	mysql_close(conn);

	mysql_thread_end();

	return 0;
}

int mysql_visitor::query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results, error_what& e_what){
	// Sql string valid.
	MYSQL *conn = mysql_init(NULL);

	my_bool reconnect = 0;
	mysql_options(conn, MYSQL_SECURE_AUTH, &reconnect);

	// Connect mysql's server.
	if (!mysql_real_connect(conn, conn_->ip(), conn_->user(), conn_->pwd(), conn_->dbname(), conn_->port(), NULL, 0)) {

		e_what.err_no(error_database_connect_fail);
		e_what.err_message(error_database_connect_fail_message);

		// free Mysql.
		mysql_close(conn);

		mysql_thread_end();

		return -1;
	}

	mysql_set_character_set(conn, "utf8");

	for (int n = 0;  n< sqls.size(); n++){

		if(strcmp(sqls[n].c_str(),"") == 0) continue;

		// Execute sql statement.
		mysql_query(conn, sqls[n].c_str());
		const char* er = mysql_error(conn);
		if((*er) != 0x00){

			string err_info = error_database_query_fail_message;
			err_info.append(" error:");
			err_info.append(er);

			e_what.err_no(error_database_query_fail);
			e_what.err_message(err_info.c_str());

			mysql_close(conn);

			mysql_thread_end();

			return -1;
		}


		// Starting to handle return of result.
		MYSQL_RES *res;
		MYSQL_ROW row;

		db_result* result = new db_result();

		// Add result to vector.
		results.push_back(result);


		// Get MYSQL_RES.
		res = mysql_store_result(conn);

		if(res){

			// Get row number and column number in MYSQL_RES.
			result->row_amount 	= mysql_num_rows(res);
			result->column_amount= mysql_num_fields(res);


			// Get column data in MYSQL_RES.
			MYSQL_FIELD *columns;
			columns = mysql_fetch_fields(res);
			for(int i = 0; i < result->column_amount; i++)
			{
		//	   printf("Field %u is %s\n", i, fields[i].name);
		//		result->fields_[i] = columns[i].name;
				result->add_header(new db_field_header(columns[i].name));
			}

			// Get row data in MYSQL_RES.
			while((row = mysql_fetch_row(res))!= NULL) {

				u64_t *FieldLength = mysql_fetch_lengths(res);

				db_row* d_row = new db_row(result);

				// Save field data.
				for(int i = 0; i<result->column_amount; i++ ){
					// Get field length.
					u64_t field_size  = (u64_t)FieldLength[i];

					// Get field value.
					char* field_value = row[i];

					d_row->add_field(new db_field(field_value, field_size));
				}

				// Save row data.
				result->add_row(d_row);
			}

		}

		mysql_free_result(res);

	}

	mysql_close(conn);

	mysql_thread_end();

	return 0;
}

int mysql_visitor::query_result(vector<string>& sqls, error_what& e_what){
	// Sql string valid.
	MYSQL *conn = mysql_init(NULL);

	my_bool reconnect = 0;
	mysql_options(conn, MYSQL_SECURE_AUTH, &reconnect);

	if (!mysql_real_connect(conn, conn_->ip(), conn_->user(), conn_->pwd(), conn_->dbname(), conn_->port(), NULL, 0)) {

		e_what.err_no(error_database_connect_fail);
		e_what.err_message(error_database_connect_fail_message);

		// free Mysql.
		mysql_close(conn);

		mysql_thread_end();

		return -1;
	}

	mysql_set_character_set(conn, "utf8");

	// Start trasaction.
	mysql_query(conn, "start transaction;");

	for(int i=0;i< sqls.size(); i++){

		if(sqls[i] != ""){

			mysql_query(conn, sqls[i].c_str());
			const char* er = mysql_error(conn);
			if((*er) != 0x00){

				string err_info = error_database_query_fail_message;
				err_info.append(" error:");
				err_info.append(er);

				e_what.err_no(error_database_query_fail);
				e_what.err_message(err_info.c_str());

				// Roll back transaction.
				mysql_query(conn, "rollback;");

				mysql_close(conn);

				mysql_thread_end();

				return -1;

			}
		}
	}

	// Commit transaction.
	mysql_query(conn, "commit;");

	mysql_close(conn);

	mysql_thread_end();

	return 0;
}

