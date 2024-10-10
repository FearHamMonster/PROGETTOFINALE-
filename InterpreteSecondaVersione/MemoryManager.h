#ifndef EXPR_MANAGER_H
#define EXPR_MANAGER_H
#include "Node.h"

class MemoryManager {
public:

    MemoryManager() = default;
    ~MemoryManager() {
        clearMemory();
    }

    MemoryManager(const MemoryManager& other) = delete;
    MemoryManager& operator=(const MemoryManager& other) = delete;

    //Nodi di tipo espressione

    Not* makeNot(Expression* expr) {
        Not* node = new Not(expr);
        nodesVector.push_back(node);
        return node;
    }

    NegOp* makeNegOp(Expression* expr) {
        NegOp* node = new NegOp(expr);
        nodesVector.push_back(node);
        return node;
    }

    And* makeAnd(Expression* expr1, Expression* expr2 ) {
        And* node = new And(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }

    Or* makeOr(Expression* expr1, Expression* expr2 ) {
        Or* node = new Or(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    Add* makeAdd(Expression* expr1, Expression* expr2 ) {
        Add* node = new Add(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }

    Sub* makeSub(Expression* expr1, Expression* expr2 ) {
        Sub* node = new Sub(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    Mul* makeMul(Expression* expr1, Expression* expr2 ) {
        Mul* node = new Mul(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    Div* makeDiv(Expression* expr1, Expression* expr2 ) {
        Div* node = new Div(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    Less* makeLess(Expression* expr1, Expression* expr2 ) {
        Less* node = new Less(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    LessEqual* makeLessEqual(Expression* expr1, Expression* expr2 ) {
        LessEqual* node = new LessEqual(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    More* makeMore(Expression* expr1, Expression* expr2 ) {
        More* node = new More(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    MoreEqual* makeMoreEqual(Expression* expr1, Expression* expr2 ) {
        MoreEqual* node = new MoreEqual(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    Equal* makeEqual(Expression* expr1, Expression* expr2 ) {
        Equal* node = new Equal(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }
    
    NotEqual* makeNotEqual(Expression* expr1, Expression* expr2 ) {
        NotEqual* node = new NotEqual(expr1,expr2);
        nodesVector.push_back(node);
        return node;
    }

    Id* makeId(std::string idName) {
        Id* node = new Id(idName);
        nodesVector.push_back(node);
        return node;
    }


    //Nodo di tipo program, block, decls, type

    Program* makeProgram(Block* block)
    {
        Program* node = new Program(block);
        nodesVector.push_back(node);
        return node;
    }
    Block* makeBlock(Decls* decls, Seq* Seq){
        Block* node = new Block(decls, Seq);
        nodesVector.push_back(node);
        return node;
    }

    Decls* makeDecls(Decl* decl, Decls* decls){
        Decls* node = new Decls(decl, decls);
        nodesVector.push_back(node);
        return node;
    }

    Decl* makeDecl(Type* type, Id* idName){
        Decl* node = new Decl(type, idName);
        nodesVector.push_back(node);
        return node;
    }

    Type* makeType(Type::TypeCode type){
        Type* node = new Type(type);
        nodesVector.push_back(node);
        return node;
    }

    Type* makeType(Type::TypeCode type, int size){
        Type* node = new Type(type,size);
        nodesVector.push_back(node);
        return node;
    }

    //Constant gestisce sia constanti booleane sia intere
      Constant* makeConstant(int value, Type::TypeCode t) {
        Constant* node = new Constant(t,value);
        nodesVector.push_back(node);
        return node;
    }

    Access* makeAccess(Id* idName, Expression* index)
    {
        Access* node = new Access(idName, index);
        nodesVector.push_back(node);
        return node;
    }

    //Nodi di tipo statement
    
    Seq* makeSeq(Seq* seq, Stmt* stmt)
    {
        Seq* node = new Seq(seq,stmt);
        nodesVector.push_back(node);
        return node;        
    }

    If* makeIf(Stmt* stmt, Expression* exp)
    {
        If* node = new If(stmt,exp);
        nodesVector.push_back(node);
        return node;        
    }

    Else* makeElse(Stmt* stmtIfTrue, Stmt* stmtIfFalse, Expression* expr)
    {
        Else* node = new Else(stmtIfTrue, stmtIfFalse, expr);
        nodesVector.push_back(node);
        return node;        
    }

    While* makeWhile(Stmt* stmt, Expression* expr)
    {
        While* node = new While(stmt, expr);
        nodesVector.push_back(node);
        return node;        
    }

    Do* makeDo(Stmt* stmt, Expression* expr)
    {
        Do* node = new Do(stmt, expr);
        nodesVector.push_back(node);
        return node;        
    }

    Set* makeSet(Id* idName, Expression* value)
    {
        Set* node = new Set(idName, value);
        nodesVector.push_back(node);
        return node;        
    }
    
    SetElem* makeSetElem(Id* vectorName, Expression* index, Expression* value)
    {
        SetElem* node = new SetElem(vectorName, value, index);
        nodesVector.push_back(node);
        return node;        
    }

    Break* makeBreak()
    {
        Break* node = new Break();
        nodesVector.push_back(node);
        return node;        
    }
    
    Print* makePrint(Expression* expToPrint)
    {
        Print* node = new Print(expToPrint);
        nodesVector.push_back(node);
        return node;
    }


    //si deallocano tutti i nodi allocati
    void clearMemory() {
        auto i = nodesVector.begin();
        for (; i != nodesVector.end(); ++i) {
            delete(*i);
        }
        nodesVector.resize(0);
    }

private:
    std::vector<Node*> nodesVector;
};



#endif
