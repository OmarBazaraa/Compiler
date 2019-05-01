#ifndef __OPERATOR_NODES_H_
#define __OPERATOR_NODES_H_

#include "basic_nodes.h"
#include "value_nodes.h"


/**
 * The node class holding an assignment operator in the parse tree.
 */
struct AssignOprNode : public ExpressionNode {
    ExpressionNode* lhs;
    ExpressionNode* rhs;

    AssignOprNode(const Location& loc, ExpressionNode* lhs, ExpressionNode* rhs) : ExpressionNode(loc) {
        this->lhs = lhs;
        this->rhs = rhs;
    }

    virtual ~AssignOprNode() {
        if (lhs) delete lhs;
        if (rhs) delete rhs;
    }

    virtual bool analyze(ScopeContext* context) {
        if (!(rhs->analyze(context) & lhs->analyze(context))) {
            // Note that I used a bitwise and to execute both lhs and rhs expressions
            return false;
        }

        type = lhs->type;
        reference = lhs->reference;

        if (reference) {
            if (dynamic_cast<Var*>(reference) == NULL) {
                context->printError("assignment of function '" + reference->header() + "'", rhs->loc);
                return false;
            }
            else if (((Var*) reference)->isConst) {
                context->printError("assignment of read-only variable '" + reference->header() + "'", rhs->loc);
                return false;
            }
        }
        else {
            context->printError("lvalue required as left operand of assignment", lhs->loc);
            return false;
        }

        return true;
    }

    virtual string toString(int ind = 0) {
        return string(ind, ' ') + "(" + lhs->toString() + " = " + rhs->toString() + ")";
    }
};

/**
 * The node class holding a binary operator in the parse tree.
 */
struct BinaryOprNode : public ExpressionNode {
    Operator opr;
    ExpressionNode* lhs;
    ExpressionNode* rhs;

    BinaryOprNode(const Location& loc, Operator opr, ExpressionNode* lhs, ExpressionNode* rhs) : ExpressionNode(loc) {
        this->opr = opr;
        this->lhs = lhs;
        this->rhs = rhs;
    }

    virtual ~BinaryOprNode() {
        if (lhs) delete lhs;
        if (rhs) delete rhs;
    }

    virtual bool analyze(ScopeContext* context) {
        if (!(lhs->analyze(context) & rhs->analyze(context))) {
            // Note that I used a bitwise and to execute both lhs and rhs expressions
            return false;
        }

        // Note that lhs & rhs types are computed after calling analyze function
        type = max(lhs->type, rhs->type);

        //
        // TODO: handle function pointer
        //
        
        if (lhs->type == DTYPE_VOID || rhs->type == DTYPE_VOID) {
            context->printError("invalid operands of types '" + 
                Utils::dtypeToStr(lhs->type) + "' and '" + Utils::dtypeToStr(rhs->type) +
                "' to binary operator '" + Utils::oprToStr(opr) + "'", loc);
            return false;
        }

        return true;
    }

    virtual string toString(int ind = 0) {
        return string(ind, ' ') + "(" + lhs->toString() + " " + Utils::oprToStr(opr) + " " + rhs->toString() + ")";
    }
};

/**
 * The node class holding a unary operator in the parse tree.
 */
struct UnaryOprNode : public ExpressionNode {
    Operator opr;
    ExpressionNode* expr;

    UnaryOprNode(const Location& loc, Operator opr, ExpressionNode* expr) : ExpressionNode(loc) {
        this->opr = opr;
        this->expr = expr;
    }

    virtual ~UnaryOprNode() {
        if (expr) delete expr;
    }

    virtual bool analyze(ScopeContext* context) {
        if (!expr->analyze(context)) {
            return false;
        }

        type = expr->type;
        reference = expr->reference;

        if (expr->type == DTYPE_VOID) {
            context->printError("invalid operand of type '" + 
                Utils::dtypeToStr(expr->type) + "' to unary 'operator" + Utils::oprToStr(opr) + "'", loc);
            return false;
        }

        if (opr == OPR_SUF_INC || opr == OPR_PRE_INC || opr == OPR_SUF_DEC || opr == OPR_PRE_DEC) {
            if (reference) {
                if (dynamic_cast<Var*>(reference) == NULL) {
                    context->printError("increment/decrement of function '" + reference->header() + "'", expr->loc);
                    return false;
                }
                else if (((Var*) reference)->isConst) {
                    context->printError("increment/decrement of read-only variable '" + reference->header() + "'", expr->loc);
                    return false;
                }
            }
            else {
                context->printError("lvalue required as an operand of increment/decrement", expr->loc);
                return false;
            }
        }

        return true;
    }

    virtual string toString(int ind = 0) {
        string ret = string(ind, ' ') + "(";

        if (opr == OPR_SUF_INC || opr == OPR_SUF_DEC) {
            ret += expr->toString() + Utils::oprToStr(opr);
        } else {
            ret += Utils::oprToStr(opr) + expr->toString();
        }

        return ret += ")";
    }
};

#endif