#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <algorithm>

#include "Node.h"
#include "MemoryManager.h"
#include "Token.h"

class Parser {

public:
    Parser(MemoryManager& manager): mm{ manager }{}

    ~Parser() = default;
    Parser(Parser const&) = delete;
    Parser& operator=(Parser const&) = delete;

    //un oggetto p di tipo Parser inizia il parsing chiamando p()
    Program* operator()(std::vector<Token>& tokenStream) {
        if(tokenStream.empty())
            throw ParseError("Il programma e' vuoto");

        tokenVector = tokenStream.begin();
        streamEnd = tokenStream.end();
        Program* p = buildProgram();
        if (tokenVector != streamEnd) {
            throw ParseError("Unexpected end of input");
        }
        return p;
    }



private:
    std::vector<Token>::const_iterator streamEnd;

    //token da analizzare durante il parsing 
    std::vector<Token>::const_iterator tokenVector;
  
    //la classe che si occupa di deallocare i nodi dell'ast 
    MemoryManager& mm;

    //Si elencano le variabili già definite nel seguente vettore
    std::vector<std::string> initializedVariables;
    
    //funzioni che si occupano di costurire l'ast
    Program* buildProgram();
    Block* buildBlock();
    Decls* buildDecls();
    Decl* buildDecl();
    Type* buildType();
    Id* buildId();
    Seq* buildSeq();
    Stmt* buildStmt();
    Expression* buildBool();
    Expression* buildOr();
    Expression* buildJoin();
    Expression* buildEquality();
    Expression* buildRel();
    Expression* buildExpr();
    Expression* buildTerm();
    Expression* buildUnary();
    Expression* buildFactor();


    //per ignorare il token corrente
    void safe_next() {
        if (tokenVector == streamEnd) {
            throw ParseError("Unexpected end of input");
        }
        ++tokenVector;
    }

    //Prima di fare safe_next si controlla che il token sia corrente sia quello che ci si aspetta
    void safe_next(const int tokenId)
    {
        if (tokenVector->tag == tokenId)
            safe_next();
        else 
            throw ParseError(std::string("Expecting ")+=(Token::id2word[tokenId]));
    }

};

#endif
