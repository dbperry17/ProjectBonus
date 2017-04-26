#include <iostream>
#include "parser.h"

using namespace std;

bool testing = false;
bool testParse = true;
void tester()
{
    if(testing && !testParse)
    {
        cout << "\n\nSTARTING PROGRAM" << endl;
    }
    else
    {
        testParse = false;
    }
}

/**********
 *  PGIR  *
 **********/

struct StatementNode * parse_generate_intermediate_representation()
{
    tester();
    Parser parser;
    StatementNode* program = parser.ParseInput();
    //parser.print();

    /**
     * Return symbol table???
     */
    return program;
}