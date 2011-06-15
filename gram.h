#ifndef BISON_GRAM_H
# define BISON_GRAM_H

#ifndef YYSTYPE
typedef union {
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
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	AND	257
# define	OR	258
# define	XOR	259
# define	IMP	260
# define	EQV	261
# define	NOT	262
# define	OP_REL	263
# define	OP_ASSIGN	264
# define	OP_ADD_SUB	265
# define	OP_MUL_DIV	266
# define	REAL	267
# define	INT	268
# define	BOOL	269
# define	COUNT	270
# define	UNDEF	271
# define	CONSTFUNC	272
# define	WITHOUT_PARAM_FUNC_TIME	273
# define	STVAR_NAME	274
# define	WITHOUT_PARAM_FUNC_RANDOM	275
# define	PORTREF	276
# define	SEND	277
# define	CELLPOS	278
# define	BINARY_FUNC	279
# define	BINARY_FUNC_RANDOM	280
# define	COND3_FUNC	281
# define	COND4_FUNC	282
# define	STATECOUNT	283
# define	UNARY_FUNC	284
# define	COND_REAL_FUNC	285
# define	UNARY_FUNC_RANDOM	286
# define	PORTNAME	287


extern YYSTYPE yylval;

#endif /* not BISON_GRAM_H */
