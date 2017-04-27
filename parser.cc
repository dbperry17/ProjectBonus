/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <algorithm>
#include "parser.h"

using namespace std;
bool errorFind = false;
bool testIf = false;
bool testSwitch = false;
int testNum = 1;
//cout << "test " << testNum++ << endl;

vector<ValueNode*> symTable;
struct Parser::ExprNode
{
    ValueNode* op1;
    ValueNode* op2;
    ArithmeticOperatorType arith;
};

struct Parser::CondNode
{
    ValueNode* op1;
    ValueNode* op2;
    ConditionalOperatorType condType;
};

struct Parser::CaseNode
{
    ValueNode* n;
    StatementNode* body;
};

/********************************************************************
 *                                                                  *
 * NOTE: I am aware we "can assume that there are no syntax or      *
 *       semantic errors in the input program." However, I found    *
 *       checking for syntax errors to be useful while altering     *
 *       the parsing functions so that I could make sure I didn't   *
 *       mess anything up, so I have chosen to keep that bit of     *
 *       code.                                                      *
 *                                                                  *
 ********************************************************************/

/**********************
 *  HELPER FUNCTIONS  *
 **********************/

void Parser::syntax_error()
{
    cout << "Syntax Error\n";
    exit(1);
}

Token Parser::expect(TokenType expected_type)
{
    if(errorFind)
        cout << "Expecting Token: " << expected_type << endl;

    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();

    if(errorFind)
        cout << "Found Token" << endl;

    return t;
}

Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

//Print types and variables
//for use of testing parsing only
void Parser::print()
{
    for(int i = 0; i < (int)symTable.size(); i++)
    {
        cout << symTable[i]->name << " ";

    }
    cout << "#" << endl;
}

//Check to see if item is in symbol table
ValueNode* Parser::symLookup(string name)
{
    if(errorFind)
        cout << "Starting " << "symLookup" << endl;
    ValueNode* tempNode = new ValueNode;
    int iter = 0;
    bool found = false;
    for(iter = 0; iter < (int)symTable.size(); iter++)
    {
        //Remember, string comparison returns 0 if strings are equal
        if(name.compare((symTable[iter])->name) == 0)
        {
            tempNode = symTable[iter];
            found = true;
        }
    }

    //Symbol not in table, so add it
    if(!found)
        tempNode = addValNode(name);

    if(errorFind)
        cout << "Finished " << "symLookup" << endl;

    return tempNode;
}

//Adds new ValueNode to symbol table
ValueNode* Parser::addValNode(string name)
{
    if(errorFind)
        cout << "Starting " << "addValNode" << endl;

    ValueNode* temp = new ValueNode;
    temp->name = "constant";
    symTable.push_back(temp);
    temp = symLookup(name); //yay recursion

    if(errorFind)
        cout << "Finished " << "addValNode" << endl;

    return temp;
}

//Turns a constant into a value node
ValueNode* Parser::constNode(int val)
{
    if(errorFind)
        cout << "Starting " << "constNode" << endl;

    ValueNode* temp = new ValueNode;
    temp->name = "";
    temp->value = val;

    if(errorFind)
        cout << "Finished " << "constNode" << endl;

    return temp;
}

//For testing purposes
void Parser::printStatementList(StatementNode* head)
{
    cout << "\nStatement List: " << endl;

    StatementNode* current = head;
    while (current != NULL)
    {
        if (current->type == ASSIGN_STMT)
        {
            cout << "Assign Statement -> ";
        }
        else if (current->type == IF_STMT)
        {
            cout << "If Statement -> " << endl;
            if(testSwitch)
            {
                cout << "\tFalse path: ";
                printStatementList(head->if_stmt->false_branch);
            }
            else
            {
                cout << "\tTrue path: ";
                printStatementList(head->if_stmt->true_branch);
            }
            cout << "\n" << endl;
        }
        else if (current->type == NOOP_STMT)
        {
            cout << "No-Op Statement -> ";
        }
        else if (current->type == PRINT_STMT)
        {
            cout << "Print Statement -> ";
        }
        else if (current->type == GOTO_STMT)
        {
            cout << "Go-To Statement -> ";
        }

        current = current->next;
    }


    cout << "NULL" << endl;
}


