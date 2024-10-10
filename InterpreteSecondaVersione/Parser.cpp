#include <sstream>
#include "Parser.h"


Program* Parser::buildProgram()
{
    return mm.makeProgram(buildBlock());
}


Block* Parser::buildBlock()
{
    safe_next(Token::CURLY_L);
    auto decls = buildDecls();
    auto seq = buildSeq();
    Block* block = mm.makeBlock(decls, seq);
    safe_next(Token::CURLY_R);
    return block;
}

Seq* Parser::buildSeq()
{
    //Se non si trova una parentesi graffa chiusa, ci sono altri statement
    if(tokenVector->tag != Token::CURLY_R) 
    {
        Stmt* stmt = buildStmt();
        Seq* Seq = buildSeq();
        return mm.makeSeq(Seq,stmt);
    }
    else return nullptr;
}

Stmt* Parser::buildStmt()
{
    switch(tokenVector->tag)    //si controllano i simboli iniziali per determinare quale statement costruire
    {
        case Token::ID:
        {   
            Id* id = buildId();

            if(tokenVector->tag == Token::SQUARE_L)  //Si accede ad un elemento dell'array
            {
                safe_next();
                Expression* indice = buildBool();
                safe_next(Token::SQUARE_R);
                safe_next(Token::ASSIGN);
                Expression* expr = buildBool();
                safe_next(Token::SEMICOLON);
                return mm.makeSetElem(id, indice, expr);   
            }
            else   //si accede ad una variabile semplice 
            {
                safe_next(Token::ASSIGN);
                Expression* expr = buildBool();
                safe_next(Token::SEMICOLON);
                return mm.makeSet(id, expr);

            }
        }

        case Token::WHILE:
        {
            safe_next();
            safe_next(Token::PAR_L);
            Expression* expr = buildBool();
            safe_next(Token::PAR_R);
            Stmt* stmt = buildStmt();
            return mm.makeWhile(stmt,expr);
        }

        case Token::DO:
        {
            safe_next();
            Stmt* stmt = buildStmt();
            safe_next(Token::WHILE);
            safe_next(Token::PAR_L);
            Expression* expr = buildBool();
            safe_next(Token::PAR_R);
            safe_next(Token::SEMICOLON);
            return mm.makeDo(stmt,expr);
        }
       
        case Token::IF:
        {  
            safe_next();
            safe_next(Token::PAR_L);
            Expression* condition = buildBool();
            safe_next(Token::PAR_R);
            Stmt* stmt1 = buildStmt();
            if(tokenVector->tag == Token::ELSE)
            {
                safe_next();
                Stmt* stmt2 = buildStmt();
                return mm.makeElse(stmt1,stmt2,condition);
            }
            return mm.makeIf(stmt1, condition);
        }

        
        case Token::BREAK:
        {
            safe_next();
            safe_next(Token::SEMICOLON);
            return mm.makeBreak();
        }

        case Token::PRINT:
        {
            safe_next();
            safe_next(Token::PAR_L);
            Expression* expr = buildBool();
            safe_next(Token::PAR_R);
            safe_next(Token::SEMICOLON);
            return mm.makePrint(expr);
        }

        case Token::CURLY_L:
        {
            return buildBlock();
        }

        default:    
        {
            throw ParseError("Errore durante parsing di statement");
        }
    }
}


Decls* Parser::buildDecls()
{
    //controllo che se c'è un altro token tipo, perchè se c'è allora si deve creare un altro decls
    if(tokenVector->tag == Token::BOOL_CONST || tokenVector->tag == Token::INT_CONST)
    {
        Decl* decl = buildDecl();
        return mm.makeDecls(decl, buildDecls());
    }
    else return nullptr;

}

Decl* Parser::buildDecl()
{
  Type* type = buildType();
  Id* id = buildId();
  
  
  //controllo necessario per evitare di dichiarare la stessa variabile una seconda volta
  for(int i = 0; i<initializedVariables.size(); i++)
  {
    if(initializedVariables[i] == id->getName())
      throw ParseError("la variabile " + id->getName() + "e' stata gia' dichiarata");
  }
  initializedVariables.push_back(id->getName());
  
  
  safe_next(Token::SEMICOLON);
  return mm.makeDecl(type,id);
}

Id* Parser::buildId()
{
    if(tokenVector->tag != Token::ID)
        throw ParseError("Token errato, identificatore non trovato");
        
    auto id = mm.makeId(tokenVector->word);
    safe_next();
    return id;
    
}

