#ifndef VISITOR_H
#define VISITOR_H

#include <vector>
#include <iostream>

#include "Node.h"
#include "Exceptions.h"
#include "MemoryManager.h"
#include "VariableSpace.h"


class Visitor {
public:
    //Altre visite
    virtual int visitProgram(Program* obj)=0;
    virtual int visitBlock(Block* obj)=0;

    //Dichiarazioni
    virtual int visitDecls(Decls* obj)=0;
    virtual int visitDecl(Decl* obj)=0;
    virtual int visitId(Id* obj)=0;
    virtual int visitType(Type* obj)=0;

    //Statements
    virtual int visitIf(If* obj)=0;
    virtual int visitElse(Else* obj)=0;
    virtual int visitWhile(While* obj)=0;
    virtual int visitDo(Do* obj)=0;
    virtual int visitSet(Set* obj)=0;
    virtual int visitSetElem(SetElem* obj)=0;
    virtual int visitBreak(Break* obj)=0;
    virtual int visitPrint(Print* obj)=0;
    virtual int visitSeq(Seq* obj)=0;
    virtual int visitAccess(Access* obj)=0;

    //Operazioni logiche e aritmetiche
    virtual int visitAdd(Add* obj)=0;
    virtual int visitSub(Sub* obj)=0;
    virtual int visitMul(Mul* obj)=0;
    virtual int visitDiv(Div* obj)=0;
    virtual int visitAnd(And* obj)=0;
    virtual int visitConstant(Constant* obj)=0;
    virtual int visitOr(Or* obj)=0;
    virtual int visitLess(Less* obj)=0;
    virtual int visitLessEqual(LessEqual* obj)=0;
    virtual int visitMore(More* obj)=0;
    virtual int visitMoreEqual(MoreEqual* obj)=0;
    virtual int visitEqual(Equal* obj)=0;
    virtual int visitNotEqual(NotEqual* obj)=0;
    virtual int visitNegOp(NegOp* obj)=0;
    virtual int visitNot(Not* obj)=0;


};

// Visitor concreto per la valutazione delle espressioni
class EvaluationVisitor : public Visitor {
public:
    EvaluationVisitor(VariableSpace& e) : env {e},EncounteredBreak{false}{}

    //copy constructor e = operator ridefiniti
    EvaluationVisitor(EvaluationVisitor const&) = delete;
    EvaluationVisitor& operator=(EvaluationVisitor const&) = delete;


    //visita di nodi statement e altri: si ritorna 0 perchè il compito di queste visite non è di calcolo di espressioni
    int visitProgram(Program* programObj) override {
        programObj->getBlock()->accept(this);
         return 0;
    }

    int visitBlock(Block* blockObj) override {
        if(blockObj->getDecls()) 
            blockObj->getDecls()->accept(this);
        if(blockObj->getSeq()) 
            blockObj->getSeq()->accept(this);
         return 0;
    }

    int visitDecls(Decls* declsObj) override {
        //to ensure that declarations are done in order, it's important to visit the decl object first
        declsObj->getDecl()->accept(this);
        if(declsObj->getDecls())
            declsObj->getDecls()->accept(this);
         return 0;
    }

    int visitDecl(Decl* declObj) override { 
        Type* t = declObj->getType();
        //si controlla che il tipo sia primitivo o di tipo vector
        if(declObj->getType()->isVector())
            env.declareArray(declObj->getId()->getName(), t->getTypeCode(), t->getSize());
        else
            env.declareConstant(declObj->getId()->getName(), t->getTypeCode());
         return 0;
    }

    int visitSeq(Seq* seqObj) override {
        //Se si incontra un break, non si eseguono altre istruzioni finchè non si esce dal ciclo
        if(EncounteredBreak)  return 0;

        //IMPORTANTE: si visita prima lo stmt, per assicurarsi di eseguire gli stmt in ordine
        seqObj->getStmt()->accept(this);

        if(seqObj->getSeq())
            seqObj->getSeq()->accept(this);

         return 0;
    }    


     int visitPrint(Print* printObj) {
        std::cout<<printObj->getExp()->accept(this)<<std::endl; 
        return 0;
    };

