//
//  MCAST.hpp
//  MiniC
//
//  Created by 梁志远 on 19/08/2017.
//  Copyright © 2017 Ogreaxe. All rights reserved.
//

#ifndef MCAST_hpp
#define MCAST_hpp

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <unordered_map>

using namespace std;

typedef string MCType;

//struct NCValueHolder{
//    
//};
//
//struct NCIntegerHolder:public NCValueHolder{
//    int value;
//};
//struct NCFloatHolder:public NCValueHolder{
//    float value;
//};
//struct NCStringHolder:public NCValueHolder{
//    string value;
//};


class NCASTNode {
public:
    NCASTNode(){}
    virtual ~NCASTNode(){}
};

typedef shared_ptr<NCASTNode> AstNodePtr;

//template<class T>
//class NCVariable:public NCASTNode{
//    T value;
//};

class NCExpression:public NCASTNode{
public:
    NCExpression(){}
};

//class MCVariableDeclarationExpression:public NCExpression{
//public:
//    string name;
//    NCExpression expression;
//};

class NCUnaryExpression:public NCExpression{
public:
    string op;
    shared_ptr<NCExpression> expression;
};

class NCBinaryExpression:public NCExpression{
public:
    NCBinaryExpression(){}
    
    string op;
    shared_ptr<NCExpression> left;
    shared_ptr<NCExpression> right;
};


//class NCPrimaryExpression:public NCExpression{
//public:
//    string op;
//    shared_ptr<NCExpression> left;
//    shared_ptr<NCExpression> right;
//};

class NCPrimaryPrefix:public NCExpression{
public:
    
};

class NCPrimarySuffix:public NCExpression{
public:
};

class NCAssignExpr:public NCExpression{
public:
    NCAssignExpr(shared_ptr<NCExpression> expr,string op,shared_ptr<NCExpression> value):expr(expr),op(op),value(value){}
    shared_ptr<NCExpression> expr;
    string op; //currently only "=" is supported
    shared_ptr<NCExpression> value;
};

//class.method(...)
class NCMethodCallExpr:public NCPrimarySuffix{
public:
    vector<shared_ptr<NCExpression>> args;
    string name;
    shared_ptr<NCExpression> scope;

    //method(args);
    NCMethodCallExpr(vector<shared_ptr<NCExpression>> & args,string &name):args(args), name(name), scope(nullptr){
    }
    
    //scope.method(args);
    NCMethodCallExpr(vector<shared_ptr<NCExpression>> & args, shared_ptr<NCExpression> scope, string &name):args(args), name(name), scope(scope){
    }
};

class NCFieldAccessExpr:public NCPrimarySuffix{
public:
    shared_ptr<NCExpression> scope;
    string field;

    NCFieldAccessExpr(shared_ptr<NCExpression> scope, string &field):field(field), scope(scope){};
};

class NCArrayAccessExpr:public NCPrimarySuffix{
public:
    NCArrayAccessExpr(shared_ptr<NCExpression> scope,shared_ptr<NCExpression> expression):scope(scope),expression(expression){};
    
    shared_ptr<NCExpression> scope;
    shared_ptr<NCExpression> expression;
};

class NCArrayInitializer:public NCExpression{
public:
    vector<shared_ptr<NCExpression>> elements;
};

class NCNameExpression:public NCExpression{
public:
    NCNameExpression(string&name):name(name){};
    string name;
};

class NCLiteral:public NCExpression{
public:
    virtual ~NCLiteral(){}
};

class NCIntegerLiteral:public NCLiteral{
    int value;
public:
    int getValue(){return value;};
//    NCIntegerLiteral(string & intStr);
    NCIntegerLiteral(int val):value(val){};
};

class NCFloatLiteral:public NCLiteral{
    
    float value;
public:
//    NCFloatLiteral(string & floatStr);
    
    int getValue(){return value;};
    NCFloatLiteral(float val):value(val){};
};

class NCStringLiteral:public NCLiteral{
    string str;
public:
    string getValue(){return str;};
    NCStringLiteral(string & str):str(str){};
};


class NCStatement:public NCASTNode{
    
};

class NCBlockStatement:public NCStatement{
public:
    shared_ptr<NCExpression> expression;
};

typedef NCBlockStatement  NCExpressionStatement;

class NCArrayBracketPair{
    
};

class VariableDeclarator:public NCExpression{
public:
    pair<string, vector<NCArrayBracketPair>> id;
    
    string id_str(){return id.first;}
    
    shared_ptr<NCExpression> expression;
};

class VariableDeclarationExpression:public NCExpression{
public:
    string type;
    vector<shared_ptr<NCExpression>> variables;
};

typedef VariableDeclarationExpression NCVariableDeclarationExpression;

