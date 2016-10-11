/*
 * node.cpp
 *
 *  Created on: 2016-10-11
 *      Author: root
 */

#include "node.h"

using namespace bingo::config;

// ------------------------------  node_attr_set ----------------------------------- //

node_attr_set& node_attr_set::push_back(node_attr* n){
	set.push_back(n);
	return (*this);
}

node_attr* node_attr_set::operator[](int index){
	int max = set.size() - 1;
	if(index <= max)
		return &(set[index]);
	else
		return 0;
}

node_attr* node_attr_set::operator[](const char* attr_name){
	node_attr* p = 0;
	int max = set.size();
	for (int i = 0; i < max; i++) {
		if(set[i].name.compare(attr_name) == 0){
			p = &(set[i]);
			break;
		}
	}
	return p;
}

size_t node_attr_set::size(){
	return set.size();
}

boost::ptr_vector<node_attr>& node_attr_set::collection(){
	return set;
}

// ------------------------------  node_set ----------------------------------- //

node* node_set::push_back(node* n){
	set.push_back(n);
	return n;
}

node* node_set::operator[](int index){
	int max = set.size() - 1;
	if(index <= max)
		return &(set[index]);
	else
		return 0;
}

node* node_set::operator[](const char* name){
	foreach_(node&n, set){
		if(n.name.compare(name) == 0)
			return &n;
	}
	return 0;
}

size_t node_set::size(){
	return set.size();
}

boost::ptr_vector<node>& node_set::collection(){
	return set;
}