    int visitIf(If* ifObj) override {

        if (ifObj->getCondition()->accept(this))
            ifObj->getStmt()->accept(this);
         return 0;
    }


    int visitElse(Else* elseObj) override {

        if (elseObj->getCondition()->accept(this))
            elseObj->getifTrueStmt()->accept(this);
        else
            elseObj->getifFalseStmt()->accept(this);
         return 0;
    }

    int visitWhile(While* whileObj) override {
        while(whileObj->getCondition()->accept(this))
        {
            whileObj->getStmt()->accept(this);
            if(EncounteredBreak) 
            {
                EncounteredBreak = false;
                break;
            }
        }
        return 0;
    }

    int visitDo(Do* doObj) override {
        do
        {   
            if(EncounteredBreak)
            {
                EncounteredBreak = false;
                break;
            }
            doObj->getStmt()->accept(this);
        } while(doObj->getCondition()->accept(this));
        return 0;
    }

    int visitSet(Set* setObj) override {
        std::string idName =  setObj->getId()->getName();
        int value = setObj->getExp()->accept(this);

        env.assignToId(idName, currentType, value);
        return 0;
    }

    int visitSetElem(SetElem* setElemObj) override {
        std::string idName =  setElemObj->getId()->getName();
        
        //bisogna per forza ricavare il valore di value alla fine, in modo da poter accedere
        //corretamente al suo tipo con currentType
        int index = setElemObj->getIndex()->accept(this);
        int value = setElemObj->getExp()->accept(this);
     
        env.assignToArray(idName,currentType,value,index);
        return 0;
    }

     int visitBreak(Break* breakObj) {
        EncounteredBreak = true;
        return 0;
    }

    //visitare un tipo equivale a non fare nulla
    int visitType(Type* type) 
    { 
        return 0;
    }


    //visita di nodi espressione: da qui si ritorna non più 0 ma valori significativi 

     int visitId(Id* idObj) {
        currentType = env.getIdType(idObj->getName());
        return env.getIdValue(idObj->getName());
    }


    int visitNot(Not* notObj) override {
        int expr = notObj->getExp()->accept(this);
        checkIsBool(); 
        return !expr;
    }

    int visitNegOp(NegOp* negOpObj) override {
        int expr = negOpObj->getExp()->accept(this);
        checkIsInt(); 
        return -expr;
    }    

    int visitAnd(And* andObj) override {
        int left = andObj->getLeftExp()->accept(this);
        checkIsBool(); 
        int right = andObj->getRightExp()->accept(this);
        checkIsBool(); 
        return left && right;
    }

    int visitOr(Or* orObj) override {
        int left = orObj->getLeftExp()->accept(this);
        checkIsBool(); 
        int right = orObj->getRightExp()->accept(this);
        checkIsBool(); 
        return left || right;
    }

    int visitAdd(Add* addObj) override {
        int left = addObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = addObj->getRightExp()->accept(this);
        checkIsInt(); 
        return left + right;
    }

    int visitSub(Sub* subObj) override {
        int left = subObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = subObj->getRightExp()->accept(this);
        checkIsInt(); 
        return left - right;
    }

    int visitMul(Mul* mulObj) override {
        int left = mulObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = mulObj->getRightExp()->accept(this);
        checkIsInt(); 
        return left * right;
    }

    int visitDiv(Div* divObj) override {
        int denominator = divObj->getRightExp()->accept(this); 
        checkIsInt(); 

        if(denominator==0) throw EvaluationError("Division by 0 is not permitted");

        int numerator = divObj->getLeftExp()->accept(this);
        checkIsInt(); 
        return numerator / denominator;
    }

    int visitMore(More* moreObj) override {
        int left = moreObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = moreObj->getRightExp()->accept(this);
        checkIsInt(); 
        //necessario aggiornare currentType, sennò rimarrebbe impostato a int
        currentType = Type::BOOL_CONST;
        return left > right;
    }

    int visitMoreEqual(MoreEqual* moreEqualObj) override {
        int left = moreEqualObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = moreEqualObj->getRightExp()->accept(this);
        checkIsInt(); 
        currentType = Type::BOOL_CONST;
        return left >= right;
    }