/*************
 *  PARSING  *
 *************/

// Parsing
/*
 * program -> var_section body
 * var_section -> VAR int_var_decl array_var_decl
 * int_var_decl -> id_list SEMICOLON
 * array_var_decl -> id_list COLON ARRAY LBRAC NUM RBRAC SEMICOLON
 * id_list -> ID COMMA id_list | ID
 * body -> LBRACE stmt_list RBRACE
 * stmt_list -> stmt stmt_list | stmt
 * stmt -> assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt
 * assign_stmt -> var_access EQUAL expr SEMICOLON
 * var_access -> ID | ID LBRAC expr RBRAC
 * expr -> term PLUS expr
 * expr -> term
 * term -> factor MULT term
 * term -> factor
 * factor -> LPAREN expr RPAREN
 * factor -> NUM
 * factor -> var_access
 * print_stmt -> print var_access SEMICOLON
 * while_stmt -> WHILE condition body
 * if_stmt -> IF condition body
 * condition -> expr relop expr
 * relop -> GREATER | LESS | NOTEQUAL
 * switch_stmt -> SWITCH var_access LBRACE case_list RBRACE
 * switch_stmt -> SWITCH var_access LBRACE case_list default_case RBRACE
 * case_list -> case case_list | case
 * case -> CASE NUM COLON body
 * default_case -> DEFAULT COLON body
 */

//program -> var_section body
StatementNode* Parser::parse_program()
{
    if(errorFind)
        cout << "Starting " << "parse_program" << endl;

    //program -> var_section body
    parse_var_section();
    StatementNode* node = parse_body();

    if(testIf)
    {
        printStatementList(node);
    }

    if(errorFind)
        cout << "Finished " << "parse_program" << endl;

    return node;
}

//OLD:  var_section -> id_list SEMICOLON
//var_section -> VAR int_var_decl array_var_decl
//TODO: Alter this
//TODO: May need to do something after parsing
void Parser::parse_var_section()
{
    if(errorFind)
        cout << "Starting " << "parse_var_section" << endl;

    expect(VAR);
    parse_int_var_decl();
    parse_array_var_decl();
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_var_section" << endl;
}

//int_var_decl -> id_list SEMICOLON
//TODO: Implement this (Might need to return something)
void Parser::parse_int_var_decl()
{
    parse_id_list(false, 0);
    expect(SEMICOLON);
}

//array_var_decl -> id_list COLON ARRAY LBRAC NUM RBRAC SEMICOLON
//TODO: Implement this (Might need to return something)
//TODO: Figure out what to do with the num (expecting for now for parsing purposes)
void Parser::parse_array_var_decl()
{
    //idea: get token of start of idlist, parse idList,
    //      expect until num, store num, unget token to
    //      start of idList, then parse idlist again
    //      with proper size
    //      problem: don't want to store things with first call
    //      solution:   if arr, don't store unless size != 0
    parse_id_list(true, 0);
    expect(COLON);
    expect(ARRAY);
    expect(LBRAC);
    expect(NUM);
    expect(RBRAC);
    expect(SEMICOLON);
}

//id_list -> ID COMMA id_list | ID
//parameter: 1 if array input, 0 if int input
//TODO: Deal with ints vs arrays
void Parser::parse_id_list(bool arr, int size)
{
    if(errorFind)
        cout << "Starting " << "parse_id_list" << endl;

    // id_list -> ID
    // id_list -> ID COMMA id_list
    ValueNode* tmpSym = new ValueNode;
    Token t = expect(ID);
    tmpSym->name = t.lexeme;
    if(arr)
    {

    }
    else
        symTable.push_back(tmpSym);

    t = lexer.GetToken();
    if (t.token_type == COMMA)
    {
        // id_list -> ID COMMA id_list
        //General case
        parse_id_list(arr, 0);
    }
    else if (t.token_type == SEMICOLON)
    {
        // id_list -> ID
        lexer.UngetToken(t);
    }
    else
        syntax_error();

    if(errorFind)
        cout << "Finished " << "parse_id_list" << endl;
}

