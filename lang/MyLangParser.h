/** \file
 *  This C header file was generated by $ANTLR version 3.5.2
 *
 *     -  From the grammar source file : MyLang.g
 *     -                            On : 2024-12-24 07:50:54
 *     -                for the parser : MyLangParserParser
 *
 * Editing it, at least manually, is not wise.
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The parser 
MyLangParser

has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 *
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pMyLangParser, which is returned from a call to MyLangParserNew().
 *
 * The methods in pMyLangParser are  as follows:
 *
 *  - 
 MyLangParser_source_return
      pMyLangParser->source(pMyLangParser)
 *  - 
 MyLangParser_sourceItem_return
      pMyLangParser->sourceItem(pMyLangParser)
 *  - 
 MyLangParser_funcSignature_return
      pMyLangParser->funcSignature(pMyLangParser)
 *  - 
 MyLangParser_argList_return
      pMyLangParser->argList(pMyLangParser)
 *  - 
 MyLangParser_arg_return
      pMyLangParser->arg(pMyLangParser)
 *  - 
 MyLangParser_statement_return
      pMyLangParser->statement(pMyLangParser)
 *  - 
 MyLangParser_body_return
      pMyLangParser->body(pMyLangParser)
 *  - 
 MyLangParser_typeRef_return
      pMyLangParser->typeRef(pMyLangParser)
 *  - 
 MyLangParser_dec_return
      pMyLangParser->dec(pMyLangParser)
 *  - 
 MyLangParser_arrayList_return
      pMyLangParser->arrayList(pMyLangParser)
 *  - 
 MyLangParser_arrayType_return
      pMyLangParser->arrayType(pMyLangParser)
 *  - 
 MyLangParser_assignmentStatement_return
      pMyLangParser->assignmentStatement(pMyLangParser)
 *  - 
 MyLangParser_callStatement_return
      pMyLangParser->callStatement(pMyLangParser)
 *  - 
 MyLangParser_condStatement_return
      pMyLangParser->condStatement(pMyLangParser)
 *  - 
 MyLangParser_loopStatement_return
      pMyLangParser->loopStatement(pMyLangParser)
 *  - 
 MyLangParser_returnStatement_return
      pMyLangParser->returnStatement(pMyLangParser)
 *  - 
 MyLangParser_repeatStatement_return
      pMyLangParser->repeatStatement(pMyLangParser)
 *  - 
 MyLangParser_breakStatement_return
      pMyLangParser->breakStatement(pMyLangParser)
 *  - 
 MyLangParser_expressionStatement_return
      pMyLangParser->expressionStatement(pMyLangParser)
 *  - 
 MyLangParser_expression_return
      pMyLangParser->expression(pMyLangParser)
 *  - 
 MyLangParser_slice_return
      pMyLangParser->slice(pMyLangParser)
 *  - 
 MyLangParser_ranges_return
      pMyLangParser->ranges(pMyLangParser)
 *  - 
 MyLangParser_assignmentExpression_return
      pMyLangParser->assignmentExpression(pMyLangParser)
 *  - 
 MyLangParser_logicalOrExpression_return
      pMyLangParser->logicalOrExpression(pMyLangParser)
 *  - 
 MyLangParser_logicalAndExpression_return
      pMyLangParser->logicalAndExpression(pMyLangParser)
 *  - 
 MyLangParser_bitwiseOrExpression_return
      pMyLangParser->bitwiseOrExpression(pMyLangParser)
 *  - 
 MyLangParser_bitwiseXorExpression_return
      pMyLangParser->bitwiseXorExpression(pMyLangParser)
 *  - 
 MyLangParser_bitwiseAndExpression_return
      pMyLangParser->bitwiseAndExpression(pMyLangParser)
 *  - 
 MyLangParser_equalityExpression_return
      pMyLangParser->equalityExpression(pMyLangParser)
 *  - 
 MyLangParser_relationalExpression_return
      pMyLangParser->relationalExpression(pMyLangParser)
 *  - 
 MyLangParser_shiftExpression_return
      pMyLangParser->shiftExpression(pMyLangParser)
 *  - 
 MyLangParser_additiveExpression_return
      pMyLangParser->additiveExpression(pMyLangParser)
 *  - 
 MyLangParser_multiplicativeExpression_return
      pMyLangParser->multiplicativeExpression(pMyLangParser)
 *  - 
 MyLangParser_unaryExpression_return
      pMyLangParser->unaryExpression(pMyLangParser)
 *  - 
 MyLangParser_callExpression_return
      pMyLangParser->callExpression(pMyLangParser)
 *  - 
 MyLangParser_primaryExpression_return
      pMyLangParser->primaryExpression(pMyLangParser)
 *  - 
 MyLangParser_expressionList_return
      pMyLangParser->expressionList(pMyLangParser)
 *  - 
 MyLangParser_varDeclaration_return
      pMyLangParser->varDeclaration(pMyLangParser)
 *  - 
 MyLangParser_builtin_return
      pMyLangParser->builtin(pMyLangParser)
 *  - 
 MyLangParser_identifier_return
      pMyLangParser->identifier(pMyLangParser)
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD license"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_MyLangParser_H
#define _MyLangParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct MyLangParser_Ctx_struct MyLangParser, * pMyLangParser;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */

typedef struct MyLangParser_source_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_source_return;



typedef struct MyLangParser_sourceItem_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_sourceItem_return;



typedef struct MyLangParser_funcSignature_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_funcSignature_return;



typedef struct MyLangParser_argList_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_argList_return;



typedef struct MyLangParser_arg_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_arg_return;



typedef struct MyLangParser_statement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_statement_return;



typedef struct MyLangParser_body_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_body_return;



typedef struct MyLangParser_typeRef_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_typeRef_return;



typedef struct MyLangParser_dec_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_dec_return;



typedef struct MyLangParser_arrayList_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_arrayList_return;



typedef struct MyLangParser_arrayType_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_arrayType_return;



typedef struct MyLangParser_assignmentStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_assignmentStatement_return;



typedef struct MyLangParser_callStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_callStatement_return;



typedef struct MyLangParser_condStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_condStatement_return;



typedef struct MyLangParser_loopStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_loopStatement_return;



typedef struct MyLangParser_returnStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_returnStatement_return;



typedef struct MyLangParser_repeatStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_repeatStatement_return;



typedef struct MyLangParser_breakStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_breakStatement_return;



typedef struct MyLangParser_expressionStatement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_expressionStatement_return;



typedef struct MyLangParser_expression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_expression_return;



typedef struct MyLangParser_slice_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_slice_return;



typedef struct MyLangParser_ranges_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_ranges_return;



typedef struct MyLangParser_assignmentExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_assignmentExpression_return;



typedef struct MyLangParser_logicalOrExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_logicalOrExpression_return;



typedef struct MyLangParser_logicalAndExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_logicalAndExpression_return;



typedef struct MyLangParser_bitwiseOrExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_bitwiseOrExpression_return;



typedef struct MyLangParser_bitwiseXorExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_bitwiseXorExpression_return;



typedef struct MyLangParser_bitwiseAndExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_bitwiseAndExpression_return;



typedef struct MyLangParser_equalityExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_equalityExpression_return;



typedef struct MyLangParser_relationalExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_relationalExpression_return;



typedef struct MyLangParser_shiftExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_shiftExpression_return;



typedef struct MyLangParser_additiveExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_additiveExpression_return;



typedef struct MyLangParser_multiplicativeExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_multiplicativeExpression_return;



typedef struct MyLangParser_unaryExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_unaryExpression_return;



typedef struct MyLangParser_callExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_callExpression_return;



typedef struct MyLangParser_primaryExpression_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_primaryExpression_return;



typedef struct MyLangParser_expressionList_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_expressionList_return;



typedef struct MyLangParser_varDeclaration_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_varDeclaration_return;



typedef struct MyLangParser_builtin_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_builtin_return;



typedef struct MyLangParser_identifier_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;

}
    MyLangParser_identifier_return;




/** Context tracking structure for 
MyLangParser

 */