    int visitLess(Less* lessObj) override {
        int left = lessObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = lessObj->getRightExp()->accept(this);
        checkIsInt(); 
        currentType = Type::BOOL_CONST;
        return left < right;
    }

    int visitLessEqual(LessEqual* lessEqualObj) override {
        int left = lessEqualObj->getLeftExp()->accept(this);
        checkIsInt(); 
        int right = lessEqualObj->getRightExp()->accept(this);
        checkIsInt(); 
        currentType = Type::BOOL_CONST;
        return left <= right;
    }

    int visitEqual(Equal* equalObj) override {          
        int left = equalObj->getLeftExp()->accept(this);
        Type::TypeCode t1 = currentType; 
        int right = equalObj->getRightExp()->accept(this);
        Type::TypeCode t2 = currentType; 
        
        //l'operazione equal è ammessa sia tra tipi int che bool: bisogna però controllare che i tipi destro e sinistro coincidano
        if(t1!=t2) throw EvaluationError("Eguaglianza tra espressioni di tipo distinto");

        currentType = Type::BOOL_CONST;
        return left == right;
    }

    int visitNotEqual(NotEqual* notEqualObj) override {
        int left = notEqualObj->getLeftExp()->accept(this);
        Type::TypeCode t1 = currentType; 
        int right = notEqualObj->getRightExp()->accept(this);
        Type::TypeCode t2 = currentType; 
        
        if(t1!=t2) throw EvaluationError("Eguaglianza tra espressioni di tipo distinto");

        currentType = Type::BOOL_CONST;
        return left != right;
    }

    int visitConstant(Constant* constantObj) override {
        //si aggiorna currentType con il tipo della costante visitata
        currentType = constantObj->getTypeCode();
        return constantObj->getValue();
    }


    int visitAccess(Access* accessObj)
    {
        int index = accessObj->getIndex()->accept(this);
        //si aggiorna current type in quanto è' l'ultimo tipo estratto
        currentType = env.getArrayType(accessObj->getId()->getName());

        //si estrae dall'array in memoria la cella corrispondente. Tutti i vari controlli (come out of bound) sono fatti dall'ambiente dinamico
        return env.getArrayCell(accessObj->getId()->getName(), index);
    }

    
private:

    void checkIsBool(){
        if(currentType!=Type::BOOL_CONST)
            throw EvaluationError("Errore: expecting bool expression");
    }

    void checkIsInt(){
        if(currentType!=Type::INT_CONST)
            throw EvaluationError("Errore: expecting int expression");
    }
    //currentType riflette il tipo dell'ultima espressione visitata
    Type::TypeCode currentType;
    //quando si incontra un break EncounteredBreak è vera
    bool EncounteredBreak;
    VariableSpace& env;
};


// Visitor concreto per la stampa delle espressioni
class PrintVisitor : public Visitor {
public:
    PrintVisitor& operator=(PrintVisitor const&) = delete;

    int visitProgram(Program* program) override {
        std::cout<<"Program(";
        program->getBlock()->accept(this);
        std::cout<<")";
        return 0;
    }

    int visitBlock(Block* block) override {
        std::cout<<"Block(";
        if(block->getDecls()) 
            block->getDecls()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<", ";
        if(block->getSeq()) 
            block->getSeq()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
         return 0;
    }

    int visitDecls(Decls* decls) override {

        std::cout<<"Decls(";
        decls->getDecl()->accept(this);
        std::cout<<", ";
        if(decls->getDecls())
            decls->getDecls()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
         return 0;
    }

    int visitDecl(Decl* decl) override {
        std::cout<<"Decl(";
        decl->getType()->accept(this);
        std::cout<<", ";
        decl->getId()->accept(this);
        std::cout<<")";

         return 0;
    }

    int visitType(Type* type) override {
        std::cout<<"Type(";
        if(type->isVector())
        {
            std::cout<<",[";        
            std::cout<<type->getSize();
            std::cout<<"]";        
        }
        std::cout<<")";        
        return 0;
    }


    int visitId(Id* id) override {
        std::cout<<"Id(";
        std::cout<<id->getName();
        std::cout<<")";

         return 0;
    }

    int visitSeq(Seq* Seq) override {

        std::cout<<"Seq(";
        Seq->getStmt()->accept(this);
        std::cout<<", ";
        if(Seq->getSeq())
            Seq->getSeq()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
         return 0;
    }    
    
