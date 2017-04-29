/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <algorithm>
#include "parser.h"
using namespace std;

/********************
 *  TEST VARIABLES  *
 ********************/

bool errorFind = false;
bool testIf = false;
bool testSwitch = false;
int testNum = 1;
//cout << "test " << testNum++ << endl;


/*******************************
 *  GLOBAL STRUCTS AND UNIONS  *
 *******************************/

struct Parser::CondNode
{
    ExprNode* op1;
    ExprNode* op2;
    ConditionalOperatorType condType;
};

struct Parser::CaseNode
{
    ValueNode* n;
    StatementNode* body;
};

struct Parser::ArrNode
{
    std::string name;
    int size;
    vector<ValueNode*> valNodes;
};

union Parser::varNode
{
    ValueNode* intNode;
    ArrNode* arrNode;
};

struct Parser::myVar
{
    varNode* var;
    TokenType type;
};

vector<Parser::myVar*> symTable;


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
	{
		cout << "Token Type: " << t.token_type << endl;
		syntax_error();
	}


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
        if(symTable[i]->type == NUM)
            cout << symTable[i]->var->intNode->name << " ";
        else
            cout << symTable[i]->var->arrNode->name << " ";
    }
    cout << "#" << endl;
}

//Check to see if item is in symbol table
Parser::myVar* Parser::symLookup(string name, TokenType type)
{
    if(errorFind)
        cout << "Starting " << "symLookup" << endl;
    myVar* tempNode = new myVar;
    bool found = false;
    for(int iter = 0; iter < (int)symTable.size(); iter++)
    {
        if(symTable[iter]->type == NUM)
        {
            //Remember, string comparison returns 0 if strings are equal
            if (name.compare(symTable[iter]->var->intNode->name) == 0)
            {
                tempNode = symTable[iter];
                found = true;
            }
        }
        else
        {
            if (name.compare(symTable[iter]->var->arrNode->name) == 0)
            {
                tempNode = symTable[iter];
                found = true;
            }
        }
    }

    //Symbol not in table, so add it
    if(!found && type == ARRAY)
        tempNode = addArrNode(name);
    else if(!found) //if type is unknow, assume NUM
        tempNode = addValNode(name);

    if(errorFind)
        cout << "Finished " << "symLookup" << endl;

    return tempNode;
}

//Adds new ValueNode to symbol table
Parser::myVar* Parser::addValNode(string name)
{
    if(errorFind)
        cout << "Starting " << "addValNode" << endl;

    myVar* temp = new myVar;
    temp->var = new varNode;
    temp->var->intNode = new ValueNode;
    temp->type = NUM;
    temp->var->intNode->name = name;
    symTable.push_back(temp);
    temp = symLookup(name, NUM); //yay recursion

    if(errorFind)
        cout << "Finished " << "addValNode" << endl;

    return temp;
}

