#ifndef __UTILS_H_
#define __UTILS_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "consts.h"

using namespace std;


/**
 * Union of different primitive data types.
 */
union Value {
    bool boolVal;
    char charVal;
    int intVal;
    float floatVal;
};

/**
 * Struct holding the location information of the tokens.
 */
struct Location {
    int lineNum;
    int pos;
    int len;
};

/**
 * Struct holding the basic information of the tokens.
 */
struct Token {
    char* value;
    Location loc;
};

/**
 * Collection of utility functions to be used across the entire project modules.
 *
 * Note that all methods in this class must be static methods.
 */
struct Utils {

    /**
     * Checks whether the given operator is an arithmetic operator or not.
     *
     * @param opr the operator to check.
     *
     * @return {@code true} if the given operator is arithmetic; {@code false} otherwise.
     */
    static bool isArithmeticOpr(Operator opr) {
        switch (opr) {
            case OPR_ADD:
            case OPR_U_PLUS:
            case OPR_SUB:
            case OPR_U_MINUS:
            case OPR_MUL:
            case OPR_DIV:
            case OPR_MOD:
                return true;
        }
        return false;
    }

    /**
     * Checks whether the given operator is a logical operator or not.
     *
     * @param opr the operator to check.
     *
     * @return {@code true} if the given operator is logical; {@code false} otherwise.
     */
    static bool isLogicalOpr(Operator opr) {
        switch (opr) {
            case OPR_LOGICAL_AND:
            case OPR_LOGICAL_OR:
            case OPR_LOGICAL_NOT:
            case OPR_GREATER:
            case OPR_GREATER_EQUAL:
            case OPR_LESS:
            case OPR_LESS_EQUAL:
            case OPR_EQUAL:
            case OPR_NOT_EQUAL:
                return true;
        }
        return false;
    }

    /**
     * Checks whether the given operator is a bitwise operator or not.
     *
     * @param opr the operator to check.
     *
     * @return {@code true} if the given operator is bitwise; {@code false} otherwise.
     */
    static bool isBitwiseOpr(Operator opr) {
        switch (opr) {
            case OPR_AND:
            case OPR_OR:
            case OPR_XOR:
            case OPR_NOT:
            case OPR_SHL:
            case OPR_SHR:
                return true;
        }
        return false;
    }

    /**
     * Checks whether the given operator requires l-value operand or not.
     *
     * @param opr the operator to check.
     *
     * @return {@code true} if the given operator requires l-value operand; {@code false} otherwise.
     */
    static bool isLvalueOpr(Operator opr) {
        switch (opr) {
            case OPR_ASSIGN:
            case OPR_PRE_INC:
            case OPR_SUF_INC:
            case OPR_PRE_DEC:
            case OPR_SUF_DEC:
                return true;
        }
        return false;
    }

    /**
     * Converts the given operator into its corresponding token string.
     *
     * @param opr the operator to convert.
     *
     * @return the corresponding token string.
     */
    static string oprToStr(Operator opr) {
        switch (opr) {
            case OPR_ASSIGN:
                return "=";
            case OPR_ADD:
            case OPR_U_PLUS:
                return "+";
            case OPR_SUB:
            case OPR_U_MINUS:
                return "-";
            case OPR_MUL:
                return "*";
            case OPR_DIV:
                return "/";
            case OPR_MOD:
                return "%";
            case OPR_PRE_INC:
            case OPR_SUF_INC:
                return "++";
            case OPR_PRE_DEC:
            case OPR_SUF_DEC:
                return "--";
            case OPR_AND:
                return "&";
            case OPR_OR:
                return "|";
            case OPR_XOR:
                return "^";
            case OPR_NOT:
                return "~";
            case OPR_SHL:
                return "<<";
            case OPR_SHR:
                return ">>";
            case OPR_LOGICAL_AND:
                return "&&";
            case OPR_LOGICAL_OR:
                return "||";
            case OPR_LOGICAL_NOT:
                return "!";
            case OPR_GREATER:
                return ">";
            case OPR_GREATER_EQUAL:
                return ">=";
            case OPR_LESS:
                return "<";
            case OPR_LESS_EQUAL:
                return "<=";
            case OPR_EQUAL:
                return "==";
            case OPR_NOT_EQUAL:
                return "!=";
        }

        return "#";
    }

