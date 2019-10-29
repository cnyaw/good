#include "..\include\yard.hpp"

#include "..\tests\yard_all_parser_tests.hpp"

int main()
{
	RunXMLParserTests();
	RunSchemeParserTests();
	RunCParserTests();
	system("PAUSE");
	return 0;
}