//Adds new ArrNode to symbol table
Parser::myVar* Parser::addArrNode(string name)
{
    if(errorFind)
        cout << "Starting " << "addArrNode" << endl;

    myVar* temp = new myVar;
    temp->type = ARRAY;
    temp->var = new varNode;
    temp->var->arrNode = new ArrNode;
    temp->var->arrNode->name = name;
    symTable.push_back(temp);
    temp = symLookup(name, ARRAY); //yay recursion

    if(errorFind)
        cout << "Finished " << "addArrNode" << endl;

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

//Some evaluation is done while parsing
//this resets it for compiling
void resetValues()
{
	for(int iter = 0; iter < (int)symTable.size(); iter++)
	{
		if(symTable[iter]->type == NUM)
		{
			symTable[iter]->var->intNode->value = 0;
		}
		else
		{
			for(int j = 0; j < symTable[iter]->var->arrNode->size; j++)
			{
				symTable[iter]->var->arrNode->valNodes[j]->value = 0;
			}
		}
	}
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
void Parser::parse_var_section()
{
    if(errorFind)
        cout << "Starting " << "parse_var_section" << endl;

    expect(VAR);
    parse_int_var_decl();
    parse_array_var_decl();

    if(errorFind)
        cout << "Finished " << "parse_var_section" << endl;
}

//int_var_decl -> id_list SEMICOLON
void Parser::parse_int_var_decl()
{
    if(errorFind)
        cout << "Starting " << "parse_int_var_decl" << endl;

    parse_id_list(false);
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_int_var_decl" << endl;
}

//array_var_decl -> id_list COLON ARRAY LBRAC NUM RBRAC SEMICOLON
void Parser::parse_array_var_decl()
{
    if(errorFind)
        cout << "Starting " << "parse_array_var_decl" << endl;

    //idea: get token of start of idlist, parse idList,
    //      expect until num, store num, unget token to
    //      start of idList, then parse idlist again
    //      with proper size
    //      problem:    don't want to store things with first call
    //      solution:   if arr, don't store unless size != 0
    parse_id_list(true);
    expect(COLON);
    expect(ARRAY);
    expect(LBRAC);
    Token t = lexer.GetToken();
    for(int i = 0; i < (int)symTable.size(); i++)
    {
        if(symTable[i]->type == ARRAY)
        {
            symTable[i]->var->arrNode->size = stoi(t.lexeme);
            for(int j = 0; j < symTable[i]->var->arrNode->size; j++)
            {
                string name = symTable[i]->var->arrNode->name;
				name = name + to_string(j);
				ValueNode* temp = new ValueNode;
				temp->name = name;
				symTable[i]->var->arrNode->valNodes.push_back(temp);
            }
        }
    }
    expect(RBRAC);
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_array_var_decl" << endl;
}

//id_list -> ID COMMA id_list | ID
//parameter: 1 if array input, 0 if int input
void Parser::parse_id_list(bool arr)
{
    if(errorFind)
        cout << "Starting " << "parse_id_list" << endl;

    // id_list -> ID
    // id_list -> ID COMMA id_list
    myVar* tmpSym = new myVar;
    Token t = expect(ID);
    if(arr)
    {
        tmpSym->var = new varNode;
        tmpSym->var->arrNode = new ArrNode;
        tmpSym->type = ARRAY;
        tmpSym->var->arrNode->name = t.lexeme;
        symTable.push_back(tmpSym);
    }
    else if(!arr)
    {
        tmpSym->var = new varNode;
        tmpSym->var->intNode = new ValueNode;
        tmpSym->type = NUM;
        tmpSym->var->intNode->name = t.lexeme;
        symTable.push_back(tmpSym);
    }
    t = lexer.GetToken();
    if (t.token_type == COMMA)
    {
        // id_list -> ID COMMA id_list
        //General case
        parse_id_list(arr);
    }
    else if ((t.token_type == SEMICOLON) || (t.token_type == COLON))
    {
        // id_list -> ID
        lexer.UngetToken(t);
    }
    else
    {
        syntax_error();
    }

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


//assign_stmt -> var_access EQUAL expr SEMICOLON
StatementNode* Parser::parse_assign_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_assign_stmt" << endl;

    StatementNode* stmt = new StatementNode;
    stmt->type = ASSIGN_STMT;
    AssignmentStatement* assignNode = new AssignmentStatement;
    stmt->assign_stmt = assignNode;
    ValueNode* tempNode;

    tempNode = parse_var_access();
	assignNode->left_hand_side = tempNode;

    expect(EQUAL);

    //assign_stmt -> var_access EQUAL expr SEMICOLON
    ExprNode* exprNode;
    exprNode = parse_expr();
    assignNode->op = exprNode->arith;
    assignNode->operand1 = exprNode->op1;
    assignNode->operand2 = exprNode->op2;
	stmt->assign_stmt->left_hand_side->value = evalExpr(exprNode);

	Token t = peek();
	expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_assign_stmt" << endl;

    return stmt;
}

//var_access -> ID | ID LBRAC expr RBRAC
ValueNode* Parser::parse_var_access()
{
    if(errorFind)
        cout << "Starting " << "parse_var_access" << endl;

    Token t = lexer.GetToken();
    Token t2 = lexer.GetToken();
    myVar* temp;
    ValueNode* node;

    if(t2.token_type == LBRAC)
	{
		//var_access -> ID LBRAC expr RBRAC
		temp = symLookup(t.lexeme, ARRAY);
		ExprNode* pos = parse_expr();
		node = temp->var->arrNode->valNodes[evalExpr(pos)];
		expect(RBRAC);
	}
    else
    {
        //var_access -> ID
        lexer.UngetToken(t2);
        temp = symLookup(t.lexeme, NUM);
        node = temp->var->intNode;
    }

    if(errorFind)
        cout << "Finished " << "parse_var_access" << endl;

    return node;
}

//expr -> term PLUS expr
//expr -> term
ExprNode* Parser::parse_expr()
{
    if(errorFind)
        cout << "Starting " << "parse_expr" << endl;

    ExprNode* expr;

    expr = parse_term();
    Token t = lexer.GetToken();
	ExprNode* current = expr;
	while(current->arith != OPERATOR_NONE)
	{
		current = current->op2;
	}
    if(t.token_type == PLUS)
    {
        current->arith = OPERATOR_PLUS;
        current->op2 = parse_expr();
    }
    else
    {
        lexer.UngetToken(t);
        current->arith = OPERATOR_NONE;
        current->op2 = NULL;
    }


    if(errorFind)
        cout << "Finished " << "parse_expr" << endl;

    return expr;
}

//term -> factor MULT term
//term -> factor
ExprNode* Parser::parse_term()
{
    if(errorFind)
        cout << "Starting " << "parse_term" << endl;

	ExprNode* node = new ExprNode;
    node->op1 = parse_factor();
    Token t = lexer.GetToken();
    if(t.token_type == MULT)
    {
		node->arith = OPERATOR_MULT;
		node->op2 = parse_term();
	}
    else
	{
		node->arith = OPERATOR_NONE;
		lexer.UngetToken(t);
	}

    if(errorFind)
        cout << "Finished " << "parse_term" << endl;

    return node;
}

//factor -> LPAREN expr RPAREN
//factor -> NUM
//factor -> var_access
ValueNode* Parser::parse_factor()
{
    if(errorFind)
        cout << "Starting " << "parse_factor" << endl;

    ValueNode* node;
    ExprNode* expr;
    Token t = lexer.GetToken();

    if(t.token_type == LPAREN)
    {
        expr = parse_expr();
		node = constNode(evalExpr(expr));
		expect(RPAREN);
    }
    else if(t.token_type == NUM)
        node = constNode(stoi(t.lexeme));
    else
	{
		lexer.UngetToken(t);
		node = parse_var_access();
	}

    if(errorFind)
        cout << "Finished " << "parse_factor" << endl;

    return node;
}

//if_stmt -> IF condition body
StatementNode* Parser::parse_if_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_if_stmt" << endl;

    StatementNode* stmt = new StatementNode;
    stmt->type = IF_STMT;
    IfStatement* ifNode = new IfStatement;
    stmt->if_stmt = ifNode;
    StatementNode* noOpNode = new StatementNode;
    noOpNode->type = NOOP_STMT;


    CondNode* condNode;

    expect(IF);
    condNode = parse_condition();
    ifNode->condition_operand1 = condNode->op1->op1;
    ifNode->condition_op = condNode->condType;
    ifNode->condition_operand2 = condNode->op2->op1;
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

    if(errorFind)
        cout << "finished " << "parse_if_stmt" << endl;

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
    whileNode->condition_operand1 = condNode->op1->op1;
    whileNode->condition_op = condNode->condType;
    whileNode->condition_operand2 = condNode->op2->op1;
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

//condition -> expr relop expr
Parser::CondNode* Parser::parse_condition()
{
    if(errorFind)
        cout << "Starting " << "parse_condition" << endl;

    CondNode* node = new CondNode;

    node->op1 = parse_expr();
    node->condType = parse_relop();
    node->op2 = parse_expr();

    if(errorFind)
        cout << "Finished " << "parse_condition" << endl;

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
    ValueNode* switchVar = parse_var_access();
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


    Token t = peek();
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
StatementNode* Parser::parse_print_stmt()
{
    if(errorFind)
        cout << "Starting parse_print_stmt" << endl;

    StatementNode* stmt = new StatementNode;
    stmt->type = PRINT_STMT;
    PrintStatement* printNode = new PrintStatement;
    stmt->print_stmt = printNode;

    expect(PRINT);
    printNode->id = parse_var_access();
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


