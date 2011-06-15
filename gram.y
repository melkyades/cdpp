/*******************************************************************
*
*  DESCRIPTION: Gramatica usada por GadCelLa
*
*  AUTHOR:    Amir Barylko & Jorge Boyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 22/05/1999 (v2)
*
*******************************************************************/

%{

#define	FALSE	0
#define	TRUE	(!FALSE)

#include "synnode.h"	// PSyntaxNode
#include "parser.h"	// SingleParser
#include "ntupla.h"	// nTupla


int yyparse() ;
int yylex() ;

int yyerror( const char *s ) ;

%}

%union {
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
} ; 


%left  <PBinaryNode> AND OR XOR IMP EQV
%right <PUnaryNode>  NOT
%token <PBinaryNode> OP_REL
%right <PAssignNode> OP_ASSIGN
%left  <PBinaryNode> OP_ADD_SUB
%left  <PBinaryNode> OP_MUL_DIV
%token <PSyntaxNode> REAL INT BOOL COUNT UNDEF CONSTFUNC WITHOUT_PARAM_FUNC_TIME STVAR_NAME
%token <PUnaryNode>  WITHOUT_PARAM_FUNC_RANDOM PORTREF SEND CELLPOS
%token <PBinaryNode> BINARY_FUNC BINARY_FUNC_RANDOM
%token <P3Node>      COND3_FUNC
%token <P4Node>      COND4_FUNC
%token <PUnaryNode>  STATECOUNT UNARY_FUNC COND_REAL_FUNC UNARY_FUNC_RANDOM PORTNAME
%token '(' ')'
%token ';'


%type <PTupla>         Tupla Resto_nTupla
%type <PListNode>      AssignSet PortSendSet AssignResult
%type <PAssignNode>    Assign
%type <PSyntaxNode>    PortSend OptPortName OptParamPort
%type <PSyntaxNode>    RealRelExp RealExp IdRef Constant Function CellRef
%type <PSyntaxNode>    Resultado
%type <PSyntaxNode>    BoolExp 

%%


Language : 
       RuleList                               
   ;


RuleList :
     Rule
   | Rule RuleList 
   ;


Rule : 
   AssignResult Resultado '{' BoolExp '}' 
   					{ SingleParser::Instance().addRule( new RuleNode( $1.Node, NULL, $2.Node, $4.Node ),
					                                    $4.Stochastic); }
   | AssignResult '{' AssignSet '}' Resultado '{' BoolExp '}' 
   					{ SingleParser::Instance().addRule( new RuleNode( $1.Node, $3.Node, $5.Node, $7.Node ),
					                                    $7.Stochastic); }
   ;
   
AssignResult :
   Resultado				{ SendNCPortNode *spn = new SendNCPortNode(NULL, $1.Node);
					  $$.Node = new ListNode();
   					  $$.Node->add(spn);
					  $$.Stochastic = $1.Stochastic; }
   | '{' PortSendSet '}'		{ $$.Node = $2.Node ;
					  $$.Stochastic = $2.Stochastic; }
   ;


Resultado :
   Constant				{ $$.Node = $1.Node;
					  $$.Stochastic = FALSE; }
   | UNDEF				{ $$.Node = $1.Node ;
					  $$.Stochastic = FALSE; }
   | '{' RealExp '}'			{ $$.Node = $2.Node ;
					  $$.Stochastic = $2.Stochastic; }
   ;


