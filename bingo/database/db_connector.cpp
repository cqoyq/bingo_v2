/*
 * dbconnector.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include "db_connector.h"

using namespace bingo;
using namespace bingo::database;

db_connector::db_connector() {
	// TODO Auto-generated constructor stub
	port_ = 0;
}

db_connector::~db_connector() {
	// TODO Auto-generated destructor stub
}

const char* db_connector::ip(){
	return ip_.c_str();
}

void db_connector::ip(const char* s){
	ip_ = s;
}

const char* db_connector::user(){
	return user_.c_str();
}

void db_connector::user(const char* s){
	user_ = s;
}

const char* db_connector::pwd(){
	return pwd_.c_str();
}

void db_connector::pwd(const char* s){
	pwd_ = s;
}

const char* db_connector::dbname(){
	return dbname_.c_str();
}

void db_connector::dbname(const char* s){
	dbname_ = s;
}

u32_t db_connector::port(){
	return port_;
}

void db_connector::port(u32_t v){
	port_ = v;
}

