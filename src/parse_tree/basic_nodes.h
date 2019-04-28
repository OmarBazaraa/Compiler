#ifndef __BASIC_NODES_H_
#define __BASIC_NODES_H_

#include <iostream>
#include <string>
#include <vector>

#include "../context/context.h"
#include "../utils/consts.h"
#include "../utils/utils.h"


//
// Prototypes
//
struct Node;
struct StatementNode;
struct ExpressionNode;
struct VarDeclarationNode;
struct CaseStmtNode;

typedef vector<StatementNode*> StmtList;
typedef vector<ExpressionNode*> ExprList;
typedef vector<VarDeclarationNode*> VarList;
typedef vector<CaseStmtNode*> CaseList;


/**
 * The base class of all parse tree nodes.
 */
struct Node {

    virtual ~Node() {

    }
    
    virtual bool analyze(Context* context) {
        return true;
    }

    virtual void print(int ind = 0) {
        
    }
};

/**
 * The base class of all statement nodes in the parse tree.
 */
struct StatementNode : Node {
    Location loc;

    StatementNode() {

    }

    StatementNode(const Location& loc) {
        this->loc = loc;
    }

    virtual ~StatementNode() {

    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << ";" ;
    }
};

/**
 * The base class of all expression nodes in the parse tree.
 */
struct ExpressionNode : public StatementNode {

    ExpressionNode() {

    }

    ExpressionNode(const Location& loc) : StatementNode(loc) {
        this->loc = loc;
    }
};

/**
 * The node class representing a syntax error statement.
 */
struct ErrorNode : public StatementNode {
    string what;

    ErrorNode(const Location& loc, const string& what) {
        this->loc = loc;
        this->what = what;

        this->loc.pos -= this->loc.len - 1;
    }

    ~ErrorNode() {

    }

    virtual bool analyze(Context* context) {
        printf("%s\n", context->sourceCode[loc.lineNum - 1].c_str());
        printf("%*s\n", loc.pos, "^");
        return false;
    }

    virtual void print(int ind) {
        cout << string(ind, ' ') << "ERROR;" ;
    }
};

#endif