//body -> LBRACE stmt_list RBRACE
StatementNode* Parser::parse_body()
{
    if(errorFind)
        cout << "\n\nStarting " << "parse_body" << endl;

    // body -> LBRACE stmt_list RBRACE
    expect(LBRACE);
    StatementNode* node = parse_stmt_list();
    expect(RBRACE);

    if(errorFind)
        cout << "Finished " << "parse_body" << endl;

    return node;
}

//stmt_list -> stmt stmt_list | stmt
StatementNode* Parser::parse_stmt_list()
{
    if(errorFind)
        cout << "Starting " << "parse_stmt_list" << endl;

    // stmt_list -> stmt
    // stmt_list -> stmt stmt_list
    bool nintendo = false; //because I think of them every time I type Switch
    Token t = peek();
    if(t.token_type == SWITCH)
        nintendo = true;

    StatementNode* stmt = parse_stmt();
    StatementNode* stmtList;


    t = peek();
    if ((t.token_type == WHILE) || (t.token_type == ID) ||
        (t.token_type == SWITCH) || (t.token_type == PRINT) ||
        (t.token_type == FOR) || (t.token_type == IF))
    {
        // stmt_list -> stmt stmt_list
        stmtList = parse_stmt_list();

        if(nintendo)
        {
            StatementNode* current = stmt;
            //Alter all cases to fix missing info

            while(current->next != NULL)
            {
                current = current->next;
            }
            current->next = stmtList;
        }
        else if(stmt->type == IF_STMT)
        {
            stmt->next = stmt->if_stmt->false_branch;
            stmt->if_stmt->false_branch->next = stmtList;
        }
        else if(stmt->next != NULL)
        {
            if (stmt->next->type == IF_STMT)
            {
                stmt->next->next = stmt->next->if_stmt->false_branch;
                stmt->next->if_stmt->false_branch->next = stmtList;
            }
        }
        else
            stmt->next = stmtList;
    }
    else if (t.token_type == RBRACE)
    {
        // stmt_list -> stmt
        if(nintendo)
        {
            StatementNode* current = stmt;
            //Alter all cases to fix missing info

            while(current->next != NULL)
            {
                current = current->next;
            }
            current->next = NULL;
        }
        else if(stmt->type == IF_STMT)
        {
            stmt->next = stmt->if_stmt->false_branch;
            stmt->if_stmt->false_branch->next = NULL;
        }
        else if(stmt->next != NULL)
        {
            if (stmt->next->type == IF_STMT)
            {
                stmt->next->next = stmt->next->if_stmt->false_branch;
                stmt->next->if_stmt->false_branch->next = NULL;
            }
        }
        else
            stmt->next = NULL;
    }
    else
    {
        syntax_error();
    }

    if(errorFind)
    {
        cout << "Finished " << "parse_stmt_list" << endl;
    }

    return stmt;
}

//stmt -> assign_stmt
//stmt -> print_stmt
//stmt -> while_stmt
//stmt -> if_stmt
//stmt -> switch_stmt
StatementNode * Parser::parse_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_stmt" << endl;

    StatementNode* stmt = new StatementNode;

    //stmt -> assign_stmt
    //stmt -> print_stmt
    //stmt -> while_stmt
    //stmt -> if_stmt
    //stmt -> switch_stmt
    //stmt -> for_stmt
    Token t = peek();

    if (t.token_type == ID)
    {
        // stmt -> assign_stmt
        if(errorFind)
            cout << "\nStatement type: Assign" << endl;
        stmt = parse_assign_stmt();
    }
    else if (t.token_type == PRINT)
    {
        //stmt -> print_stmt
        if(errorFind)
            cout << "\nStatement type: Print" << endl;
        stmt = parse_print_stmt();
    }
    else if (t.token_type == WHILE)
    {
        // stmt -> while_stmt
        if(errorFind)
            cout << "\nStatement type: While" << endl;
        stmt = parse_while_stmt();
    }
    else if (t.token_type == IF)
    {
        //stmt -> if_stmt
        if(errorFind)
            cout << "\nStatement type: If" << endl;
        stmt = parse_if_stmt();
    }
    else if (t.token_type == SWITCH)
    {
        // stmt -> switch_stmt
        if(errorFind)
            cout << "\nStatement type: Switch" << endl;
        stmt = parse_switch_stmt();
    }
    else
    {
        syntax_error();
    }

    if(errorFind)
        cout << "Finished " << "parse_stmt" << endl;

    return stmt;
}


