#ifndef __LOOP_NODES_H_
#define __LOOP_NODES_H_

#include "basic_nodes.h"
#include "statement_nodes.h"


/**
 * The node class holding a while loop in the parse tree.
 */
struct WhileNode : public StatementNode {
    ExpressionNode* cond;
    StatementNode* body;

    WhileNode(ExpressionNode* cond, StatementNode* body) {
        this->cond = cond;
        this->body = body;
    }

    virtual ~WhileNode() {
        if (cond) {
            delete cond;
        }
        if (body) {
            delete body;
        }
    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << "while (";
        cond->print(0);
        cout << ") " << endl;
        body->print(ind + (dynamic_cast<BlockNode*>(body) ? 0 : 4));
    }
};

/**
 * The node class holding a do-while loop in the parse tree.
 */
struct DoWhileNode : public StatementNode {
    ExpressionNode* cond;
    StatementNode* body;

    DoWhileNode(ExpressionNode* cond, StatementNode* body) {
        this->cond = cond;
        this->body = body;
    }

    virtual ~DoWhileNode() {
        if (cond) {
            delete cond;
        }
        if (body) {
            delete body;
        }
    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << "do" << endl;
        body->print(ind + (dynamic_cast<BlockNode*>(body) ? 0 : 4));
        cout << endl;
        cout << string(ind, ' ') << "while (";
        cond->print(0);
        cout << ");";
    }
};

/**
 * The node class holding a for loop in the parse tree.
 */
struct ForNode : public StatementNode {
    StatementNode* initStmt;
    ExpressionNode* cond;
    ExpressionNode* inc;
    StatementNode* body;

    ForNode(StatementNode* initStmt, ExpressionNode* cond, ExpressionNode* inc, StatementNode* body) {
        this->initStmt = initStmt;
        this->cond = cond;
        this->inc = inc;
        this->body = body;
    }

    virtual ~ForNode() {
        if (initStmt) {
            delete initStmt;
        }
        if (cond) {
            delete cond;
        }
        if (inc) {
            delete inc;
        }
        if (body) {
            delete body;
        }
    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << "for (";

        if (initStmt) {
            initStmt->print(0);
        }
        cout << "; ";

        if (cond) {
            cond->print(0);
        }
        cout << "; ";

        if (inc) {
            inc->print(0);
        }

        cout << ")" << endl;

        body->print(ind + (dynamic_cast<BlockNode*>(body) ? 0 : 4));
    }
};

#endif