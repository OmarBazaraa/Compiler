#ifndef __CONTROL_NODES_H_
#define __CONTROL_NODES_H_

#include "../context/generation_context.h"
#include "basic_nodes.h"
#include "statement_nodes.h"


/**
 * The node class holding an if statement in the parse tree.
 */
struct IfNode : public StatementNode {
    ExpressionNode* cond;
    StatementNode* ifBody;
    StatementNode* elseBody;

    IfNode(const Location& loc, ExpressionNode* cond, StatementNode* ifBody, StatementNode* elseBody = NULL) : StatementNode(loc) {
        this->cond = cond;
        this->ifBody = ifBody;
        this->elseBody = elseBody;
    }

    virtual ~IfNode() {
        if (cond) delete cond;
        if (ifBody) delete ifBody;
        if (elseBody) delete elseBody;
    }

    virtual bool analyze(ScopeContext* context) {
        if (context->isGlobalScope()) {
            context->printError("if-statement is not allowed in global scope", loc);
            return false;
        }

        bool ret = true;

        context->addScope(SCOPE_IF);

        ret &= cond->analyze(context);
        ret &= ifBody->analyze(context);

        if (elseBody) {
            ret &= elseBody->analyze(context);
        }

        context->popScope();

        return ret;
    }

    virtual string toString(int ind = 0) {
        string ret = string(ind, ' ') + "if (" + cond->toString() + ")\n";
        ret += ifBody->toString(ind + (dynamic_cast<BlockNode*>(ifBody) ? 0 : 4));

        if (elseBody) {
            ret += "\n" + string(ind, ' ') + "else\n";
            ret += elseBody->toString(ind + (dynamic_cast<BlockNode*>(elseBody) ? 0 : 4));
        }

        return ret;
    }

    virtual string generateQuad(GenerationContext* generationContext) {
        string ret = "";
        int label1 = generationContext->labelCounter++;
		
        ret += cond->generateQuad(generationContext);
		ret += Utils::oprToQuad(Operator::OPR_JZ, cond->type) + "L" + to_string(label1) + "\n";
		ret += ifBody->generateQuad(generationContext);
		
        if (elseBody) {
            int label2 = generationContext->labelCounter++;

            ret += Utils::oprToQuad(Operator::OPR_JMP, DataType::DTYPE_ERROR) + "L" + to_string(label2) + "\n";
            ret += "L" + to_string(label1) + ":\n";
            ret += elseBody->generateQuad(generationContext);
            ret += "L" + to_string(label2) + ":\n";
        }
        else {
            ret += "L" + to_string(label1) + ":\n";
        }

        return ret;
    }

};

/**
 * The node class holding a case statement in the parse tree.
 */
struct CaseLabelNode : public StatementNode {
    ExpressionNode* expr;
    StatementNode* stmt;

    CaseLabelNode(const Location& loc, ExpressionNode* expr, StatementNode* stmt) : StatementNode(loc) {
        this->expr = expr;
        this->stmt = stmt;
    }

    virtual ~CaseLabelNode() {
        if (expr) delete expr;
        if (stmt) delete stmt;
    }

    virtual bool analyze(ScopeContext* context) {
        Switch* switchStmt = context->switches.empty() ? NULL : context->switches.top();

        if (switchStmt == NULL) {
            context->printError("case label not within switch statement", loc);
            return false;
        }

        bool ret = true;

        if (expr) {     // case label
            ret = expr->analyze(context);

            if (ret && !expr->isConst) {
                context->printError("constant expression required in case label", expr->loc);
                ret = false;
            }
            if (ret && !Utils::isIntegerType(expr->type)) {
                context->printError("case quantity not an integer", expr->loc);
                ret = false;
            }
            if (ret && expr->isConst && Utils::isIntegerType(expr->type)) {
                // TODO: calculate this value
                // int val = 0;

                // if (switchStmt->labels.count(val)) {
                //     context->printError("duplicate case value", loc);
                //     ret = false;
                // }

                // switchStmt->labels.insert(val);
            }
        }
        else {          // default label
            if (switchStmt->defaultLabel) {
                context->printError("multiple default labels in one switch", loc);
                ret = false;
            }

            switchStmt->defaultLabel = true;
        }

        ret &= stmt->analyze(context);

        return ret;
    }

    virtual string toString(int ind = 0) {
        string ret = string(max(0, ind - 4), ' ') + (expr ? "case " + expr->toString() + ":\n" : "default:\n");
        ret += stmt->toString(ind);
        return ret;
    }

    virtual string generateQuad(GenerationContext* generationContext) {
        if (expr == NULL) {
            return stmt->generateQuad(generationContext);
        }

        string ret = "";
        int label1 = generationContext->labelCounter++;

		// @AbdoEed The type of this push is incorrect it should be the type of the switch expr. not the case expr.
        ret += Utils::oprToQuad(Operator::OPR_PUSH, expr->type) + "SWITCH_COND@" + to_string(generationContext->breakLabels.top()) + "\n";
        ret += expr->generateQuad(generationContext);
        ret += Utils::oprToQuad(Operator::OPR_EQUAL, expr->type);
        ret += Utils::oprToQuad(Operator::OPR_JZ, DataType::DTYPE_BOOL) + "L" + to_string(label1) + "\n";
        ret += stmt->generateQuad(generationContext);
        ret += "L" + to_string(label1) + ":\n";

        return ret;
    }
};

/**
 * The node class holding a switch statement in the parse tree.
 */
struct SwitchNode : public StatementNode {
    ExpressionNode* cond;
    StatementNode* body;
    Switch switchStmt;

    SwitchNode(const Location& loc, ExpressionNode* cond, StatementNode* body) : StatementNode(loc) {
        this->cond = cond;
        this->body = body;
    }

    virtual ~SwitchNode() {
        if (cond) delete cond;
        if (body) delete body;
    }

    virtual bool analyze(ScopeContext* context) {
        if (context->isGlobalScope()) {
            context->printError("switch-statement is not allowed in global scope", loc);
            return false;
        }

        bool ret = true;

        context->addScope(SCOPE_SWITCH);
        context->switches.push(&switchStmt);

        ret &= cond->analyze(context);

        if (!Utils::isIntegerType(cond->type)) {
            context->printError("switch quantity not an integer", cond->loc);
            ret = false;
        }

        ret &= body->analyze(context);

        context->switches.pop();
        context->popScope();

        return ret;
    }

    virtual string toString(int ind = 0) {
        string ret = string(ind, ' ') + "switch (" + cond->toString() + ")\n";
        ret += body->toString(ind + (dynamic_cast<BlockNode*>(body) ? 0 : 4));
        return ret;
    }

    virtual string generateQuad(GenerationContext* generationContext) {
        string ret = "";
        int label1 = generationContext->labelCounter++;

        ret += cond->generateQuad(generationContext);
        ret += Utils::oprToQuad(Operator::OPR_POP, cond->type) + "SWITCH_COND@" + to_string(label1) + "\n";
        generationContext->breakLabels.push(label1);

        ret += body->generateQuad(generationContext);

        generationContext->breakLabels.pop();
        ret += "L" + to_string(label1) + ":\n";

        return ret;
    }
};

#endif