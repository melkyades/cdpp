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
		nTupla	*Node;
	} PTupla ;
} YYSTYPE;
#define	AND	258
#define	OR	259
#define	XOR	260
#define	IMP	261
#define	EQV	262
#define	NOT	263
#define	OP_REL	264
#define	OP_ADD_SUB	265
#define	OP_MUL_DIV	266
#define	REAL	267
#define	INT	268
#define	BOOL	269
#define	COUNT	270
#define	UNDEF	271
#define	CONSTFUNC	272
#define	WITHOUT_PARAM_FUNC_TIME	273
#define	WITHOUT_PARAM_FUNC_RANDOM	274
#define	PORTREF	275
#define	SEND	276
#define	CELLPOS	277
#define	BINARY_FUNC	278
#define	BINARY_FUNC_RANDOM	279
#define	COND3_FUNC	280
#define	COND4_FUNC	281
#define	STATECOUNT	282
#define	UNARY_FUNC	283
#define	COND_REAL_FUNC	284
#define	UNARY_FUNC_RANDOM	285
#define	PORTNAME	286


extern YYSTYPE yylval;
