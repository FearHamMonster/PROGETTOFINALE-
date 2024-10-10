#ifndef NODE_H
#define NODE_H
#include <string>
#include <map>

#include "Exceptions.h"




//dichiarazione forward di visitor
class Visitor;  

class Node
{
    public:
    virtual ~Node() = default;
    Node() = default;
    virtual  int accept(Visitor* v) = 0;
};

//Type
class Type : public Node{

public:
    enum TypeCode{INT_CONST, BOOL_CONST};

    Type(TypeCode t) : type{t}, isVect{false}{}
    Type(TypeCode t, int s) : type{t}, size{s}, isVect{true}{}

    TypeCode getTypeCode() {return type;}
    
    bool isVector(){
        return isVect;
    }
    int getSize()
    {
        if(isVector()) return size;
        
        throw EvaluationError("Trying to get size from non vector type");
    }


    int accept(Visitor* v) override;   

private:
    TypeCode type;
    int size;
    bool isVect;
};

//Expression
class Expression : public Node{
};

class Constant : public Expression {
    
public: 
    Constant(Type::TypeCode t, int v) : typeCode{t}, value{v} {}
    Type::TypeCode getTypeCode() {return typeCode;}
    
    //si usa value intero, ma viene usato per memorizzare anche variabili booleane
    int getValue() {
        return value;
    }
    int accept(Visitor* v);
private:
    Type::TypeCode typeCode;
    int value;
};

class Id: public Expression
{
public:
  Id(std::string name_) : name{name_} {};
  Id& operator= (const Id& other) = default;
  
  std::string getName() {
    return name;
  }

   int accept(Visitor* v) override;

private:
  std::string name; 
};

//Decls
class Decl : public Node{
public:

    Decl(Type* t, Id* i): type{t}, id{i}{}
    Type* getType(){return type;}
    Id* getId(){return id;}

     int accept(Visitor* v) override;
private:
    Type* type;
    Id* id;
};

class Decls : public Node{
public:

    Decls(Decl* dec, Decls* decs ): declaration{dec}, declarations{decs}{}

    Decl* getDecl(){return declaration;}
    Decls* getDecls(){return declarations;}

     int accept(Visitor* v) override;

private:
    Decl* declaration;
    Decls* declarations; 
};

class Stmt : public Node{
};

//Classi per statements
class Seq: public Node{
public:

Seq(Seq* seq_, Stmt* stmt_) : seq{seq_}, stmt{stmt_}{}

Seq* getSeq() {return seq;}
Stmt* getStmt() {return stmt;}

 int accept(Visitor* v) override;   

private:
Seq* seq;
Stmt* stmt;
};

class Block : public Stmt{
public:

    Block(Decls* decs, Seq* Seq) : declarations{decs}, statements{Seq}{}
    Decls* getDecls(){return declarations;}
    Seq* getSeq(){return statements;}

     int accept(Visitor* v) override;   

private:
    Decls* declarations;
    Seq* statements;

};


//Program
class Program : public Node{
public:

    Program(Block* b) : block{b}{}

    Block* getBlock() {return block;}
    
     int accept(Visitor* v) override;

private:
    Block* block;
};

//Operations
class Op : public Expression {
};


//Superclassi per operazioni binarie/unarie aritmetiche e booleane
class BinaryOp : public Op{
public:
    BinaryOp(Expression* l, Expression* r) :
    left{l}, right{r}{} 
    
    Expression* getLeftExp() {return left;}
    Expression* getRightExp() {return right;}
private:
    Expression* left;
    Expression* right;
};

class UnaryOp : public Op{
public:
    UnaryOp( Expression* ex) : exp{ex}{}
    Expression* getExp() {return exp;}

private:
    Expression* exp;
};


//Operazioni unarie
class NegOp : public UnaryOp {
public:
    NegOp(Expression* e) : UnaryOp(e){} 
     int accept(Visitor* v) override;    
};

class Not : public UnaryOp{
public:
    Not(Expression* e) : UnaryOp(e){} 
     int accept(Visitor* v) override;    
};


//Operazioni binarie con valore di ritorno booleano
class And : public BinaryOp{
public:
    And(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Or : public BinaryOp{
public:
    Or(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class More : public BinaryOp{
public:
    More(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class MoreEqual : public BinaryOp{
public:
    MoreEqual(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Less : public BinaryOp{
public:
    Less(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class LessEqual : public BinaryOp{
public:
    LessEqual(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Equal : public BinaryOp{
public:
    Equal(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class NotEqual : public BinaryOp{
public:
    NotEqual(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};


//Operazioni binarie con valore di ritorno aritmetico
class Add : public BinaryOp{
public:
    Add(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Sub : public BinaryOp{
public:
    Sub(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Mul : public BinaryOp{
public:
    Mul(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};

class Div : public BinaryOp{
public:
    Div(Expression* l, Expression* r) : BinaryOp(l,r){}
     int accept(Visitor* v) override;
};



//operazione di accesso indicizzato a vettore
class Access : public Op{
public:

    Access(Id* vec, Expression* ind) : vector{vec}, index{ind}{}
    Id* getId() {return vector;}
    Expression* getIndex() {return index;}

     int accept(Visitor* v) override;

private:
    Id* vector;
    Expression* index;

};




//Statements    
class If : public Stmt {
public:

    If(Stmt* s, Expression* e) : stmt{s}, condition{e}{}

    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}

     int accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
};

class Else : public Stmt{
public:

    Else(Stmt* stmtTrue, Stmt* stmtFalse, Expression* e)
     : stmtIfTrue{stmtTrue}, stmtIfFalse{stmtFalse}, condition{e}{}

    Stmt* getifTrueStmt() {return stmtIfTrue;}
    Stmt* getifFalseStmt() {return stmtIfFalse;}
    Expression* getCondition () {return condition;}

     int accept(Visitor* v) override;   

private:
    Expression* condition;
    Stmt* stmtIfTrue;
    Stmt* stmtIfFalse;
};

class While : public Stmt{
public:

    While(Stmt* s, Expression* e) : stmt{s}, condition{e}{}
    
    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}

 
     int accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
};

class Do : public Stmt{
public:

    Do(Stmt* s, Expression* e) : stmt{s}, condition{e}{}
    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}


     int accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
  
};

class Set : public Stmt{
public:

    Set(Id* var, Expression* e) : variable{var}, exp{e}{}
    Id* getId(){return variable;}
    Expression* getExp(){return exp;}

     int accept(Visitor* v) override;   

private:
    Id* variable;
    Expression* exp;    
};

class SetElem : public Stmt{
public:

    SetElem(Id* v, Expression* e, Expression* i) :
         arrayName{v}, exp{e}, index{i} {}

    Id* getId(){return arrayName;}
    Expression* getExp(){return exp;}
    Expression* getIndex(){return index;}


     int accept(Visitor* v) override;   


private:
    Id* arrayName;
    Expression* exp;    
    Expression* index;
};

class Break : public Stmt{
public:
    Break() = default;
     int accept(Visitor* v) override;   

};

class Print : public Stmt{
public:

    Print(Expression* e) : expToPrint{e}{}
    Expression* getExp(){return expToPrint;}
   
     int accept(Visitor* v) override;   

private:
    Expression* expToPrint;    
};


#endif