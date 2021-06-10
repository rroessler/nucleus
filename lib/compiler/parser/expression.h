#ifndef NUC_EXPRESSION_H
#define NUC_EXPRESSION_H

/****************************
 *  EXPRESSION PARSING API  *
 ****************************/

// allows use of the expression macro below
#define DEF_EXPR static void fuser_parsePrecedence(Precedence prec)

/** Parses a Nucleus Expression. */
#define EXPRESSION fuser_parsePrecedence(P_ASSIGNMENT);

#endif