//OLD:	assign_stmt -> ID EQUAL primary SEMICOLON
//OLD:	assign_stmt -> ID EQUAL expr SEMICOLON
//assign_stmt -> var_access EQUAL expr SEMICOLON
//TODO: Alter this
StatementNode* Parser::parse_assign_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_assign_stmt" << endl;

    StatementNode* stmt = new StatementNode;
    stmt->type = ASSIGN_STMT;
    AssignmentStatement* assignNode = new AssignmentStatement;
    stmt->assign_stmt = assignNode;
    ValueNode* tempNode;

    Token t = expect(ID);
    tempNode = symLookup(t.lexeme);
    assignNode->left_hand_side = tempNode;
    expect(EQUAL);
    t = peek();
    tempNode = parse_primary();
    Token t2 = peek();

    if((t2.token_type == PLUS) || (t2.token_type == MINUS) ||
       (t2.token_type == MULT) || (t2.token_type == DIV))
    {
        //assign_stmt -> ID EQUAL expr SEMICOLON
        lexer.UngetToken(t);
        ExprNode* exprNode;
        exprNode = parse_expr();
        assignNode->op = exprNode->arith;
        assignNode->operand1 = exprNode->op1;
        assignNode->operand2 = exprNode->op2;
    }
    else
    {
        //assign_stmt -> ID EQUAL primary SEMICOLON
        assignNode->operand1 = tempNode;
        assignNode->op = OPERATOR_NONE;
        assignNode->operand2 = NULL;
    }

    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_assign_stmt" << endl;

    return stmt;
}

//var_access -> ID | ID LBRAC expr RBRAC
//TODO: Implement this
void Parser::parse_var_access()
{

}

//OLD: expr -> primary op primary
//expr -> term PLUS expr
//expr -> term
//TODO: Alter this
Parser::ExprNode* Parser::parse_expr()
{
    if(errorFind)
        cout << "Starting " << "parse_expr" << endl;

    ExprNode* expr = new ExprNode;

    expr->op1 = parse_primary();
    expr->arith = parse_op();
    expr->op2 = parse_primary();

    if(errorFind)
        cout << "Finished " << "parse_expr" << endl;

    return expr;
}

//term -> factor MULT term
//term -> factor
//TODO: Implement this
void Parser::parse_term()
{

}

//factor -> LPAREN expr RPAREN
//factor -> NUM
//factor -> var_access
//TODO: Implement this
void Parser::parse_factor()
{

}

//if_stmt -> IF condition body
StatementNode* Parser::parse_if_stmt()
{
    StatementNode* stmt = new StatementNode;
    stmt->type = IF_STMT;
    IfStatement* ifNode = new IfStatement;
    stmt->if_stmt = ifNode;
    StatementNode* noOpNode = new StatementNode;
    noOpNode->type = NOOP_STMT;


    CondNode* condNode;

    expect(IF);
    condNode = parse_condition();
    ifNode->condition_operand1 = condNode->op1;
    ifNode->condition_op = condNode->condType;
    ifNode->condition_operand2 = condNode->op2;
    ifNode->true_branch = parse_body();

    StatementNode* current = ifNode->true_branch;
    //Find end of True Branch's body
    while(current->next != NULL)
    {
        current = current->next;
    }

    //append no-op node to end of TB's body
    current->next = noOpNode;
    ifNode->false_branch = noOpNode;

    return stmt;
}