class IfStatement:public NCStatement{
public:
    IfStatement():condition(nullptr),thenStatement(nullptr),elseStatement(nullptr){};
    
    IfStatement(shared_ptr<NCExpression> condition,shared_ptr<NCStatement> thenStatement):condition(condition),thenStatement(thenStatement),elseStatement(nullptr){};
    
    IfStatement(shared_ptr<NCExpression> condition, shared_ptr<NCStatement> thenStatement,shared_ptr<NCStatement> elseStatement):condition(condition),thenStatement(thenStatement),elseStatement(elseStatement){};
    
    shared_ptr<NCExpression> condition;
    shared_ptr<NCStatement> thenStatement;
    shared_ptr<NCStatement> elseStatement;
};

class ReturnStatement:public NCStatement{
public:
    ReturnStatement():expression(nullptr){}
    ReturnStatement(shared_ptr<NCExpression> expression):expression(expression){}
    shared_ptr<NCExpression> expression;
};

class WhileStatement:public NCStatement{
public:
    WhileStatement(shared_ptr<NCExpression> condition,shared_ptr<NCStatement> statement):condition(condition),statement(statement){}
    shared_ptr<NCExpression> condition;
    shared_ptr<NCStatement> statement;
};

class ForStatement:public NCStatement{
public:
    vector<shared_ptr<NCExpression>> init;
    vector<shared_ptr<NCExpression>> update;
    shared_ptr<NCExpression> expr;
    shared_ptr<NCStatement> body;

    ForStatement(){};
    ForStatement(vector<shared_ptr<NCExpression>> &init,
                 vector<shared_ptr<NCExpression>> &update,
                 shared_ptr<NCExpression> expr,
                 shared_ptr<NCStatement> body):update(update),init(init),expr(expr),body(body){}
};

class BreakStatement:public NCStatement{
};

class ContinueStatement:public NCStatement{
};

//class NCExpressionStatement:public NCStatement{
//    
//};
//
//class MCVariableExpressionStatement:public NCExpressionStatement{
//public:
//    string name;
//    NCExpression expression;
//};

//class NCASTWhile:public NCStatement{
//    AstNodePtr condition;
//    AstNodePtr body;
//};
//
//class NCASTReturn:public NCStatement{
//    AstNodePtr node;
//};
//
//class NCASTBranch:public NCStatement{
//    AstNodePtr condition;
//    AstNodePtr if_body;
//    AstNodePtr else_body;
//};
//
//class NCASTCompare:public NCASTNode{
//    AstNodePtr lnode;
//    AstNodePtr rnode;
//};
//
//class NCASTAssign:public NCASTNode{
//    AstNodePtr lnode;
//    AstNodePtr rnode;
//};
//
//class NCASTBinOp:public NCASTNode{
//    string op;
//    AstNodePtr lnode;
//    AstNodePtr rnode;
//};
//
//class NCASTUnaOp:public NCASTNode{
//    string op;
//    AstNodePtr lnode;
//};


class NCParameter{
public:
    NCParameter(){}
    
    NCParameter(string type,string name):type(type), name(name){}
    
    string type;
    string name;
};

class NCArgument{
    NCArgument();
    
    NCParameter parameter;
    
    void * value;
    
    void operator = (const void * value);
};

class NCBlock:public NCStatement{
public:
    vector<AstNodePtr> statement_list;
};
//
//class NCClassMember:public NCASTNode{
//public:
//    string type;
//    string name;
//    shared_ptr<NCExpression> expression;
//};
//
//class NCAstClassDefinition:public NCASTNode{
//public:
//    unordered_map<string, shared_ptr<NCClassMember>> memberMap;
//    
//};

class NCASTFunctionDefinition:public NCASTNode{
public:
    
    MCType return_type;
    
    string name;
    
    vector<NCParameter> parameters;
    
//    AstNodePtr statement_list;
    shared_ptr<NCBlock> block;
    
    AstNodePtr return_stat;
};

class NCASTRoot:public NCASTNode {
public:
    vector<shared_ptr<NCASTNode>> classList;
    vector<shared_ptr<NCASTNode>> functionList;
};

/////////////////////////////////////////////////////////
//
//      class definition
//
/////////////////////////////////////////////////////////

class NCBodyDeclaration:public NCASTNode {
    
};

class NCFieldDeclaration:public NCBodyDeclaration {
public:
    shared_ptr<NCExpression> declarator;
};

class NCMethodDeclaration:public NCBodyDeclaration {
public:
    shared_ptr<NCASTNode> method;
};

class NCConstructorDeclaration:public NCBodyDeclaration {
    
};

class NCClassDeclaration:public NCASTNode {
public:
    string name;
    
//    shared_ptr<NCClassDeclaration> parent;
    string parent;
    
    vector<shared_ptr<NCBodyDeclaration>> members;
};

#endif /* MCAST_hpp */