struct MyLangParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;

     MyLangParser_source_return
     (*source)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_sourceItem_return
     (*sourceItem)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_funcSignature_return
     (*funcSignature)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_argList_return
     (*argList)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_arg_return
     (*arg)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_statement_return
     (*statement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_body_return
     (*body)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_typeRef_return
     (*typeRef)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_dec_return
     (*dec)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_arrayList_return
     (*arrayList)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_arrayType_return
     (*arrayType)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_assignmentStatement_return
     (*assignmentStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_callStatement_return
     (*callStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_condStatement_return
     (*condStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_loopStatement_return
     (*loopStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_returnStatement_return
     (*returnStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_repeatStatement_return
     (*repeatStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_breakStatement_return
     (*breakStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_expressionStatement_return
     (*expressionStatement)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_expression_return
     (*expression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_slice_return
     (*slice)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_ranges_return
     (*ranges)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_assignmentExpression_return
     (*assignmentExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_logicalOrExpression_return
     (*logicalOrExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_logicalAndExpression_return
     (*logicalAndExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_bitwiseOrExpression_return
     (*bitwiseOrExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_bitwiseXorExpression_return
     (*bitwiseXorExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_bitwiseAndExpression_return
     (*bitwiseAndExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_equalityExpression_return
     (*equalityExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_relationalExpression_return
     (*relationalExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_shiftExpression_return
     (*shiftExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_additiveExpression_return
     (*additiveExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_multiplicativeExpression_return
     (*multiplicativeExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_unaryExpression_return
     (*unaryExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_callExpression_return
     (*callExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_primaryExpression_return
     (*primaryExpression)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_expressionList_return
     (*expressionList)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_varDeclaration_return
     (*varDeclaration)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_builtin_return
     (*builtin)	(struct MyLangParser_Ctx_struct * ctx);

     MyLangParser_identifier_return
     (*identifier)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred6_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred9_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred13_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred14_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred16_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred17_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred18_MyLang)	(struct MyLangParser_Ctx_struct * ctx);

     ANTLR3_BOOLEAN
     (*synpred19_MyLang)	(struct MyLangParser_Ctx_struct * ctx);
    // Delegated rules

    const char * (*getGrammarFileName)();
    void            (*reset)  (struct MyLangParser_Ctx_struct * ctx);
    void	    (*free)   (struct MyLangParser_Ctx_struct * ctx);
/* @headerFile.members() */
pANTLR3_BASE_TREE_ADAPTOR	adaptor;
pANTLR3_VECTOR_FACTORY		vectors;
/* End @headerFile.members() */
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pMyLangParser MyLangParserNew         (
pANTLR3_COMMON_TOKEN_STREAM
 instream);
ANTLR3_API pMyLangParser MyLangParserNewSSD      (
pANTLR3_COMMON_TOKEN_STREAM
 instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the 
parser
 will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif
#define EOF      -1
#define T__37      37
#define T__38      38
#define T__39      39
#define T__40      40
#define T__41      41
#define T__42      42
#define T__43      43
#define T__44      44
#define T__45      45
#define T__46      46
#define T__47      47
#define T__48      48
#define T__49      49
#define T__50      50
#define T__51      51
#define T__52      52
#define T__53      53
#define T__54      54
#define T__55      55
#define T__56      56
#define T__57      57
#define T__58      58
#define T__59      59
#define T__60      60
#define T__61      61
#define T__62      62
#define T__63      63
#define T__64      64
#define T__65      65
#define T__66      66
#define T__67      67
#define T__68      68
#define T__69      69
#define T__70      70
#define T__71      71
#define T__72      72
#define T__73      73
#define T__74      74
#define T__75      75
#define T__76      76
#define ArgListToken      4
#define ArgToken      5
#define ArrayList      6
#define ArrayToken      7
#define ArrayTokenSuffix      8
#define AssignmentOp      9
#define Bits      10
#define Body      11
#define Bool      12
#define BreakToken      13
#define BuiltIn      14
#define CallToken      15
#define Char      16
#define CondToken      17
#define Dec      18
#define ExpressionToken      19
#define FuncDefToken      20
#define FuncSignatureToken      21
#define Hex      22
#define Identifier      23
#define Literal      24
#define LoopToken      25
#define RangeToken      26
#define RelationalOp      27
#define RepeatToken      28
#define ReturnToken      29
#define SliceToken      30
#define SourceToken      31
#define Str      32
#define TypeRefToken      33
#define UnaryOp      34
#define VarDeclToken      35
#define WS      36
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for MyLangParser
 * =============================================================================
 */
/** } */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
