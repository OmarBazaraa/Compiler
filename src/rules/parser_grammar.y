%{
#include <iostream>
#include <string>

#include "../parse_tree/parse_tree.h"

using namespace std;

//
// External functions & variables
//
extern int yylex();
extern FILE* yyin;
extern int curLineNum;
extern int curCursorPos;
extern int curTokenLen;

//
// Functions prototypes
//
void yyerror(const char* s);

//
// Global variables
//
Node* programRoot = NULL;
%}

// =====================================================================================================
// Symbol Types
// ============

%union {
    Node*               node;
    BlockNode*          blockNode;
    StatementNode*      StmtNode;
    VarDeclarationNode* varDeclNode;
    IfNode*             ifNode;
    SwitchNode*         switchNode;
    CaseStmtNode*       caseStmtNode;
    WhileNode*          whileNode;
    DoWhileNode*        doWhileNode;
    ForNode*            forNode;
    FunctionNode*       functionNode;
    FunctionCallNode*   functionCallNode;
    ReturnStmtNode*     returnStmtNode;
    ExpressionNode*     exprNode;
    TypeNode*           typeNode;
    ValueNode*          valueNode;
    IdentifierNode*     identifierNode;

    StmtList*           stmtList;
    ExprList*           exprList;
    VarList*            varList;
    CaseList*           caseList;

    Token               token;
    Location            location;
}

// =====================================================================================================
// Tokens Definition
// =================

// Data types
%token <location> TYPE_INT
%token <location> TYPE_FLOAT
%token <location> TYPE_CHAR
%token <location> TYPE_BOOL
%token <location> TYPE_VOID

// Keywords
%token <location> CONST
%token <location> IF
%token <location> ELSE
%token <location> SWITCH
%token <location> CASE
%token <location> DEFAULT
%token <location> FOR
%token <location> DO
%token <location> WHILE
%token <location> BREAK
%token <location> CONTINUE
%token <location> RETURN

// Operators
%token <location> INC
%token <location> DEC
%token <location> SHL
%token <location> SHR
%token <location> LOGICAL_AND
%token <location> LOGICAL_OR
%token <location> EQUAL
%token <location> NOT_EQUAL
%token <location> GREATER_EQUAL
%token <location> LESS_EQUAL

// Values
%token <token> INTEGER
%token <token> FLOAT
%token <token> CHAR
%token <token> BOOL
%token <token> IDENTIFIER

// =====================================================================================================
// Non-terminal Symbols Types
// ==========================

%type <blockNode>           program stmt_block
%type <StmtNode>            stmt branch_body for_init_stmt
%type <stmtList>            stmt_list case_body
%type <varDeclNode>         var_decl var_decl_uninit var_decl_init
%type <ifNode>              if_stmt unmatched_if_stmt matched_if_stmt
%type <switchNode>          switch_stmt
%type <caseStmtNode>        case_stmt
%type <caseList>            switch_body case_stmt_block case_stmt_list
%type <whileNode>           while_stmt
%type <doWhileNode>         do_while_stmt
%type <forNode>             for_stmt for_header
%type <functionNode>        function function_header
%type <functionCallNode>    function_call
%type <returnStmtNode>      return_stmt
%type <varList>             param_list param_list_ext
%type <exprList>            arg_list arg_list_ext
%type <exprNode>            expression for_expr
%type <typeNode>            type
%type <valueNode>           value
%type <identifierNode>      ident

// =====================================================================================================
// Precendence & Associativity
// ===========================

// Note that order matters here
%right      '='
%left       LOGICAL_OR
%left       LOGICAL_AND
%left       '|'
%left       '^'
%left       '&'
%left       EQUAL NOT_EQUAL
%left       LESS_EQUAL GREATER_EQUAL '<' '>'
%left       SHR SHL
%left       '-' '+'
%left       '*' '/' '%'
%right      '!' '~'
%right      U_PLUS U_MINUM
%right      PRE_INC PRE_DEC
%left       SUF_INC SUF_DEC

%nonassoc   IF_UNMAT
%nonassoc   ELSE

%%

// =====================================================================================================
// Rules Section
// =============

program:            /* epsilon */           { programRoot = new BlockNode(); }
    |               stmt_list               { programRoot = new BlockNode(*$1); delete $1; }
    ;

stmt_list:          stmt                    { $$ = new StmtList(); $$->push_back($1); }
    |               stmt_block              { $$ = new StmtList(); $$->push_back($1); }
    |               stmt_list stmt          { $$ = $1; $$->push_back($2); }
    |               stmt_list stmt_block    { $$ = $1; $$->push_back($2); }
    ;

stmt_block:         '{' '}'                 { $$ = new BlockNode(); }
    |               '{' stmt_list '}'       { $$ = new BlockNode(*$2); delete $2; }
    ;

