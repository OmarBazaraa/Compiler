#ifndef __BASIC_NODES_H_
#define __BASIC_NODES_H_

#include <iostream>
#include <string>
#include <vector>

#include "../context/scope_context.h"

#include "../symbol_table/symbol_table.h"

#include "../utils/consts.h"
#include "../utils/utils.h"


//
// Prototypes
//
struct StatementNode;
struct ExpressionNode;
struct VarDeclarationNode;
struct CaseStmtNode;
struct FunctionNode;

typedef vector<StatementNode*> StmtList;
typedef vector<ExpressionNode*> ExprList;
typedef vector<VarDeclarationNode*> VarList;
typedef vector<CaseStmtNode*> CaseList;


/**
 * The base class of all statement nodes in the parse tree.
 */
struct StatementNode {
    Location loc;

    StatementNode() {}

    StatementNode(const Location& loc) {
        this->loc = loc;
    }

    ~StatementNode() {}

    virtual bool analyze(ScopeContext* context) {
        return true;
    }

    virtual string toString(int ind = 0) {
        return string(ind, ' ') + ";" ;
    }
};

/**
 * The base class of all expression nodes in the parse tree.
 */
struct ExpressionNode : public StatementNode {
    //
    // NOTE: the following variables will be computed after calling analyze function
    //
    DataType type = DTYPE_ERROR;    // data type of the expression
    Symbol* reference = NULL;       // reference of the expression is exist

    ExpressionNode() {}

    ExpressionNode(const Location& loc) : StatementNode(loc) {}

    virtual DataType getType() {
        return type;
    }

    virtual bool isReference() {
        return type;
    }
};

/**
 * An expression container class.
 */
struct ExprContainerNode : public ExpressionNode {
    ExpressionNode* expr;

    ExprContainerNode(const Location& loc, ExpressionNode* expr) : ExpressionNode(loc) {
        this->expr = expr;
    }

    virtual ~ExprContainerNode() {
        if (expr) delete expr;
    }

    virtual bool analyze(ScopeContext* context) {
        if (!context->initializeVar && context->isGlobalScope()) {
            context->printError("expression is not allowed in global scope", loc);
            return false;
        }

        bool ret = expr->analyze(context);

        type = expr->type;
        reference = expr->reference;

        return ret;
    }

    virtual string toString(int ind = 0) {
        return expr->toString(ind);
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

    virtual bool analyze(ScopeContext* context) {
        context->printError(what, loc);
        return false;
    }

    virtual string toString(int ind) {
        return string(ind, ' ') + ">> ERROR" ;
    }
};

#endif