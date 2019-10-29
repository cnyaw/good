// Dedicated to the public domain by Christopher Diggins 
// http://www.cdiggins.com 
// This work can be used, modified, or redistributed 
// without restriction, obligation or warrantee.

#ifndef YARD_C_PARSER_TESTS_HPP
#define YARD_C_PARSER_TESTS_HPP

#include "../grammars/yard_c_grammar.hpp"

void RunCParserTests()
{
	using namespace yard_test;

	Test<c_grammar::Exp>("x<=0");
	Test<c_grammar::Exp>("f(x<=0)");
	Test<c_grammar::Exp>("0");
	Test<c_grammar::Exp>("(0)");
	Test<c_grammar::Exp>("(x)");
	Test<c_grammar::UnaryExp>("(x)");
	Test<c_grammar::PostfixExp>("(x)");
	Test<c_grammar::CastExp>("(x)");
	Test<c_grammar::PrimaryExp>("(x)");
	Test<c_grammar::Paranthesized<c_grammar::Exp> >("(x)");
	Test<c_grammar::Exp>("f(x)");
	Test<c_grammar::Exp>("f(a, b)");
	Test<c_grammar::Exp>("a + b");
	Test<c_grammar::Exp>("a + b + c");
	Test<c_grammar::Exp>("a + b<=c");
	Test<c_grammar::Exp>("f(a + b)");
	Test<c_grammar::Exp>("f(g())");
	Test<c_grammar::Exp>("(x<=0)");
	Test<c_grammar::Exp>("a[b]");
	Test<c_grammar::Exp>("(a[b])");
	Test<c_grammar::Exp>("x = a[b]");
	Test<c_grammar::Exp>("f(a[b])");
	Test<c_grammar::Exp>("a == b");
	Test<c_grammar::Exp>("a == b == c");
	Test<c_grammar::Exp>("N = a == b");
	Test<c_grammar::Exp>("a == b ? c : d");
	Test<c_grammar::Exp>("n < 2 ? 1 : fib(n - 2) + fib(n - 1)");
	Test<c_grammar::Exp>("n = n < 2 ? 1 : fib(n - 2) + fib(n - 1)");
	Test<c_grammar::Exp>("N = ((argc == 2) ? atoi(argv[1]) : 1)");
	Test<c_grammar::AssExp>("x = y = z");
	Test<c_grammar::Exp>("a = b ? c : d");
	Test<c_grammar::Exp>("a = b ? c : d");
	Test<c_grammar::AssExp>("N = ((argc == 2) ? atoi(argv[1]) : 1)");
	Test<c_grammar::Statement>("n = x;");
	Test<c_grammar::Statement>("n = a == b;");
	Test<c_grammar::Statement>("n = a == b ? x : y;");
	Test<c_grammar::Statement>("{ x; a; }");
	Test<c_grammar::Statement>("a;");
	Test<c_grammar::Statement>("{ x; { a; } { } y; }");
	Test<c_grammar::FunDef>("void a() { }");
	Test<c_grammar::FunDef>("void a(b c, d e) { }");
	Test<c_grammar::FunDef>("void a(b c, d e) { a; b; }");
	Test<c_grammar::FunDef>("void a(b c, d e) { a; { g; } b; }");
	Test<c_grammar::Declaration>("int N = ((argc == 2) ? atoi(argv[1]) : 1);");	
	Test<c_grammar::Declaration>("int i, k, *x, *y;");
	Test<c_grammar::Exp>("i[1] = j[2]");
	Test<c_grammar::Exp>("x[y] += x[y]");
	Test<c_grammar::Exp>("i = n-1");
	Test<c_grammar::Exp>("++i");
	Test<c_grammar::Exp>("i--");
	Test<c_grammar::Exp>("i >= 0");
	Test<c_grammar::Statement>("for (i=0; i < 10; ++i) { }");
	Test<c_grammar::Statement>("for (i = n-1; i >= 0; i--) { y[i] += x[i]; }");
	Test<c_grammar::Declaration>("char *pp;");
	Test<c_grammar::Declaration>("char pp, pe;");
	Test<c_grammar::Declaration>("unsigned char pp;");
	Test<c_grammar::Declaration>("unsigned char *pp;");
	Test<c_grammar::Declaration>("unsigned char pp, pe;");
	Test<c_grammar::Declaration>("unsigned char *pp, *pe;");
	Test<c_grammar::Exp>("(int **) x");	
	Test<c_grammar::Exp>("malloc(sizeof(int *))");	
	Test<c_grammar::Exp>("malloc(rows * sizeof(int *))");	
	Test<c_grammar::Declaration>("int **m;");
	Test<c_grammar::Declaration>("int **m = x;");
	Test<c_grammar::Declaration>("int **m = (int **) x;");
	Test<c_grammar::Declaration>("int **m = (int **) malloc(rows * sizeof(int *));");
	printf("\n\n");
}

#endif