BoolExp :
   BOOL			         { $$.Node = $1.Node ;
					$$.Stochastic = FALSE; }
   | '(' BoolExp ')'		 { $$.Node = $2.Node ;
					$$.Stochastic = $2.Stochastic; }
   | RealRelExp			 { $$.Node = $1.Node ;
					$$.Stochastic = $1.Stochastic ; }
   | NOT BoolExp                 { $$.Node = $1.Node ;
					$1.Node->child( $2.Node ) ;
					$$.Stochastic = $2.Stochastic; }
   | BoolExp AND BoolExp         { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | BoolExp OR  BoolExp         { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | BoolExp XOR BoolExp         { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | BoolExp IMP BoolExp         { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | BoolExp EQV BoolExp         { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }

   ; 
   

RealRelExp : 
   RealExp OP_REL RealExp            { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | COND_REAL_FUNC '(' RealExp ')'  { $$.Node = $1.Node ;
					$1.Node->child( $3.Node ) ;
					$$.Stochastic = $3.Stochastic; }
   ;


RealExp : 
   IdRef                         { $$.Node = $1.Node ;
					$$.Stochastic = $1.Stochastic; }
   | '(' RealExp ')'             { $$.Node = $2.Node ;
					$$.Stochastic = $2.Stochastic; }
   | RealExp OP_ADD_SUB RealExp  { $$.Node = $2.Node ;
					$2.Node->left( $1.Node ) ;
					$2.Node->right( $3.Node ) ;
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   | RealExp OP_MUL_DIV RealExp  { $$.Node = $2.Node;
					$2.Node->left( $1.Node );
					$2.Node->right( $3.Node );
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   ;

IdRef : 
   CellRef OptPortName           { $$.Node = $1.Node;
   					((VarNode *) $$.Node)->port( ((StringNode *) $2.Node)->getString() );
					$$.Stochastic = FALSE; }
   | Constant                    { $$.Node = $1.Node;
					$$.Stochastic = FALSE; }
   | Function                    { $$.Node = $1.Node;
					$$.Stochastic = $1.Stochastic; }
   | UNDEF			 { $$.Node = $1.Node;
					$$.Stochastic = FALSE; }
   | PORTREF '(' PORTNAME ')'    { $$.Node = new PortRefNode( $3.Node ) ;
					$$.Stochastic = FALSE; }
   | SEND '(' PORTNAME ',' RealExp ')'
				 { $$.Node = new SendPortNode( $3.Node, $5.Node );
					$$.Stochastic = $5.Stochastic; }
   | CELLPOS '(' RealExp ')'     { $$.Node = new AbsCellPosNode( $3.Node );
					$$.Stochastic = FALSE; }
   | STVAR_NAME                  { $$.Node = $1.Node ;
					$$.Stochastic = FALSE; }
   ;

OptPortName :
   /* Empty */			 { $$.Node = new StringNode("");
   					$$.Stochastic = FALSE; }
   | PORTNAME			 { $$.Node = $1.Node;
   					$$.Stochastic = FALSE; }
   ;

AssignSet :
   /* Empty */			 { $$.Node = new ListNode();
   					$$.Stochastic = FALSE; }
   | Assign AssignSet            { $$.Node = $2.Node;
   					$$.Node->add($1.Node);
   					$$.Stochastic = $1.Stochastic | $2.Stochastic; }
   ;

Assign :
   STVAR_NAME OP_ASSIGN RealExp	';'
                                 { $$.Node = $2.Node ;
					$2.Node->var( $1.Node );
					$2.Node->val( $3.Node );
					$$.Stochastic = $1.Stochastic | $3.Stochastic; }
   ;

PortSendSet :
   /* Empty */			 { $$.Node = new ListNode();
   					$$.Stochastic = FALSE; }
   | PortSend PortSendSet        { $$.Node = $2.Node;
   					$$.Node->add($1.Node);
   					$$.Stochastic = $1.Stochastic | $2.Stochastic; }
   ;

PortSend :
   SEND '(' PORTNAME ',' RealExp ')' ';'
				 { $$.Node = new SendPortNode( $3.Node, $5.Node );
					$$.Stochastic = $5.Stochastic; }
   | PORTNAME OP_ASSIGN RealExp	';'
                                 { $$.Node = new SendNCPortNode( $1.Node, $3.Node );
					$$.Stochastic = $3.Stochastic; }
   ;


Constant :
   INT                        { $$.Node = $1.Node ;
				$$.Stochastic = FALSE; }
   | REAL		      { $$.Node = $1.Node ;
				$$.Stochastic = FALSE; }
   | CONSTFUNC                { $$.Node = $1.Node ;
				$$.Stochastic = FALSE; }
   ;
   

Function :
   COUNT					{ $$.Node = $1.Node ;
							$$.Stochastic = FALSE; }
   | STATECOUNT '(' RealExp OptParamPort ')'	{ $$.Node = new CountNode ( $3.Node, (StringNode *) $4.Node ) ;
							$$.Stochastic = $3.Stochastic; }
   | UNARY_FUNC '(' RealExp ')'			{ $$.Node = $1.Node ;
							$1.Node->child( $3.Node ) ;
							$$.Stochastic = $3.Stochastic; }
   | BINARY_FUNC '(' RealExp ',' RealExp ')'	{ $$.Node = $1.Node ;
							$1.Node->left( $3.Node );
							$1.Node->right( $5.Node );
							$$.Stochastic = $3.Stochastic | $5.Stochastic; }
   | WITHOUT_PARAM_FUNC_TIME			{ $$.Node = new TimeNode() ;
							$$.Stochastic = FALSE; }
   | WITHOUT_PARAM_FUNC_RANDOM			{ $$.Node = $1.Node ;
							$1.Node->child( new ConstantNode( Real(0), RealType::TheReal ) );
							$$.Stochastic = TRUE; }
   | UNARY_FUNC_RANDOM '(' RealExp ')'		{ $$.Node = $1.Node ;
							$1.Node->child( $3.Node ) ;
							$$.Stochastic = TRUE; }
   | BINARY_FUNC_RANDOM '(' RealExp ',' RealExp ')'
						{ $$.Node = $1.Node ;
							$1.Node->left( $3.Node );
							$1.Node->right( $5.Node );
							$$.Stochastic = TRUE; }
   | COND3_FUNC '(' BoolExp ',' RealExp ',' RealExp ')'
						{ $$.Node = $1.Node ;
							$1.Node->child1( $3.Node );
							$1.Node->child2( $5.Node );
							$1.Node->child3( $7.Node );
							$$.Stochastic = $3.Stochastic | $5.Stochastic | $7.Stochastic; }
   | COND4_FUNC '(' BoolExp ',' RealExp ',' RealExp ',' RealExp ')'
						{ $$.Node = $1.Node ;
							$1.Node->child1( $3.Node );
							$1.Node->child2( $5.Node );
							$1.Node->child3( $7.Node );
							$1.Node->child4( $9.Node );
							$$.Stochastic = $3.Stochastic | $5.Stochastic | $7.Stochastic | $9.Stochastic; }
   ;


OptParamPort :
   /* empty */				{ $$.Node = new StringNode("");
					  $$.Stochastic = FALSE; }
   | ',' PORTNAME			{ $$.Node = $2.Node;
					  $$.Stochastic = FALSE; }
   ;

CellRef :
   '(' Tupla				{ $$.Node = new VarNode( *($2.Node) );
					  $$.Stochastic = FALSE;
					  delete $2.Node;
					}
   ;

Tupla :
   INT ',' INT Resto_nTupla		{ $$.Node = $4.Node->addFirst( (int) $3.Node->evaluate().value() )->addFirst( (int) $1.Node->evaluate().value() ); }
   ;

Resto_nTupla:
   ',' INT Resto_nTupla			{ $$.Node = $3.Node->addFirst( (int) $2.Node->evaluate().value() ); }
   | ')'				{ $$.Node = new nTupla; }
   ;

// NOTAS:
// Los siguientes tipos (tokens) se parsean en PARSER.CPP:
//
// BOOL es alguno de los siguientes:  t | f | ?
// UNDEF es:  ?
// INT es de la forma:  [Sign] Digit { Digit }
// REAL es de la forma:  INT  |  [Sign] {Digit}. Digit {Digit}
// OP_REL es de la forma:  = | != | > | < | >= | <=
// OP_ASSIGN es de la forma: :=
// OP_ADD_SUB es de la forma:  + | -
// OP_MUL_DIV es de la forma:  * | /
// COUNT es de la forma:  truecount | falsecount | undefcount
// STATECOUNT es de la forma:  statecount
// AND es de la forma:  and
// OR es de la forma:  or
// NOT es de la forma:  not
// Sign es de la forma:  +  |  -
// Digit es de la forma:  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
// CONSTFUNC es de la forma:  pi | e | inf
// WITHOUT_PARAM_FUNC, UNARY_FUNC, BINARY_FUNC y COND_REAL_FUNC son distintos tipos de funciones
// STVAR_NAME es una cadena de caracteres sin los corchetes delimitadores.
 
%%