Type* Parser::buildType()
{
    Type::TypeCode tc;
    if (tokenVector->tag == Token::INT_CONST)
        tc = Type::INT_CONST;
    else if(tokenVector->tag == Token::BOOL_CONST)
        tc = Type::BOOL_CONST;

    safe_next(); 
    //si costruisce un tipo array
    if(tokenVector->tag == Token::SQUARE_L)
    {
        safe_next();    
        if(tokenVector->tag != Token::NUM)
            throw ParseError("Token errato, costante numerica non trovata");
        
        //stoi converte da string a int
        Type* type = mm.makeType(tc, std::stoi(tokenVector->word));
        safe_next();    
        safe_next(Token::SQUARE_R);  
        return type;
    }
    //oppure si costruisce un tipo primitivo
    return mm.makeType(tc);
}

//Si costruiscono le espressioni, dividendo nelle varie funzioni per garantire le regole di precedenza e associatività
Expression* Parser::buildBool()
{
    Expression* exp = buildJoin();
    while(tokenVector->tag == Token::OR)
    {
        safe_next();
        exp = mm.makeOr(exp, buildJoin());
    }
    return exp;
}

Expression* Parser::buildJoin()
{
    Expression* exp = buildEquality();
    while(tokenVector->tag == Token::AND)
    {
        safe_next();
        exp = mm.makeAnd(exp, buildEquality());
    }
    return exp;
    
}

Expression* Parser::buildEquality()
{
    Expression* exp = buildRel();
    while(tokenVector->tag == Token::EQ || tokenVector->tag == Token::NOT_EQ)
    {
        if(tokenVector->tag == Token::EQ)
        {
            safe_next();
            exp = mm.makeEqual(exp, buildRel());
        }
        else if(tokenVector->tag == Token::NOT_EQ)
        {
            safe_next();
            exp = mm.makeNotEqual(exp, buildRel());
        }
    }
    return exp;
}

Expression* Parser::buildRel()
{
    Expression* left = buildExpr();   
    switch(tokenVector->tag)
    {
        case Token::LESS:
        {
            safe_next();
            return mm.makeLess(left,buildExpr());
        }

        case Token::LESS_EQ:
        {
            safe_next();
            return mm.makeLessEqual(left,buildExpr());
        }
        
        case Token::MORE:
        {
            safe_next();
            return mm.makeMore(left,buildExpr());
        }

        case Token::MORE_EQ:
        {
            safe_next();
            return mm.makeMoreEqual(left,buildExpr());
        } 

        default:
        {
            return left;
        }
    }

}


Expression* Parser::buildExpr()
{
    Expression* exp = buildTerm();
    while(tokenVector->tag == Token::ADD || tokenVector->tag == Token::MIN)
    {
        if(tokenVector->tag == Token::ADD)
        {
            safe_next();
            exp = mm.makeAdd(exp, buildTerm());
        }
        else if(tokenVector->tag == Token::MIN)
        {
            safe_next();
            exp = mm.makeSub(exp, buildTerm());
        }
    }
    return exp;   
}

Expression* Parser::buildTerm()
{
    Expression* exp = buildUnary();
    while(tokenVector->tag == Token::MUL || tokenVector->tag == Token::DIV)
    {
        if(tokenVector->tag == Token::MUL)
        {
            safe_next();
            exp = mm.makeMul(exp, buildUnary());
        }
        else if(tokenVector->tag == Token::DIV)
        {
            safe_next();
            exp = mm.makeDiv(exp, buildUnary());
        }
    }
    return exp;   
}

Expression* Parser::buildUnary()
{
    switch(tokenVector->tag)
    {
        case Token::NOT:
            safe_next();
            return mm.makeNot(buildUnary());
            break;

        case Token::MIN:
            safe_next();
            return mm.makeNegOp(buildUnary());
            break;
        default:
            return buildFactor();
            break;
    }
}


Expression* Parser::buildFactor()
{
    switch(tokenVector->tag)
    {
        case Token::PAR_L:
        {
            safe_next();
            Expression* exp = buildBool();
            safe_next(Token::PAR_R);
            return exp;
        }
        
        case Token::ID:
        {
            Id* id = buildId();
            if(tokenVector->tag == Token::SQUARE_L) 
            {
                safe_next();
                Expression* index = buildBool();
                safe_next(Token::SQUARE_R);
                return mm.makeAccess(id, index);
            }
            return id;
        }

        case Token::NUM:
        {
            std::string stringNum = tokenVector->word;
            safe_next();
            //si converte word in intero
            return mm.makeConstant(std::stoi(stringNum),Type::INT_CONST);
        }

        //weather token is true or false  is created
        case Token::TRUE:

        case Token::FALSE:
        {
            std::string word = tokenVector->word;
            safe_next();
            if(word == Token::id2word[Token::TRUE])
                return mm.makeConstant(true,Type::BOOL_CONST);
            
            else if(word == Token::id2word[Token::FALSE])
                return mm.makeConstant(false,Type::BOOL_CONST);
        }

        default:
        {
            throw ParseError("Errore durante parsing di factor");
        }
    }       
}