//while_stmt -> WHILE condition body
StatementNode* Parser::parse_while_stmt()
{
    StatementNode* stmt = new StatementNode;
    stmt->type = IF_STMT;
    IfStatement* whileNode = new IfStatement;
    stmt->if_stmt = whileNode;
    StatementNode* noOpNode = new StatementNode;
    noOpNode->type = NOOP_STMT;
    StatementNode* gtStmt = new StatementNode;
    gtStmt->type = GOTO_STMT;
    GotoStatement* gtNode = new GotoStatement;
    gtStmt->goto_stmt = gtNode;
    gtNode->target = stmt;


    CondNode* condNode;

    expect(WHILE);
    condNode = parse_condition();
    whileNode->condition_operand1 = condNode->op1;
    whileNode->condition_op = condNode->condType;
    whileNode->condition_operand2 = condNode->op2;
    whileNode->true_branch = parse_body();

    StatementNode* current = whileNode->true_branch;
    //Find end of True Branch's body
    while (current->next != NULL)
    {
        current = current->next;
    }

    //append goto node to end of TB's body
    current->next = gtStmt;

    //append no-op node to end of TB's body
    gtStmt->next = noOpNode;
    whileNode->false_branch = noOpNode;

    return stmt;
}

//OLD:	condition -> primary relop primary
//condition -> expr relop expr
//TODO: Alter this
Parser::CondNode* Parser::parse_condition()
{
    CondNode* node = new CondNode;

    node->op1 = parse_primary();
    node->condType = parse_relop();
    node->op2 = parse_primary();

    return node;
}

//relop -> GREATER | LESS | NOTEQUAL
ConditionalOperatorType Parser::parse_relop()
{
    if(errorFind)
        cout << "Starting " << "parse_relop" << endl;

    //initialized so that CLion will stop complaining
    ConditionalOperatorType relop = CONDITION_NOTEQUAL;

    Token t = lexer.GetToken();
    if (t.token_type == GREATER)
    {
        //relop-> GREATER
        relop = CONDITION_GREATER;
    }
    else if (t.token_type == LESS)
    {
        //relop-> LESS
        relop = CONDITION_LESS;
    }
    else if (t.token_type == NOTEQUAL)
    {
        //relop-> NOTEQUAL
        relop = CONDITION_NOTEQUAL;
    }
    else
        syntax_error();

    if(errorFind)
        cout << "Finished " << "parse_relop" << endl;

    return relop;
}

//OLD:	switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//OLD:	switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
//switch_stmt -> SWITCH var_access LBRACE case_list RBRACE
//switch_stmt -> SWITCH var_access LBRACE case_list default_case RBRACE
//TODO: Alter this
StatementNode* Parser::parse_switch_stmt()
{
    /*
     * Working without an equals sign:
     *
     *
     * IF var == 1
	 * {
	 * 	    stmt list 1
	 * 	    goto label
	 * }
	 * IF var == 2
	 * {
	 * 	    stmt list 2
	 * 	    goto label
	 * }
	 * LABEL
	 *
	 *
	 * IF var != 1
	 * {
	 * 	    false branch:
     * 	        stmt list 1
	 *   	    goto label
	 * }
     * True Branch:
     *      noOpNode
	 * IF var != 2
	 * {
     *      false branch:
	 *  	    stmt list 2
	 * 	        goto label
	 * }
     * True Branch:
     *      noOpNode
	 * LABEL
     *
     *
     */


    StatementNode* stmt = new StatementNode;
    stmt->type = IF_STMT;
    IfStatement* switchNode = new IfStatement;
    stmt->if_stmt = switchNode;
    StatementNode* defaultStmt;
    StatementNode* label = new StatementNode;
    label->type = NOOP_STMT;
    label->next = NULL;

    expect(SWITCH);

    //expect(ID);
    Token t = lexer.GetToken();
    ValueNode* switchVar = symLookup(t.lexeme);
    expect(LBRACE);
    stmt = parse_case_list();


    //switch_stmt -> SWITCH ID LBRACE case_list RBRACE
    StatementNode* current = stmt;
    StatementNode* lastCase = stmt;
    //Alter all cases to fix missing info
    while (current->next != NULL)
    {
        if (current->type == IF_STMT)
        {
            current->if_stmt->condition_operand1 = switchVar;
            StatementNode* caseBodyCurrent = current->if_stmt->false_branch;
            while(caseBodyCurrent->next != NULL)
            {
                caseBodyCurrent = caseBodyCurrent->next;
            }
            caseBodyCurrent->goto_stmt->target = label;
            caseBodyCurrent->next = current->next;
        }

        if(current->next->next == NULL)
            lastCase = current;

        current = current->next;
    }

    /*
        StatementNode* current = stmt;
        //make sure all goto statements lead to same node
        while (current->next != NULL)
        {
            if(current->type == IF_STMT)
            {
                StatementNode* temp = current->if_stmt->false_branch;
                while(temp->next != NULL)
                {
                    temp = temp->next;
                }
                if (temp->type == GOTO_STMT)
                    if(temp->goto_stmt->target == NULL)
                        temp->goto_stmt->target = label;
            }
            current = current->next;
        }
     */


    t = peek();
    if(t.token_type == DEFAULT)
    {
        //switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
        defaultStmt = parse_default_case();
        lastCase->next->next = defaultStmt;

        current = defaultStmt;
        while (current->next != NULL)
        {
            current = current->next;
        }

        current->next = label;

    }
    else
        lastCase->next->next = label;


    expect(RBRACE);

    return stmt;
}