    /**
     * Converts the given operator into its corresponding quadruple string.
     *
     * @param opr the operator to convert.
     * @param type the type of the resulting operation.
     *
     * @return the corresponding quadruple string.
     */
    static string binOprToQuad(Operator opr, DataType type) {
        // @OmarBazaraa: I think it is better to extend this function to handle
        // @OmarBazaraa: binary operations, unary operations, and push/pop instructions

        switch (opr) {
            case OPR_ADD:
                return "ADD_" + dtypeToQuad(type);
            case OPR_SUB:
                return "SUB_" + dtypeToQuad(type);
            case OPR_MUL:
                return "MUL_" + dtypeToQuad(type);
            case OPR_DIV:
                return "DIV_" + dtypeToQuad(type);
            case OPR_MOD:
                return "MOD"; // @OmarBazaraa: why not "MOD"? xD
            case OPR_AND:
                return "AND_" + dtypeToQuad(type);
            case OPR_OR:
                return "OR_" + dtypeToQuad(type);
            case OPR_XOR:
                return "XOR_" + dtypeToQuad(type);
            case OPR_NOT:
                return "NOT_" + dtypeToQuad(type);
            case OPR_SHL:
                return "SHL_" + dtypeToQuad(type);
            case OPR_SHR:
                return "SHR_" + dtypeToQuad(type);
            // @OmarBazaraa: there is no logical AND, OR, NOT in the processor :D
            // @OmarBazaraa: you can use the same AND, OR, NOT as above.
            case OPR_LOGICAL_AND:   
                return "LAND";
            case OPR_LOGICAL_OR:
                return "LOR";
            case OPR_LOGICAL_NOT:
                return "LNOT";
            case OPR_GREATER:
                return "GT";
            case OPR_GREATER_EQUAL:
                return "GTE";
            case OPR_LESS:
                return "LT";
            case OPR_LESS_EQUAL:
                return "LTE";
            case OPR_EQUAL:
                return "EQU";
            case OPR_NOT_EQUAL:
                return "NEQ";
        }

        return "#";
    }

    /**
     * Checks whether the given type is an integer data type.
     *
     * @param type the type to check.
     *
     * @return {@code true} if the given type is integer type; {@code false} otherwise.
     */
    static bool isIntegerType(DataType type) {
        return (type == DTYPE_BOOL || type == DTYPE_CHAR || type == DTYPE_INT);
    }

    /**
     * Converts the given data type into its corresponding token string.
     *
     * @param type the type to convert.
     *
     * @return the corresponding token string.
     */
    static string dtypeToStr(DataType type) {
        switch (type) {
            case DTYPE_VOID:
                return "void";
            case DTYPE_BOOL:
                return "bool";
            case DTYPE_CHAR:
                return "char";
            case DTYPE_INT:
                return "int";
            case DTYPE_FLOAT:
                return "float";
            case DTYPE_FUNC_PTR:
                return "pointer to function";
            case DTYPE_ERROR:
                return "type error";
        }

        return "unknown";
    }

    /**
     * Converts the given data type into its corresponding quadruple string.
     *
     * @param type the type to convert.
     *
     * @return the corresponding quadruple string.
     */
    static string dtypeToQuad(DataType type) {
        switch (type) {
            case DTYPE_VOID:
                return "VOID";
            case DTYPE_BOOL:
                return "BOOL";
            case DTYPE_CHAR:
                return "CHR";
            case DTYPE_INT:
                return "INT";
            case DTYPE_FLOAT:
                return "FLOAT";
            case DTYPE_FUNC_PTR:
                return "FNCPTR";
            case DTYPE_ERROR:
                return "TYPERR";
        }

        return "unknown";
    }

    /**
     * Convert data type from t1 into t2.
     *
     * @param t1 the type to convert from.
     * @param t2 the type to convert to.
     *
     * @return the corresponding quadruple string.
     */
    static string dtypeConvQuad(DataType t1, DataType t2) {
        return (t1 != t2 ? dtypeToQuad(t1) + "_TO_" + dtypeToQuad(t2) + "\n" : "");
    }
};

#endif