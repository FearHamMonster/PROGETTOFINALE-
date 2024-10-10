#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <map>
#include <utility>

#include "Node.h"
#include "MemoryManager.h"

//A struct is created to store additional information of an array object, such as size and type
struct constantStructInfo
{
  constantStructInfo(Type::TypeCode t, int v) : value{v}, typeCode{t}{}
  Type::TypeCode typeCode;
  int value;
};

struct arrayStructInfo
{
  arrayStructInfo( Type::TypeCode t, const int s) : typeCode{t}, size{s}
  {
    array = new int[size]; 
    hasBeenInitialized = new bool[size];

    //Individual cells are later declared, during assignment 
    for(int i=0; i<size; i++)
    {
      hasBeenInitialized[i] = false;
    }
  }
  
  //hasbeeninitialized serve per verificare se una cella di un vettore è stata già inizializzata
  //se v[i] è stata inizializzata, hasBeenInitialized[i]==true
  bool* hasBeenInitialized;  
  int* array;  
  const int size;
  Type::TypeCode typeCode;

};


//Variable space gestisce la memoria di tutti gli array e costanti inizializzate
//a runtime dal programma fornito in input 
class VariableSpace {
public:

  void declareConstant(std::string idName, Type::TypeCode tc){
    if(isIdDeclared(idName))
      return;
    
    //si inizializza una variabile appena creata a 0
    primitiveVariables.push_back(std::make_pair(idName, constantStructInfo{tc,0}));
  }         

  void declareArray(std::string idName, Type::TypeCode tc, int size){
    if(isIdDeclared(idName))
      return;

    arrayVariables.push_back(std::make_pair(idName, arrayStructInfo{tc,size}));
  }         

  bool isIdDeclared(std::string idName){
    for(int i = 0; i<primitiveVariables.size(); i++)
    {
      if(primitiveVariables[i].first == idName)
        return true;
    }
    for(int i = 0; i<arrayVariables.size(); i++)
    {
      if(arrayVariables[i].first == idName)
        return true;
    }
    return false;
  }

  int getIdValue(std::string idName){
    auto& id_iterator = getElement(idName);
    return id_iterator.second.value; 
  }

  Type::TypeCode getIdType(std::string idName){
    auto& id_iterator = getElement(idName);
    return id_iterator.second.typeCode; 
  }

  void assignToId(std::string idName, Type::TypeCode tc, int data)
  {
    auto& id_iterator = getElement(idName);
    checkTypes(tc,id_iterator.second.typeCode);
    
    id_iterator.second.value = data;
  }

  void assignToArray(std::string idName, Type::TypeCode tc, int data, int index){
    auto& id_iterator = getArrayElement(idName);
    checkOutOfBound(index, id_iterator.second.size);
    checkTypes(tc,id_iterator.second.typeCode);

    id_iterator.second.hasBeenInitialized[index] = true;
    id_iterator.second.array[index] = data;
  }

  Type::TypeCode getArrayType(std::string idName){
    auto& id_iterator = getArrayElement(idName);    
    return id_iterator.second.typeCode;
  }

  int  getArrayCell(std::string idName, int index){
    auto& id_iterator = getArrayElement(idName);
    checkOutOfBound(index, id_iterator.second.size);

    if(!id_iterator.second.hasBeenInitialized[index])
      throw EvaluationError("Non è stata ancora assegnato un valore alla cella del vettore a cui si vuole accedere");

    return id_iterator.second.array[index];
  }


private:


  std::pair<std::string, constantStructInfo>& getElement(std::string idName)
  {
    for(int i = 0; i<primitiveVariables.size(); i++)
    {
      if(primitiveVariables[i].first == idName)
        return primitiveVariables[i];
    }
    throw EvaluationError("Si cerca di accedere ad una variabile non dichiarata");
  }

  std::pair<std::string, arrayStructInfo>& getArrayElement(std::string idName)
  {
    for(int i = 0; i<arrayVariables.size(); i++)
    {
      if(arrayVariables[i].first == idName)
        return arrayVariables[i];
    }
    throw EvaluationError("Si cerca di accedere ad una variabile non dichiarata");
  }

  void checkOutOfBound(int index, int size)
  {
    if(index < 0 || index >= size)
      throw EvaluationError("Errore: out of bounds"); 
  }

  void checkTypes(Type::TypeCode t1, Type::TypeCode t2)
  {
    if(t1!=t2)
      throw EvaluationError("Mismatch di tipo tra variabile e rvalue"); 
  }
  //gli array e le variabili sono memorizzati rispettivamente in arrayVariables e primitiveVariables
  std::vector<std::pair<std::string,constantStructInfo>> primitiveVariables;
  std::vector<std::pair<std::string, arrayStructInfo>> arrayVariables;

};


#endif