//case_list -> case case_list | case
StatementNode* Parser::parse_case_list()
{
    StatementNode* stmt = new StatementNode;
    stmt->type = IF_STMT;
    IfStatement* switchNode = new IfStatement;
    stmt->if_stmt = switchNode;
    StatementNode* noOpNode = new StatementNode;
    noOpNode->type = NOOP_STMT;

    /*
     * IF var != 1
	 * {
	 * 	    false branch:
     * 	        stmt list 1
	 *   	    goto label
	 * }
     * True Branch:
     *      noOpNode
     */

    CaseNode* caseNode = parse_case();

    switchNode->condition_op = CONDITION_NOTEQUAL;
    switchNode->condition_operand2 = caseNode->n;
    switchNode->false_branch = caseNode->body;
    switchNode->true_branch = noOpNode;

    stmt->next = noOpNode;

    Token t = peek();
    if(t.token_type == CASE)
    {
        //case_list -> case case_list
        noOpNode->next = parse_case_list();
    }
    else if((t.token_type == RBRACE) || (t.token_type == DEFAULT))
    {
        //case_list -> case
        noOpNode->next = NULL;
    }
    else
        syntax_error();

    return stmt;
}

//case -> CASE NUM COLON body
Parser::CaseNode* Parser::parse_case()
{
    CaseNode* caseNode = new CaseNode;
    StatementNode* gtStmt = new StatementNode;
    gtStmt->type = GOTO_STMT;
    GotoStatement* gtNode = new GotoStatement;
    gtStmt->goto_stmt = gtNode;

    expect(CASE);

    //expect(NUM);
    Token t = lexer.GetToken();
    caseNode->n = constNode(stoi(t.lexeme));

    expect(COLON);
    caseNode->body = parse_body();

    StatementNode* current = caseNode->body;
    //Find end of caseNode's body
    while(current->next != NULL)
    {
        current = current->next;
    }

    //append goto node to end of FB's body
    current->next = gtStmt;
    gtStmt->next = NULL;

    return caseNode;
}

//default_case -> DEFAULT COLON body
StatementNode* Parser::parse_default_case()
{
    expect(DEFAULT);
    expect(COLON);
    return parse_body();
}

//OLD:	print_stmt -> print ID SEMICOLON
//print_stmt -> print var_access SEMICOLON
//TODO: Alter this
StatementNode* Parser::parse_print_stmt()
{
    if(errorFind)
        cout << "Starting parse_print_stmt" << endl;

    StatementNode* stmt = new StatementNode;
    stmt->type = PRINT_STMT;
    PrintStatement* printNode = new PrintStatement;
    stmt->print_stmt = printNode;

    expect(PRINT);
    Token t = expect(ID);
    printNode->id = symLookup(t.lexeme);
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished parse_print_stmt" << endl;

    return stmt;
}

//Teacher's function
StatementNode* Parser::ParseInput()
{
    if(errorFind)
        cout << "Starting " << "parse_input" << endl;

    StatementNode* startNode = parse_program();
    expect(END_OF_FILE);

    if(errorFind)
        cout << "Finished " << "parse_input" << endl;

    return startNode;
}


