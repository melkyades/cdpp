
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EQV = 258,
     IMP = 259,
     XOR = 260,
     OR = 261,
     AND = 262,
     NOT = 263,
     OP_REL = 264,
     OP_ASSIGN = 265,
     OP_ADD_SUB = 266,
     OP_MUL_DIV = 267,
     REAL = 268,
     INT = 269,
     BOOL = 270,
     COUNT = 271,
     UNDEF = 272,
     CONSTFUNC = 273,
     WITHOUT_PARAM_FUNC_TIME = 274,
     STVAR_NAME = 275,
     WITHOUT_PARAM_FUNC_RANDOM = 276,
     PORTREF = 277,
     SEND = 278,
     CELLPOS = 279,
     BINARY_FUNC = 280,
     BINARY_FUNC_RANDOM = 281,
     COND3_FUNC = 282,
     COND4_FUNC = 283,
     STATECOUNT = 284,
     UNARY_FUNC = 285,
     COND_REAL_FUNC = 286,
     UNARY_FUNC_RANDOM = 287,
     PORTNAME = 288
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 34 "gram.y"

	struct {
		SyntaxNode *Node ;
		int Stochastic;
	} PSyntaxNode;

  	struct {
		ANDNode *Node;
		int Stochastic;
	} PBinaryNode;

	struct {
		NOTNode *Node;
		int Stochastic;
	} PUnaryNode ;

  	struct {
		IFNode *Node;
		int Stochastic;
	} P3Node ;

  	struct {
		IFUNode *Node;
		int Stochastic;
	} P4Node ;

	struct {
		AssignNode *Node;
		int Stochastic;
	} PAssignNode;
	
	struct {
		ListNode *Node;
		int Stochastic;
	} PListNode;

	struct {
		nTupla	*Node;
	} PTupla ;



/* Line 1676 of yacc.c  */
#line 128 "gram.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