stmt:               ';'                     { $$ = new StatementNode(); }
    |               BREAK ';'               { $$ = new BreakStmtNode(); }
    |               CONTINUE ';'            { $$ = new ContinueStmtNode(); }
    |               var_decl ';'            { $$ = $1; }
    |               expression ';'          { $$ = $1; }
    |               if_stmt                 { $$ = $1; }
    |               switch_stmt             { $$ = $1; }
    |               while_stmt              { $$ = $1; }
    |               do_while_stmt ';'       { $$ = $1; }
    |               for_stmt                { $$ = $1; }
    |               function                { $$ = $1; }
    |               return_stmt ';'         { $$ = $1; }
    |               error ';'               { $$ = new ErrorNode(curLineNum, curCursorPos, curTokenLen, "invalid syntax"); }
    ;

branch_body:        stmt                    { $$ = $1; }
    |               stmt_block              { $$ = $1; }
    ;

// ------------------------------------------------------------
//
// Declaration Rules
//

var_decl:           var_decl_uninit
    |               var_decl_init
    ;

var_decl_uninit:    type ident                              { $$ = new VarDeclarationNode($1, $2); }
    |               CONST type ident                        { $$ = new VarDeclarationNode($2, $3, NULL, true); }
    ;

var_decl_init:      type ident '=' expression               { $$ = new VarDeclarationNode($1, $2, $4); }
    |               CONST type ident '=' expression         { $$ = new VarDeclarationNode($2, $3, $5, true); }
    ;

// ------------------------------------------------------------
//
// Expression Rules
//

expression:         ident '=' expression                    { $$ = new AssignOprNode($1, $3); }

    |               expression '+' expression               { $$ = new BinaryOprNode(OPR_ADD, $1, $3); }
    |               expression '-' expression               { $$ = new BinaryOprNode(OPR_SUB, $1, $3); }
    |               expression '*' expression               { $$ = new BinaryOprNode(OPR_MUL, $1, $3); }
    |               expression '/' expression               { $$ = new BinaryOprNode(OPR_DIV, $1, $3); }
    |               expression '%' expression               { $$ = new BinaryOprNode(OPR_MOD, $1, $3); }
    |               expression '&' expression               { $$ = new BinaryOprNode(OPR_AND, $1, $3); }
    |               expression '|' expression               { $$ = new BinaryOprNode(OPR_OR, $1, $3); }
    |               expression '^' expression               { $$ = new BinaryOprNode(OPR_XOR, $1, $3); }
    |               expression SHL expression               { $$ = new BinaryOprNode(OPR_SHL, $1, $3); }
    |               expression SHR expression               { $$ = new BinaryOprNode(OPR_SHR, $1, $3); }
    |               expression LOGICAL_AND expression       { $$ = new BinaryOprNode(OPR_LOGICAL_AND, $1, $3); }
    |               expression LOGICAL_OR expression        { $$ = new BinaryOprNode(OPR_LOGICAL_OR, $1, $3); }
    |               expression '>' expression               { $$ = new BinaryOprNode(OPR_GREATER, $1, $3); }
    |               expression GREATER_EQUAL expression     { $$ = new BinaryOprNode(OPR_GREATER_EQUAL, $1, $3); }
    |               expression '<' expression               { $$ = new BinaryOprNode(OPR_LESS, $1, $3); }
    |               expression LESS_EQUAL expression        { $$ = new BinaryOprNode(OPR_LESS_EQUAL, $1, $3); }
    |               expression EQUAL expression             { $$ = new BinaryOprNode(OPR_EQUAL, $1, $3); }
    |               expression NOT_EQUAL expression         { $$ = new BinaryOprNode(OPR_NOT_EQUAL, $1, $3); }

    |               INC expression %prec PRE_INC            { $$ = new UnaryOprNode(OPR_PRE_INC, $2); }
    |               DEC expression %prec PRE_DEC            { $$ = new UnaryOprNode(OPR_PRE_DEC, $2); }
    
//  TODO - fix postfix inc/dec
//  |               expression INC %prec SUF_INC            { $$ = new UnaryOprNode(OPR_SUF_INC, $2); }
//  |               expression DEC %prec SUF_DEC            { $$ = new UnaryOprNode(OPR_SUF_DEC, $2); }

    |               '+' expression %prec U_PLUS             { $$ = new UnaryOprNode(OPR_U_PLUS, $2); }
    |               '-' expression %prec U_MINUM            { $$ = new UnaryOprNode(OPR_U_MINUS, $2); }
    |               '~' expression                          { $$ = new UnaryOprNode(OPR_NOT, $2); }
    |               '!' expression                          { $$ = new UnaryOprNode(OPR_LOGICAL_NOT, $2); }

    |               '(' expression ')'                      { $$ = $2; }

    |               value                                   { $$ = $1; }
    |               ident                                   { $$ = $1; }
    |               function_call                           { $$ = $1; }
    ;

// ------------------------------------------------------------
//
// If Rules
//

if_stmt:            unmatched_if_stmt
    |               matched_if_stmt
    ;

unmatched_if_stmt:  IF '(' expression ')' branch_body %prec IF_UNMAT    { $$ = new IfNode($3, $5); }
    ;

