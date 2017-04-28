/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <cstdlib>
#include "lexer.h"
#include "compiler.h"

class Parser {
  private:
    struct ExprNode;
    struct CondNode;
    struct CaseNode;
    union variables;
    struct myVar;
    struct ArrNode;
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
    myVar* symLookup(std::string name, TokenType type);
    myVar* addValNode(std::string name);
    myVar* addArrNode(std::string name);
    ValueNode* constNode(int val);
    void printStatementList(StatementNode* head);

public:
    void print();
    StatementNode* ParseInput();
    StatementNode* parse_program();
    void parse_var_section();
    void parse_int_var_decl();
    void parse_array_var_decl();
    void parse_id_list(bool arr, int size);
    StatementNode* parse_body();
    StatementNode* parse_stmt_list();
    StatementNode* parse_stmt();
    StatementNode* parse_assign_stmt();
    ValueNode* parse_var_access();
    ExprNode* parse_expr();
    ValueNode* parse_term();
    ValueNode* parse_factor();
    StatementNode* parse_print_stmt();
    StatementNode* parse_while_stmt();
    StatementNode* parse_if_stmt();
    CondNode* parse_condition();
    ConditionalOperatorType parse_relop();
    StatementNode* parse_switch_stmt();
    StatementNode* parse_case_list();
    CaseNode* parse_case();
    StatementNode* parse_default_case();
};

#endif