    int visitIf(If* ifObj) override {
        std::cout<<"If(";
        ifObj->getCondition()->accept(this);
        std::cout<<", ";
        ifObj->getStmt()->accept(this);
        std::cout<<")";
         return 0;
    }


    int visitElse(Else* elseObj) override {
        std::cout<<"Else(";
        elseObj->getCondition()->accept(this);
        std::cout<<", ";
        elseObj->getifTrueStmt()->accept(this);
        std::cout<<", ";
        elseObj->getifFalseStmt()->accept(this);
        std::cout<<")";
         return 0;
    }

    int visitWhile(While* whileObj) override {
        std::cout<<"While(";
        whileObj->getCondition()->accept(this);
        std::cout<<", ";
        whileObj->getStmt()->accept(this);
        std::cout<<")";
         return 0;
    }

    int visitDo(Do* doObj) override {
        std::cout<<"Do(";
        doObj->getCondition()->accept(this);
        std::cout<<", ";
        doObj->getStmt()->accept(this);
        std::cout<<")";
         return 0;
    }

    int visitSet(Set* setObj) override {
        std::cout<<"Set(";
        setObj->getId()->accept(this);
        std::cout<<", ";        
        setObj->getExp()->accept(this);
        std::cout<<")";
         return 0;
    }

    int visitSetElem(SetElem* setElemObj) override {
        std::cout<<"SetElem(";
        setElemObj->getId()->accept(this);
        std::cout<<",[";        
        setElemObj->getIndex()->accept(this);
        std::cout<<"], ";
        setElemObj->getExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitBreak(Break* breakObj) override {
        std::cout<<"Break()";
         return 0;
    }

    int visitPrint(Print* printObj) override {
        std::cout<<"Print(";
        printObj->getExp()->accept(this);
        std::cout<<")";
         return 0;
    }

    int visitNot(Not* notObj) override {
        std::cout<<"Not(";
        notObj->getExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitNegOp(NegOp* notObj) override {
        std::cout<<"NegOp(";
        notObj->getExp()->accept(this);
        std::cout<<")";        
         return 0;
    }    


    int visitAnd(And* andObj) override {
        std::cout<<"And(";
        andObj->getLeftExp()->accept(this);
        std::cout<<", ";        
        andObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitOr(Or* orObj) override {
        std::cout<<"Or(";
        orObj->getLeftExp()->accept(this);
        std::cout<<", ";        
        orObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitAdd(Add* addObj) override {
        std::cout<<"Add(";
        addObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        addObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitSub(Sub* subObj) override {
        std::cout<<"Sub(";
        subObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        subObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitMul(Mul* mulObj) override {
        std::cout<<"Mul(";
        mulObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        mulObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitDiv(Div* divObj) override {
        std::cout<<"Add(";
        divObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        divObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitMore(More* moreObj) override {
        std::cout<<"More(";
        moreObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        moreObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitMoreEqual(MoreEqual* moreEqualObj) override {
        std::cout<<"MoreEqual(";
        moreEqualObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        moreEqualObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitLess(Less* lessObj) override {
        std::cout<<"Less(";
        lessObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        lessObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitLessEqual(LessEqual* lessEqualObj) override {
        std::cout<<"LessEqual(";
        lessEqualObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        lessEqualObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitEqual(Equal* equalObj) override {  
        std::cout<<"Equal(";
        equalObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        equalObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }

    int visitNotEqual(NotEqual* notEqualObj) override {
        std::cout<<"NotEqual(";
        notEqualObj->getLeftExp()->accept(this); 
        std::cout<<", ";        
        notEqualObj->getRightExp()->accept(this);
        std::cout<<")";        
         return 0;
    }


    int visitConstant(  Constant* numObj) 
    {
        std::cout<<"Constant("<<numObj->getValue()<<")";
         return 0;
    }

    int visitAccess(Access* accessObj)
    {
        std::cout<<"Access(";
        accessObj->getId()->accept(this);
        std::cout<<",[";        
        accessObj->getIndex()->accept(this);
        std::cout<<"] )";        
         return 0;
    }

};


#endif
