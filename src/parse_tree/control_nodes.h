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
        if (cond) {
            delete cond;
        }
        if (ifBody) {
            delete ifBody;
        }
        if (elseBody) {
            delete elseBody;
        }
    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << "if (";
        cond->print(0);
        cout << ")" << endl;
        ifBody->print(ind + (dynamic_cast<BlockNode*>(ifBody) ? 0 : 4));

        if (elseBody) {
            cout << endl;
            cout << string(ind, ' ') << "else" << endl;
            elseBody->print(ind + (dynamic_cast<BlockNode*>(elseBody) ? 0 : 4));
        }
    }
    
    virtual void generateQuad(GenerationContext* generationContext) {
        int label1 = generationContext->labelCounter++;
        cond->generateQuad(generationContext);
        if (elseBody) {
            int label2 = generationContext->labelCounter++;
            cout << "JZ L" << label1 << endl;
            
            ifBody->generateQuad(generationContext);
            
            cout << "JMP L" << label2 << endl;
            cout << "L" << label1 << ":" << endl;
            
            elseBody->generateQuad(generationContext);
            
            cout << "L" << label2 << ":" << endl;
        } else {
            cout << "JZ L" << label1 << endl;
            
            ifBody->generateQuad(generationContext);
            
            cout << "L" << label1 << ":" << endl;
        }
    }
};

/**
 * The node class holding a switch statement in the parse tree.
 */
struct SwitchNode : public StatementNode {
    ExpressionNode* cond;
    CaseList caseList;

    SwitchNode(const Location& loc, ExpressionNode* cond, const CaseList& caseList) : StatementNode(loc) {
        this->cond = cond;
        this->caseList = caseList;
    }

    virtual ~SwitchNode() {
        if (cond) {
            delete cond;
        }
        
        for (int i = 0; i < caseList.size(); ++i) {
            delete caseList[i];
        }
    }

    virtual void print(int ind = 0) {
        cout << string(ind, ' ') << "switch (";
        cond->print(0);
        cout << ") {" << endl;
        
        for (int i = 0; i < caseList.size(); ++i) {
            caseList[i]->print(ind + 4);
        }

        cout << string(ind, ' ') << "}";
    }
    
    virtual void generateQuad(GenerationContext* generationContext) {
        int label1 = generationContext->labelCounter++;
        generationContext->breakLabels.push(label1);
        
        for (int i = 0; i < caseList.size(); ++i) {
            cond->generateQuad(generationContext);
            caseList[i]->generateQuad(generationContext);
        }
        
        generationContext->breakLabels.pop();
        cout << "L" << label1 << ":" << endl;
    }
};

#endif