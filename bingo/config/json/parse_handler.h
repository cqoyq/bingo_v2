/*
 * parse_handler.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_CONFIG_JSON_PARSE_HANDLER_HEADER_H_
#define BINGO_CONFIG_JSON_PARSE_HANDLER_HEADER_H_

#include <iostream>

#include "bingo/foreach_.h"
#include "bingo/error_what.h"
#include "bingo/config/node.h"
using bingo::config::node;

#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;
using namespace boost;

#include <boost/exception/all.hpp>
#include <boost/current_function.hpp>

namespace bingo { namespace config { namespace json {

enum {
	error_json_read_fail		 		= 0x01,
	error_json_get_value_fail			=0x02,
	error_json_get_node_fail			= 0x03,
};

struct json_parser{
	static bool is_pretty;
};

// Json parse without attribute
template<typename PARSER = json_parser>
class json_parse_handler {
public:
	json_parse_handler(){
		node_ = 0;
	}
	virtual ~json_parse_handler(){
		if(node_ != 0) delete node_;
	}

	int read(const char* file, error_what& e_what){
		try{
			pt::read_json(file, pt_);
			return 0;
		}
		catch(boost::exception& e){
			e_what.err_no(error_json_read_fail);
			e_what.err_message(current_exception_cast<std::exception>()->what());

			return -1;
		}
	}

	int read(stringstream& stream, error_what& e_what){
		try{
			pt::read_json(stream, pt_);
			return 0;
		}
		catch(boost::exception& e){
			e_what.err_no(error_json_read_fail);
			e_what.err_message(current_exception_cast<std::exception>()->what());
			return -1;
		}
	}


	int get_value(const char* path, string& value, error_what& e_what){

		try{
			value = pt_.get<string>(path);
			return 0;
		}
		catch(boost::exception& e){
			e_what.err_no(error_json_get_value_fail);
			e_what.err_message(current_exception_cast<std::exception>()->what());

			return -1;
		}
	}

	node* get_node(const char* path, error_what& e_what){
		if(node_ !=0) {
			delete node_;
			node_ = 0;
		}

		try{
			pt::ptree pt = pt_.get_child(path);

			node_ = new node();
			parse_ptree(node_, pt);

			return node_;
		}
		catch(boost::exception& e){
			e_what.err_no(error_json_get_node_fail);
			e_what.err_message(current_exception_cast<std::exception>()->what());

			return 0;
		}
	}



	void write(stringstream& stream, node* n){

		pt::ptree wtree;
		make_ptree(n, wtree);

		pt::write_json(stream, wtree, PARSER::is_pretty);
	}

	void write(const char* file, node* n){

		pt::ptree wtree;
		make_ptree(n, wtree);

		pt::write_json(file, wtree, std::locale(), PARSER::is_pretty);
	}
private:
	void parse_ptree(node* n, pt::ptree& tree){
		n->value.append(tree.get_value<string>().c_str());

		foreach_(pt::ptree::value_type &v1, tree){
			 if(v1.first == "<xmlattr>"){

			 }else{
				 // First node do it.
				 string name(v1.first);
				 node* child = new node(name.c_str());

				 parse_ptree(child, v1.second);

				 n->child.push_back(child);
			 }

		}
	}

	void make_ptree(node* n, pt::ptree& tree){

		pt::ptree& addnode = tree.add(n->name, n->value);

		if(n->child.size() > 0){
			// Have child, then node value is ''.
			addnode.put_value("");

			foreach_(node& m, n->child.collection()){
				make_ptree(&m, addnode);
			}
		}
	}
private:
	pt::ptree pt_;
	node* node_;
};

} } }


#endif /* BINGO_CONFIG_JSON_PARSE_HANDLER_HEADER_H_ */
