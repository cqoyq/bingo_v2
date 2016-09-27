/*
 * node.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_CONFIG__NODE_H_
#define BINGO_CONFIG__NODE_H_

#include <exception>
#include <string>
using namespace std;

#include <boost/ptr_container/ptr_vector.hpp>

namespace bingo { namespace config {

// Exception that attribute isn't exist.
struct attr_not_exist_exp : public std::exception {
	attr_not_exist_exp() : std::exception(){};

	const char* what()const throw()
   {
		return "attribute isn't exit!";
   }
};

// Node attribute class
struct node_attr{
	string name;
	string value;
	node_attr(const char* name, const char* value):
		name(name), value(value){}
};

// Node attribute set class
class node_attr_set{
public:
	node_attr_set& push_back(node_attr* n){
		set.push_back(n);
		return (*this);
	}

	node_attr* operator[](int index){
		int max = set.size() - 1;
		if(index <= max)
			return &(set[index]);
		else
			return 0;
	}

	node_attr* operator[](const char* attr_name){
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

	size_t size(){
		return set.size();
	}

	boost::ptr_vector<node_attr>& collection(){
		return set;
	}

private:
	boost::ptr_vector<node_attr> set;
};

class node;

// Node set class.
class node_set{
public:
	// Return add node.
	node* push_back(node* n){
		set.push_back(n);
		return n;
	}

	node* operator[](int index){
		int max = set.size() - 1;
		if(index <= max)
			return &(set[index]);
		else
			return 0;
	}

	size_t size(){
		return set.size();
	}

	boost::ptr_vector<node>& collection(){
		return set;
	}

private:
	boost::ptr_vector<node> set;
};

// Node information class.
struct node{
	string 							name;
	string 							value;
	node_attr_set				 	attrs;
	node_set				 		child;

	node(){}
	node(const char* name, const char* value = ""):
		name(name), value(value){};

	template<typename HANDLER>
	void write(stringstream& stream){
		HANDLER hdr;
		hdr.write(stream, this);
	}

	template<typename HANDLER>
	void write(const char* file){
		HANDLER hdr;
		hdr.write(file, this);
	}

private:

};

}  }


#endif /* BINGO_CONFIG__NODE_H_ */