matched_if_stmt:    IF '(' expression ')' branch_body ELSE branch_body  { $$ = new IfNode($3, $5, $7); }
    ;

// ------------------------------------------------------------
//
// Switch Rules
//

switch_stmt:        SWITCH '(' expression ')' switch_body   { $$ = new SwitchNode($3, *$5); delete $5; }
    ;

switch_body:        case_stmt_block                         { $$ = $1; }
    ;

case_stmt_block:    '{' '}'                                 { $$ = new CaseList(); }
    |               '{' case_stmt_list '}'                  { $$ = $2; }
    ;

case_stmt_list:     case_stmt                               { $$ = new CaseList(); $$->push_back($1); }
    |               case_stmt_list case_stmt                { $$ = $1; $$->push_back($2); }
    ;

case_stmt:          CASE expression ':' case_body           { $$ = new CaseStmtNode($2, *$4); delete $4; }
    |               DEFAULT ':' case_body                   { $$ = new CaseStmtNode(NULL, *$3, true); delete $3; }
    ;

case_body:          /* epsilon */                           { $$ = new StmtList(); }
    |               stmt_list                               { $$ = $1; }
    ;

// ------------------------------------------------------------
//
// While Rules
//

while_stmt:         WHILE '(' expression ')' branch_body                { $$ = new WhileNode($3, $5); }
    ;

do_while_stmt:      DO branch_body WHILE '(' expression ')'             { $$ = new DoWhileNode($5, $2); }
    ;

// ------------------------------------------------------------
//
// For Rules
//

for_stmt:           for_header branch_body                              { $$ = $1; ((ForNode*) $$)->body = $2; }
    ;

for_header:         FOR '(' for_init_stmt ';' for_expr ';' for_expr ')' { $$ = new ForNode($3, $5, $7, NULL); }
    ;

for_init_stmt:      /* epsilon */                                       { $$ = NULL; }
    |               var_decl                                            { $$ = $1; }
    |               expression                                          { $$ = $1; }
    ;

for_expr:           /* epsilon */                                       { $$ = NULL; }
    |               expression                                          { $$ = $1; }
    ;

// ------------------------------------------------------------
//
// Function Rules
//

function:           function_header stmt_block          { $$ = $1; $$->body = $2; }
    ;

function_header:    type ident '(' param_list ')'       { $$ = new FunctionNode($1, $2, *$4, NULL); delete $4; }
    ;

param_list:         /* epsilon */                       { $$ = new VarList(); }
    |               var_decl                            { $$ = new VarList(); $$->push_back($1); }
    |               param_list_ext ',' var_decl         { $$ = $1; $$->push_back($3); }
    ;

param_list_ext:     var_decl                            { $$ = new VarList(); $$->push_back($1); }
    |               param_list_ext ',' var_decl         { $$ = $1; $$->push_back($3); }
    ;

function_call:      ident '(' arg_list ')'              { $$ = new FunctionCallNode($1, *$3); delete $3; }
    ;

arg_list:           /* epsilon */                       { $$ = new ExprList(); }
    |               expression                          { $$ = new ExprList(); $$->push_back($1); }
    |               arg_list_ext ',' expression         { $$ = $1; $$->push_back($3); }
    ;

arg_list_ext:       expression                          { $$ = new ExprList(); $$->push_back($1); }
    |               arg_list_ext ',' expression         { $$ = $1; $$->push_back($3); }
    ;

return_stmt:        RETURN expression                   { $$ = new ReturnStmtNode($2); }
    |               RETURN                              { $$ = new ReturnStmtNode(NULL); }
    ;

// ------------------------------------------------------------
//
// Other Rules
//

type:               TYPE_INT        { $$ = new TypeNode(DTYPE_INT, $1); }
    |               TYPE_FLOAT      { $$ = new TypeNode(DTYPE_FLOAT, $1); }
    |               TYPE_CHAR       { $$ = new TypeNode(DTYPE_CHAR, $1); }
    |               TYPE_BOOL       { $$ = new TypeNode(DTYPE_BOOL, $1); }
    |               TYPE_VOID       { $$ = new TypeNode(DTYPE_VOID, $1); }
    ;

value:              INTEGER         { $$ = new ValueNode(DTYPE_INT, $1.value, $1.loc); delete $1.value; }
    |               FLOAT           { $$ = new ValueNode(DTYPE_FLOAT, $1.value, $1.loc); delete $1.value; }
    |               CHAR            { $$ = new ValueNode(DTYPE_CHAR, $1.value, $1.loc); delete $1.value; }
    |               BOOL            { $$ = new ValueNode(DTYPE_BOOL, $1.value, $1.loc); delete $1.value; }
    ;

ident:              IDENTIFIER      { $$ = new IdentifierNode($1.value, $1.loc); delete $1.value; }
    ;

%%

// =====================================================================================================
// User Subroutines Section
// ========================

void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s); 
    // printf("%s\n", sourceCode[curLineNum - 1].c_str());
    // printf("%*s\n", curCursorPos, "^");
}