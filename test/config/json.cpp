/*
 * json.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/config/all.h"
using namespace bingo::config;
using namespace bingo::config::json;

BOOST_AUTO_TEST_SUITE(test_config_json)

node g_node;

// 测试夹具，每个测试用例都会调用
struct s_node_fixture
{
	//测试用例执行前调用
	s_node_fixture()
	{
		g_node.name = "application";

		node* item1 = new node("gateway");
		item1->attrs.push_back(new node_attr("id","1"));
		item1->child.push_back(new node("has_deploy","0"));
		{
			node* ips = item1->child.push_back(new node("ips"));
			ips->child.push_back(new node("ip"))->attrs.push_back(new node_attr("value", "110.1.0.0/16"));
			ips->child.push_back(new node("ip"))->attrs.push_back(new node_attr("value", "110.2.0.0/16"));
		}
		g_node.child.push_back(item1);


		node* item2 = new node("phone");
		item2->child.push_back(new node("has_deploy", "1"));
		{
			node* ips = item2->child.push_back(new node("ips"));
			ips->child.push_back(new node("ip"))->attrs.push_back(new node_attr("value", "220.1.0.0/16"));
			ips->child.push_back(new node("ip"))->attrs.push_back(new node_attr("value", "220.2.0.0/16"));
		}
		g_node.child.push_back(item2);

		node* item3 = new node("name", "this is a test");
		g_node.child.push_back(item3);

		node* item4 = new node("items");
		item4->child.push_back(new node("item", "one"));
		item4->child.push_back(new node("item", "two"));
		item4->child.push_back(new node("item", "three"));
		g_node.child.push_back(item4);
	}

	//测试用例测试后调用
	~s_node_fixture()
	{


	}
};

BOOST_FIXTURE_TEST_SUITE(t,s_node_fixture)

BOOST_AUTO_TEST_CASE(t){

	// 写测试
	stringstream stream;
	g_node.write<json_parse_handler<> >(stream);		// write to stream

	cout << stream.str() << endl;
	// output:
	//	{
	//	    "application": {
	//	        "gateway": {
	//	            "has_deploy": "0",
	//	            "ips": {
	//	                "ip": "",
	//	                "ip": ""
	//	            }
	//	        },
	//	        "phone": {
	//	            "has_deploy": "1",
	//	            "ips": {
	//	                "ip": "",
	//	                "ip": ""
	//	            }
	//	        },
	//	        "name": "this is a test",
	//	        "items": {
	//	            "item": "one",
	//	            "item": "two",
	//	            "item": "three"
	//	        }
	//	    }
	//	}


	g_node.write<json_parse_handler<> >("./jsonw.xml");		// write to file

	// 读测试
	string err;
	json_parse_handler<> json;
	BOOST_CHECK_EQUAL(json.read("./jsonw.xml", err), 0);

	string value;
	json.get_value("application.gateway", value, err);
	BOOST_CHECK_EQUAL(value.c_str(), "");

	json.get_value("application.gateway.has_deploy", value, err);
	BOOST_CHECK_EQUAL(value.c_str(), "0");

	BOOST_CHECK(json.get_value("application.gateways", value, err) == -1);							// read error
	cout << "read value error:" << err << endl;
	// output:
	//	read value error:No such node (application.gateways)

	BOOST_CHECK(json.get_node("application.gateway.qa", err) == 0);					// read error
	cout << "read node error:" << err << endl;
	// output:
	//	read node error:No such node (application.gateway.qa)

	node* n = json.get_node("application.items", err);
	BOOST_CHECK(n->child[0]->value.compare("one") == 0);
	BOOST_CHECK(n->child[1]->value.compare("two") == 0);
	BOOST_CHECK(n->child[2]->value.compare("three") == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
