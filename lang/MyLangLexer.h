/** \file
 *  This C header file was generated by $ANTLR version 3.5.2
 *
 *     -  From the grammar source file : /home/spo/spo-hw/lang/MyLang.g
 *     -                            On : 2025-01-30 03:59:27
 *     -                 for the lexer : MyLangLexerLexer
 *
 * Editing it, at least manually, is not wise.
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The lexer 
MyLangLexer

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
 * a parser context typedef pMyLangLexer, which is returned from a call to MyLangLexerNew().
 *
 * As this is a generated lexer, it is unlikely you will call it 'manually'. However
 * the methods are provided anyway.
 *
 * The methods in pMyLangLexer are  as follows:
 *
 *  - 
 void
      pMyLangLexer->T__38(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__39(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__40(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__41(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__42(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__43(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__44(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__45(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__46(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__47(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__48(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__49(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__50(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__51(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__52(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__53(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__54(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__55(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__56(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__57(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__58(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__59(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__60(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__61(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__62(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__63(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__64(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__65(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__66(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__67(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__68(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__69(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__70(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__71(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__72(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__73(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__74(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__75(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->T__76(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Literal(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Bool(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Bits(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Hex(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Dec(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Char(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Str(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->BuiltIn(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Identifier(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->AssignmentOp(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->RelationalOp(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->UnaryOp(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->WS(pMyLangLexer)
 *  - 
 void
      pMyLangLexer->Tokens(pMyLangLexer)
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

#ifndef	_MyLangLexer_H
#define _MyLangLexer_H
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
typedef struct MyLangLexer_Ctx_struct MyLangLexer, * pMyLangLexer;



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

/** Context tracking structure for 
MyLangLexer

 */
struct MyLangLexer_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_LEXER    pLexer;

     void
     (*mT__38)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__39)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__40)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__41)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__42)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__43)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__44)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__45)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__46)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__47)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__48)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__49)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__50)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__51)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__52)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__53)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__54)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__55)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__56)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__57)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__58)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__59)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__60)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__61)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__62)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__63)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__64)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__65)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__66)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__67)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__68)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__69)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__70)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__71)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__72)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__73)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__74)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__75)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mT__76)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mLiteral)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mBool)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mBits)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mHex)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mDec)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mChar)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mStr)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mBuiltIn)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mIdentifier)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mAssignmentOp)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mRelationalOp)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mUnaryOp)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mWS)	(struct MyLangLexer_Ctx_struct * ctx);

     void
     (*mTokens)	(struct MyLangLexer_Ctx_struct * ctx);
    const char * (*getGrammarFileName)();
    void            (*reset)  (struct MyLangLexer_Ctx_struct * ctx);
    void	    (*free)   (struct MyLangLexer_Ctx_struct * ctx);
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pMyLangLexer MyLangLexerNew         (
pANTLR3_INPUT_STREAM
 instream);
ANTLR3_API pMyLangLexer MyLangLexerNewSSD      (
pANTLR3_INPUT_STREAM
 instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the 
lexer
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
#define ExpressionListToken      19
#define ExpressionToken      20
#define FuncDefToken      21
#define FuncSignatureToken      22
#define Hex      23
#define Identifier      24
#define Literal      25
#define LoopToken      26
#define RangeToken      27
#define RelationalOp      28
#define RepeatToken      29
#define ReturnToken      30
#define SliceToken      31
#define SourceToken      32
#define Str      33
#define TypeRefToken      34
#define UnaryOp      35
#define VarDeclToken      36
#define WS      37
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for MyLangLexer
 * =============================================================================
 */
/** } */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
