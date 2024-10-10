#include "Node.h"
#include "Visitor.h"

 int Program::accept(Visitor* v)
{
    return v->visitProgram(this);
}

//Block
 int Block::accept(Visitor* v)
{
    return v->visitBlock(this);
}

//Decls
 int Decls::accept(Visitor* v) {
    return v->visitDecls(this);
}

 int Decl::accept(Visitor* v){
    return v->visitDecl(this);
}
//Stmt
 int Seq::accept(Visitor* v) {
    return v->visitSeq(this);
}

 int If::accept(Visitor* v)
{
    return v->visitIf(this);
}

 int Else::accept(Visitor* v)
{
    return v->visitElse(this);
}

 int While::accept(Visitor* v)
{
    return v->visitWhile(this);
}

 int Do::accept(Visitor* v)
{
    return v->visitDo(this);
}

 int Set::accept(Visitor* v)
{
    return v->visitSet(this);
}

 int SetElem::accept(Visitor* v)
{
    return v->visitSetElem(this);
}

 int Break::accept(Visitor* v)
{
    return v->visitBreak(this);
}

 int Print::accept(Visitor* v)
{
    return v->visitPrint(this);
}

//Logical
 int Not::accept(Visitor* v)
{
    return v->visitNot(this);
}
 int And::accept(Visitor* v)
{
    return v->visitAnd(this);
}

 int Or::accept(Visitor* v)
{
    return v->visitOr(this);
}

 int Add::accept(Visitor* v)
{
    return v->visitAdd(this);
}

 int Sub::accept(Visitor* v)
{
    return v->visitSub(this);
}

 int Mul::accept(Visitor* v)
{
    return v->visitMul(this);
}

 int Div::accept(Visitor* v)
{
    return v->visitDiv(this);
}

 int More::accept(Visitor* v)
{
    return v->visitMore(this);
}

 int MoreEqual::accept(Visitor* v)
{
    return v->visitMoreEqual(this);
}

 int Less::accept(Visitor* v)
{
    return v->visitLess(this);
}

 int LessEqual::accept(Visitor* v)
{
    return v->visitLessEqual(this);
}

 int Equal::accept(Visitor* v)
{
    return v->visitEqual(this);
}

 int NotEqual::accept(Visitor* v)
{
    return v->visitNotEqual(this);
}

 int NegOp::accept(Visitor* v)
{
    return v->visitNegOp(this);
}

 int Access::accept(Visitor* v)
{
    return v->visitAccess(this);
}

 int Id::accept(Visitor* v)
{
    return v->visitId(this);
}

//Type
 int Type::accept(Visitor* v) {
    return v->visitType(this);
}

 int Constant::accept(Visitor* v)
{
    return v->visitConstant(this);
}
