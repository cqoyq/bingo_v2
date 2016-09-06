/*
 * xml.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/config/all.h"
using namespace bingo::config;
using namespace bingo::config::xml;

BOOST_AUTO_TEST_SUITE(test_config_xml)

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
	// 测试写
	stringstream stream;
	g_node.write<xml_parse_handler<> >(stream);		// write to stream

	cout << stream.str() << endl;
	// output:
	//	<?xml version="1.0" encoding="utf-8"?>
	//	<application>
	//		<gateway id="1">
	//			<has_deploy>0</has_deploy>
	//			<ips>
	//				<ip value="110.1.0.0/16"/>
	//				<ip value="110.2.0.0/16"/>
	//			</ips>
	//		</gateway>
	//		<phone>
	//			<has_deploy>1</has_deploy>
	//			<ips>
	//				<ip value="220.1.0.0/16"/>
	//				<ip value="220.2.0.0/16"/>
	//			</ips>
	//		</phone>
	//		<name>this is a test</name>
	//		<items>
	//			<item>one</item>
	//			<item>two</item>
	//			<item>three</item>
	//		</items>
	//	</application>

	g_node.write<xml_parse_handler<> >("./confw.xml");		// write to file

	// 测试读
	string err;
	xml_parse_handler<> xml;
	BOOST_CHECK_EQUAL(xml.read("./confw.xml", err), 0);

	string value;
	BOOST_CHECK(xml.get_value("application.ab", value, err) == -1);							// read error
	cout << "read value error:" << err << endl;
	// output:
	//	read value error:No such node (application.ab)

	xml.get_value("application.name", value, err);
	BOOST_CHECK_EQUAL(value.c_str(), "this is a test");

	BOOST_CHECK(xml.get_attr("application.gateways", "t", value, err) == -1);				// read error
	cout << "read attr of node error:" << err << endl;
	// output:
	//	read attr of node error:No such node (application.gateways)

	BOOST_CHECK(xml.get_attr("application.gateway", "t", value, err) == -1);				// read error
	cout << "read attr error:" << err << endl;
	// output:
	//  read attr error:attribute isn't exit!

	xml.get_attr("application.gateway", "id", value, err);
	BOOST_CHECK_EQUAL(value.c_str(), "1");

	BOOST_CHECK(xml.get_node("application.gateway.qa", err) == 0);					// read error
	cout << "read node error:" << err << endl;
	// output:
	//	read node error:No such node (application.gateway.qa)

	node* n = xml.get_node("application.gateway.ips", err);
	BOOST_CHECK(n->child[0]->attrs[0]->name.compare("value") == 0);
	BOOST_CHECK(n->child[0]->attrs[0]->value.compare("110.1.0.0/16") == 0);

	BOOST_CHECK(n->child[1]->attrs[0]->name.compare("value") == 0);
	BOOST_CHECK(n->child[1]->attrs[0]->value.compare("110.2.0.0/16") == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()


