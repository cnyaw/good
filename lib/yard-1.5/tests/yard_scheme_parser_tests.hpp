// Dedicated to the public domain by Christopher Diggins 
// http://www.cdiggins.com 
// This work can be used, modified, or redistributed 
// without restriction, obligation or warrantee.

#ifndef YARD_SCHEME_PARSER_TESTS_HPP
#define YARD_SCHEME_PARSER_TESTS_HPP

#include "../grammars/yard_scheme_grammar.hpp"

void RunSchemeParserTests()
{
	using namespace yard_test;

	Test<scheme_grammar::Num<2> >("#b01");
	Test<scheme_grammar::Num<8> >("#o12");
	Test<scheme_grammar::Num<10> >("#d12");
	Test<scheme_grammar::Num<16> >("#x12");
	Test<scheme_grammar::Number>("#b#i01");
	Test<scheme_grammar::Exactness>("#i");
	Test<scheme_grammar::Exactness>("#e");
	Test<scheme_grammar::Prefix<10> >("#d");
	Test<scheme_grammar::Prefix<10> >("#i");
	Test<scheme_grammar::Prefix<10> >("#e");
	Test<scheme_grammar::Radix<10> >("#d");
	Test<scheme_grammar::Prefix<10> >("#i#d");
	Test<scheme_grammar::Prefix<10> >("#d#i");
	Test<scheme_grammar::Number>("#o#e12");
	Test<scheme_grammar::Number>("#i#d12");
	Test<scheme_grammar::Number>("#d#i12");
	Test<scheme_grammar::Number>("#e#x12");
	Test<scheme_grammar::Number>("#i12");
	Test<scheme_grammar::Number>("#e12");
	Test<scheme_grammar::Number>("12");
	Test<scheme_grammar::Identifier>("+");
	Test<scheme_grammar::Identifier>("-");
	Test<scheme_grammar::Identifier>("...");
	Test<scheme_grammar::Identifier>("qwerty");
	Test<scheme_grammar::Identifier>("A");
	Test<scheme_grammar::Identifier>("b12");
	Test<scheme_grammar::Identifier>("a-b");
	Test<scheme_grammar::Identifier>("ab?");
	Test<scheme_grammar::Identifier>("a?-b!");
	Test<scheme_grammar::Boolean>("#t");
	Test<scheme_grammar::String>("\"qwerty\"");
	Test<scheme_grammar::List>("()");
	Test<scheme_grammar::List>("(a)");
	Test<scheme_grammar::List>("(12)");
	Test<scheme_grammar::List>("(a 13 #t)");
	Test<scheme_grammar::List>("(())");
	Test<scheme_grammar::List>("((()))");
	Test<scheme_grammar::List>("(()())");
	Test<scheme_grammar::List>("(()a())");
	Test<scheme_grammar::List>("((a)(b)(c))");
	Test<scheme_grammar::Vector>("#(1 2 3)");
	Test<scheme_grammar::Abbreviation>(",1");
	Test<scheme_grammar::Abbreviation>("'1");
	Test<scheme_grammar::Abbreviation>(",1");
	Test<scheme_grammar::Abbreviation>(",@1");
	Test<scheme_grammar::Abbreviation>(",(1 2 3)");
	Test<scheme_grammar::Abbreviation>("'(1 2 3)");
	Test<scheme_grammar::Abbreviation>(",(1 2 3)");
	Test<scheme_grammar::Abbreviation>(",@(1 2 3)");
}

#endif
