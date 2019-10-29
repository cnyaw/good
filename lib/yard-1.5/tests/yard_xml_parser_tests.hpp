// Dedicated to the public domain by Christopher Diggins 
// http://www.cdiggins.com 
// This work can be used, modified, or redistributed 
// without restriction, obligation or warrantee.

#ifndef YARD_XML_PARSER_TESTS_HPP
#define YARD_XML_PARSER_TESTS_HPP

#include "../grammars/yard_xml_grammar.hpp"

void RunXMLParserTests()
{
	using namespace yard_test;

	Test<xml_grammar::Document>("<?xml version='1.0' ?><x><a></a><b></b></x>");
	Test<xml_grammar::Content>("<x><a></a><b></b></x>");
	Test<xml_grammar::Content>("<x><a>xx</a><b>xxx</b></x>");
	Test<xml_grammar::Prolog>("<?xml version='1.0' ?>");
	Test<xml_grammar::STag>("<hello>");
	Test<xml_grammar::ETag>("</hello>");
	Test<xml_grammar::EmptyElemTag>("<hello/>");
	Test<xml_grammar::STag>("<hello x='y'>");
	Test<xml_grammar::ETag>("</hello>");
	Test<xml_grammar::EmptyElemTag>("<hello x='y'/>");
	Test<xml_grammar::STag>("<hello x=\"y\">");
	Test<xml_grammar::EmptyElemTag>("<hello x=\"y\"/>");
	Test<xml_grammar::EntityRef>("&xxx;");
	Test<xml_grammar::PEReference>("%xxx;");
	Test<xml_grammar::Comment>("<!-- bla -->");
	Test<xml_grammar::Element>("<x>bla</x>");
	Test<xml_grammar::CDSect>("<![CDATA[barf]]>");
	Test<xml_grammar::Element>("<x:y/>");
	Test<xml_grammar::Document>(
		"<?xml version='1.0' ?>"\
		"<note>"\
		"<to>Tove</to>"\
		"<from>Jani</from>"\
		"<heading>Reminder</heading>"\
		"<body>Don't forget me this weekend!</body>"\
		"</note>");
	Test<xml_grammar::Element>("<link>http://cdiggins.com/2007/09/15/cat-in-scheme/</link>");
	Test<xml_grammar::Element>("<comments>http://cdiggins.com/2007/09/15/cat-in-scheme/#comments</comments>");
	Test<xml_grammar::Element>("<pubDate>Sat, 15 Sep 2007 14:37:39 +0000</pubDate>");
	Test<xml_grammar::Element>("<dc:creator>cdiggins</dc:creator>");
	Test<xml_grammar::Element>("<category><![CDATA[Cat]]></category>");
	Test<xml_grammar::Element>("<guid isPermaLink=\"false\">http://cdiggins.com/2007/09/15/cat-in-scheme/</guid>");
	Test<xml_grammar::Element>("<a> \t </a>");
	Test<xml_grammar::Element>("<a> \n </a>");
	Test<xml_grammar::Element>("<a> <b> </b> <c> </c> </a>");
	Test<xml_grammar::Element>("<channel> \t\n <title>cdiggins.com</title> \t\n <link>http://cdiggins.com</link> \t\n </channel>");
}

#endif
