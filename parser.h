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
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
    ValueNode* symLookup(std::string name);
    ValueNode* addValNode(std::string name);
    ValueNode* constNode(int val);
    void printStatementList(StatementNode* head);

public:
    void print();
    StatementNode* ParseInput();
    StatementNode* parse_program();
    void parse_var_section();
    void parse_id_list();
    StatementNode* parse_body();
    StatementNode* parse_stmt_list();
    StatementNode* parse_stmt();
    StatementNode* parse_assign_stmt();
    ExprNode* parse_expr();
    ValueNode* parse_primary();
    ArithmeticOperatorType parse_op();
    StatementNode* parse_print_stmt();
    StatementNode* parse_while_stmt();
    StatementNode* parse_if_stmt();
    CondNode* parse_condition();
    ConditionalOperatorType parse_relop();
    StatementNode* parse_switch_stmt();
    StatementNode* parse_for_stmt();
    StatementNode* parse_case_list();
    CaseNode* parse_case();
    StatementNode* parse_default_case();
};

#endif

