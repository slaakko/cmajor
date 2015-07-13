/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ArrayTypeOpRepository.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/StatementBinder.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Ast/BasicType.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

class PrimitiveArrayTypeDefaultConstructor : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeDefaultConstructor() const override { return true; }
};

PrimitiveArrayTypeDefaultConstructor::PrimitiveArrayTypeDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    ComputeName();
}

void PrimitiveArrayTypeDefaultConstructor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* zero = Ir::Intf::GetFactory()->GetI8()->CreateDefaultValue();
    emitter.Own(zero);
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemSet(arrayObject, zero, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemSet(dest, zero, len, 1, false));
    }
}

class PrimitiveArrayTypeCopyConstructor : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeCopyConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeCopyConstructorOrCopyAssignment() const override { return true; }
};

PrimitiveArrayTypeCopyConstructor::PrimitiveArrayTypeCopyConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("@constructor");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void PrimitiveArrayTypeCopyConstructor::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
        arg1->SetType(ptrArrayType);
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemCopy(arrayObject, arg1, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        Ir::Intf::Object* source = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(source);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, source, arg1, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemCopy(dest, source, len, 1, false));
    }
}

class PrimitiveArrayTypeCopyAssignment : public Cm::Core::BasicTypeOp
{
public:
    PrimitiveArrayTypeCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    bool IsPrimitiveArrayTypeCopyConstructorOrCopyAssignment() const override { return true; }
};

PrimitiveArrayTypeCopyAssignment::PrimitiveArrayTypeCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator=");
    Cm::Sym::ParameterSymbol* thisParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this"));
    thisParam->SetType(typeRepository.MakePointerType(Type(), Cm::Parsing::Span()));
    AddSymbol(thisParam);
    Cm::Sym::ParameterSymbol* thatParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "that"));
    thatParam->SetType(typeRepository.MakeConstReferenceType(Type(), Cm::Parsing::Span()));
    AddSymbol(thatParam);
    ComputeName();
}

void PrimitiveArrayTypeCopyAssignment::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    Ir::Intf::Object* dim = Cm::IrIntf::CreateI64Constant(Type()->GetLastArrayDimension());
    emitter.Own(dim);
    Ir::Intf::Object* size = Cm::IrIntf::SizeOf(emitter, Type()->GetBaseType()->GetIrType());
    emitter.Own(size);
    Ir::Intf::Object* len = Cm::IrIntf::CreateTemporaryRegVar(Ir::Intf::GetFactory()->GetI64());
    emitter.Own(len);
    emitter.Emit(Cm::IrIntf::Mul(Ir::Intf::GetFactory()->GetI64(), len, dim, size));
    Ir::Intf::Object* arrayObject = result.MainObject();
    Ir::Intf::Object* arg1 = result.Arg1();
    Ir::Intf::Type* arrayObjectType = arrayObject->GetType();
    if (result.MemberVar())
    {
        Ir::Intf::Type* arrayType = Cm::IrIntf::Array(Type()->GetBaseType()->GetIrType(), Type()->GetLastArrayDimension());
        emitter.Own(arrayType);
        Ir::Intf::Type* ptrArrayType = Cm::IrIntf::Pointer(arrayType, 1);
        emitter.Own(ptrArrayType);
        arrayObject->SetType(ptrArrayType);
        arrayObjectType = ptrArrayType;
        arg1->SetType(ptrArrayType);
    }
    if (arrayObjectType->Name() == "i8*")
    {
        emitter.Emit(Cm::IrIntf::MemCopy(arrayObject, arg1, len, 1, false));
    }
    else
    {
        Ir::Intf::Type* i8Ptr = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI8(), 1);
        emitter.Own(i8Ptr);
        Ir::Intf::Object* dest = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(dest);
        Ir::Intf::Object* source = Cm::IrIntf::CreateTemporaryRegVar(i8Ptr);
        emitter.Own(source);
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, dest, arrayObject, i8Ptr));
        emitter.Emit(Cm::IrIntf::Bitcast(arrayObjectType, source, arg1, i8Ptr));
        emitter.Emit(Cm::IrIntf::MemCopy(dest, source, len, 1, false));
    }
}

Cm::Sym::FunctionSymbol* GenerateArrayTypeDefaultConstructor(Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    int n = arrayType->GetLastArrayDimension();
    Cm::Sym::TypeSymbol* elementType = arrayType->GetBaseType();
    Cm::Sym::TypeSymbol* ptrType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(elementType, Cm::Parsing::Span());
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "this");
    thisParam->SetType(ptrType);
    Cm::Sym::FunctionSymbol* defaultConstructorSymbol = new Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "@array_constructor");
    defaultConstructorSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    defaultConstructorSymbol->SetGroupName("@array_constructor_" + std::to_string(n));
    defaultConstructorSymbol->SetParent(containerScope->Ns());
    defaultConstructorSymbol->GetContainerScope()->SetParent(containerScope->Ns()->GetContainerScope());
    defaultConstructorSymbol->SetConstructorOrDestructorSymbol();
    defaultConstructorSymbol->SetMemberFunctionSymbol();
    defaultConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    defaultConstructorSymbol->SetReplicated();
    defaultConstructorSymbol->SetArrayConstructor();
    defaultConstructorSymbol->AddSymbol(thisParam);
    defaultConstructorSymbol->ComputeName();
    std::unique_ptr<Cm::BoundTree::BoundFunction> defaultConstructor(new Cm::BoundTree::BoundFunction(nullptr, defaultConstructorSymbol));
    Cm::Ast::CompoundStatementNode* body = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    defaultConstructor->Own(body);
    Cm::Ast::ConstructionStatementNode* constructLoopVarStatement = new Cm::Ast::ConstructionStatementNode(Cm::Parsing::Span(), new Cm::Ast::IntNode(Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"));
    constructLoopVarStatement->AddArgument(new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), n));
    body->AddStatement(constructLoopVarStatement);
    Cm::Ast::CompoundStatementNode* whileBlockContent = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    Cm::Ast::ConstructNode* constructNode = new Cm::Ast::ConstructNode(Cm::Parsing::Span(), MakeTypeIdNode(elementType, Cm::Parsing::Span()));
    constructNode->AddArgument(new Cm::Ast::ThisNode(Cm::Parsing::Span()));
    constructNode->AddArgument(new Cm::Ast::InvokeNode(Cm::Parsing::Span(), MakeTypeIdNode(elementType, Cm::Parsing::Span())));
    Cm::Ast::SimpleStatementNode* constructStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), constructNode);
    whileBlockContent->AddStatement(constructStatement);
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixIncNode(Cm::Parsing::Span(), new Cm::Ast::ThisNode(Cm::Parsing::Span()))));;
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixDecNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"))));
    Cm::Ast::WhileStatementNode* whileStatement = new Cm::Ast::WhileStatementNode(Cm::Parsing::Span(),
        new Cm::Ast::GreaterNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"), new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), 0)), whileBlockContent);
    body->AddStatement(whileStatement);
    Cm::Sym::DeclarationVisitor declarationVisitor(compileUnit.SymbolTable());
    body->Accept(declarationVisitor);
    Binder binder(compileUnit);
    binder.SetCurrentFunction(defaultConstructor.release());
    body->Accept(binder);
    Cm::BoundTree::BoundFunction* defaultCtor = binder.ReleaseCurrentFunction();
    GenerateReceives(containerScope, compileUnit, defaultCtor);
    compileUnit.AddBoundNode(defaultCtor);
    return defaultConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateArrayTypeCopyConstructor(Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    int n = arrayType->GetLastArrayDimension();
    Cm::Sym::TypeSymbol* elementType = arrayType->GetBaseType();
    Cm::Sym::FunctionSymbol* copyConstructorSymbol = new Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "@array_constructor");
    copyConstructorSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    copyConstructorSymbol->SetGroupName("@array_constructor_" + std::to_string(n));
    copyConstructorSymbol->SetParent(containerScope->Ns());
    copyConstructorSymbol->GetContainerScope()->SetParent(containerScope->Ns()->GetContainerScope());
    copyConstructorSymbol->SetConstructorOrDestructorSymbol();
    copyConstructorSymbol->SetMemberFunctionSymbol();
    copyConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    copyConstructorSymbol->SetReplicated();
    copyConstructorSymbol->SetArrayConstructor();
    std::unique_ptr<Cm::BoundTree::BoundFunction> copyConstructor(new Cm::BoundTree::BoundFunction(nullptr, copyConstructorSymbol));
    Cm::Ast::CompoundStatementNode* body = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    copyConstructor->Own(body);
    Cm::Ast::ConstructionStatementNode* constructLoopVarStatement = new Cm::Ast::ConstructionStatementNode(Cm::Parsing::Span(), new Cm::Ast::IntNode(Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"));
    constructLoopVarStatement->AddArgument(new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), n));
    body->AddStatement(constructLoopVarStatement);
    Cm::Ast::CompoundStatementNode* whileBlockContent = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    Cm::Ast::ConstructNode* constructNode = new Cm::Ast::ConstructNode(Cm::Parsing::Span(), MakeTypeIdNode(elementType, Cm::Parsing::Span()));
    constructNode->AddArgument(new Cm::Ast::ThisNode(Cm::Parsing::Span()));
    Cm::Ast::InvokeNode* invokeNode = new Cm::Ast::InvokeNode(Cm::Parsing::Span(), MakeTypeIdNode(elementType, Cm::Parsing::Span()));
    invokeNode->AddArgument(new Cm::Ast::DerefNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that")));
    constructNode->AddArgument(invokeNode);
    Cm::Ast::SimpleStatementNode* constructStatement = new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), constructNode);
    whileBlockContent->AddStatement(constructStatement);
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixIncNode(Cm::Parsing::Span(), new Cm::Ast::ThisNode(Cm::Parsing::Span()))));;
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixIncNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that"))));
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixDecNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"))));
    Cm::Ast::WhileStatementNode* whileStatement = new Cm::Ast::WhileStatementNode(Cm::Parsing::Span(),
        new Cm::Ast::GreaterNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"), new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), 0)), whileBlockContent);
    body->AddStatement(whileStatement);
    Cm::Sym::DeclarationVisitor declarationVisitor(compileUnit.SymbolTable());
    compileUnit.SymbolTable().BeginContainer(copyConstructorSymbol);
    Cm::Ast::ParameterNode* thisParamNode = new Cm::Ast::ParameterNode(Cm::Parsing::Span(), MakeTypeIdNode(compileUnit.SymbolTable().GetTypeRepository().MakePointerType(elementType, Cm::Parsing::Span()),
        Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "this"));
    copyConstructor->Own(thisParamNode);
    thisParamNode->Accept(declarationVisitor);
    Cm::Ast::ParameterNode* thatParamNode = new Cm::Ast::ParameterNode(Cm::Parsing::Span(), MakeTypeIdNode(compileUnit.SymbolTable().GetTypeRepository().MakePointerType(elementType, Cm::Parsing::Span()),
        Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that"));
    copyConstructor->Own(thatParamNode);
    thatParamNode->Accept(declarationVisitor);
    body->Accept(declarationVisitor);
    compileUnit.SymbolTable().EndContainer();
    Prebinder prebinder(compileUnit.SymbolTable(), compileUnit.ClassTemplateRepository());
    prebinder.BeginContainerScope(copyConstructorSymbol->GetContainerScope());
    thisParamNode->Accept(prebinder);
    thatParamNode->Accept(prebinder);
    prebinder.EndContainerScope();
    Binder binder(compileUnit);
    binder.SetCurrentFunction(copyConstructor.release());
    body->Accept(binder);
    Cm::BoundTree::BoundFunction* copyCtor = binder.ReleaseCurrentFunction();
    copyConstructorSymbol->ComputeName();
    GenerateReceives(containerScope, compileUnit, copyCtor);
    compileUnit.AddBoundNode(copyCtor);
    return copyConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateArrayTypeCopyAssignment(Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    int n = arrayType->GetLastArrayDimension();
    Cm::Sym::TypeSymbol* elementType = arrayType->GetBaseType();
    Cm::Sym::FunctionSymbol* assignmentSymbol = new Cm::Sym::FunctionSymbol(Cm::Parsing::Span(), "@array_assignment");
    assignmentSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    assignmentSymbol->SetGroupName("@array_assignment_" + std::to_string(n));
    assignmentSymbol->SetParent(containerScope->Ns());
    assignmentSymbol->GetContainerScope()->SetParent(containerScope->Ns()->GetContainerScope());
    assignmentSymbol->SetConstructorOrDestructorSymbol();
    assignmentSymbol->SetMemberFunctionSymbol();
    assignmentSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    assignmentSymbol->SetReplicated();
    assignmentSymbol->SetArrayAssignment();
    assignmentSymbol->SetReturnType(compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId)));
    std::unique_ptr<Cm::BoundTree::BoundFunction> assignment(new Cm::BoundTree::BoundFunction(nullptr, assignmentSymbol));
    Cm::Ast::CompoundStatementNode* body = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    assignment->Own(body);
    Cm::Ast::ConstructionStatementNode* constructLoopVarStatement = new Cm::Ast::ConstructionStatementNode(Cm::Parsing::Span(), new Cm::Ast::IntNode(Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"));
    constructLoopVarStatement->AddArgument(new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), n));
    body->AddStatement(constructLoopVarStatement);
    Cm::Ast::CompoundStatementNode* whileBlockContent = new Cm::Ast::CompoundStatementNode(Cm::Parsing::Span());
    Cm::Ast::AssignmentStatementNode* assignmentStatement = new Cm::Ast::AssignmentStatementNode(Cm::Parsing::Span(),
        new Cm::Ast::DerefNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "this")),
        new Cm::Ast::DerefNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that")));
    whileBlockContent->AddStatement(assignmentStatement);
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixIncNode(Cm::Parsing::Span(), new Cm::Ast::ThisNode(Cm::Parsing::Span()))));;
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixIncNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that"))));
    whileBlockContent->AddStatement(new Cm::Ast::SimpleStatementNode(Cm::Parsing::Span(), new Cm::Ast::PrefixDecNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"))));
    Cm::Ast::WhileStatementNode* whileStatement = new Cm::Ast::WhileStatementNode(Cm::Parsing::Span(),
        new Cm::Ast::GreaterNode(Cm::Parsing::Span(), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "i"), new Cm::Ast::IntLiteralNode(Cm::Parsing::Span(), 0)), whileBlockContent);
    body->AddStatement(whileStatement);
    Cm::Sym::DeclarationVisitor declarationVisitor(compileUnit.SymbolTable());
    compileUnit.SymbolTable().BeginContainer(assignmentSymbol);
    Cm::Ast::ParameterNode* thisParamNode = new Cm::Ast::ParameterNode(Cm::Parsing::Span(), MakeTypeIdNode(compileUnit.SymbolTable().GetTypeRepository().MakePointerType(elementType, Cm::Parsing::Span()),
        Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "this"));
    assignment->Own(thisParamNode);
    thisParamNode->Accept(declarationVisitor);
    Cm::Ast::ParameterNode* thatParamNode = new Cm::Ast::ParameterNode(Cm::Parsing::Span(), MakeTypeIdNode(compileUnit.SymbolTable().GetTypeRepository().MakePointerType(elementType, Cm::Parsing::Span()),
        Cm::Parsing::Span()), new Cm::Ast::IdentifierNode(Cm::Parsing::Span(), "that"));
    assignment->Own(thatParamNode);
    thatParamNode->Accept(declarationVisitor);
    body->Accept(declarationVisitor);
    compileUnit.SymbolTable().EndContainer();
    Prebinder prebinder(compileUnit.SymbolTable(), compileUnit.ClassTemplateRepository());
    prebinder.BeginContainerScope(assignmentSymbol->GetContainerScope());
    thisParamNode->Accept(prebinder);
    thatParamNode->Accept(prebinder);
    prebinder.EndContainerScope();
    Binder binder(compileUnit);
    binder.SetCurrentFunction(assignment.release());
    body->Accept(binder);
    Cm::BoundTree::BoundFunction* copyAssignment = binder.ReleaseCurrentFunction();
    assignmentSymbol->ComputeName();
    GenerateReceives(containerScope, compileUnit, copyAssignment);
    compileUnit.AddBoundNode(copyAssignment);
    return assignmentSymbol;
}

class ArrayIndexing : public Cm::Core::BasicTypeOp
{
public:
    ArrayIndexing(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
};

ArrayIndexing::ArrayIndexing(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_) : BasicTypeOp(type_)
{
    SetGroupName("operator[]");
    SetReturnType(typeRepository.MakeReferenceType(Type()->GetBaseType(), Cm::Parsing::Span()));
    Cm::Sym::ParameterSymbol* arrayParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "array"));
    arrayParam->SetType(Type());
    AddSymbol(arrayParam);
    Cm::Sym::ParameterSymbol* indexParam(new Cm::Sym::ParameterSymbol(Cm::Parsing::Span(), "index"));
    indexParam->SetType(typeRepository.GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId)));
    AddSymbol(indexParam);
    ComputeName();
}

void ArrayIndexing::Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        Ir::Intf::Object* zero = Cm::IrIntf::CreateI32Constant(0);
        emitter.Own(zero);
        if (result.Arg1()->IsStackVar())
        {
            Ir::Intf::Type* arrayPtrType = Cm::IrIntf::Pointer(Type()->GetBaseType()->GetIrType(), 1);
            emitter.Own(arrayPtrType);
            emitter.Emit(Cm::IrIntf::GetElementPtr(arrayPtrType, result.MainObject(), result.Arg1(), result.Arg2()));
        }
        else
        {
            Ir::Intf::Type* arrayPtrType = Cm::IrIntf::Pointer(Type()->GetIrType(), 1);
            emitter.Own(arrayPtrType);
            emitter.Emit(Cm::IrIntf::GetElementPtr(arrayPtrType, result.MainObject(), result.Arg1(), zero, result.Arg2()));
        }
    }
    else
    {
        emitter.Emit(Cm::IrIntf::Index(result.MainObject(), result.Arg1(), result.Arg2()));
    }
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetDefaultConstructor(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type)
{
    if (!defaultConstructor)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            defaultConstructor.reset(new PrimitiveArrayTypeDefaultConstructor(compileUnit.SymbolTable().GetTypeRepository(), type));
        }
        else
        {
            defaultConstructor.reset(GenerateArrayTypeDefaultConstructor(type, containerScope, compileUnit));
        }
    }
    return defaultConstructor.get();
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetCopyConstructor(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type)
{
    if (!copyConstructor)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            copyConstructor.reset(new PrimitiveArrayTypeCopyConstructor(compileUnit.SymbolTable().GetTypeRepository(), type));
        }
        else
        {
            copyConstructor.reset(GenerateArrayTypeCopyConstructor(type, containerScope, compileUnit));
        }
    }
    return copyConstructor.get();
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetCopyAssignment(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type)
{
    if (!copyAssignment)
    {
        if (type->IsPrimitiveSingleDimensionArrayType())
        {
            copyAssignment.reset(new PrimitiveArrayTypeCopyAssignment(compileUnit.SymbolTable().GetTypeRepository(), type));
        }
        else
        {
            copyAssignment.reset(GenerateArrayTypeCopyAssignment(type, containerScope, compileUnit));
        }
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* ArrayTypeOpCache::GetIndexing(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* arrayType)
{
    if (!indexing)
    {
        indexing.reset(new ArrayIndexing(compileUnit.SymbolTable().GetTypeRepository(), arrayType));
    }
    return indexing.get();
}

ArrayTypeOpFunGroup::~ArrayTypeOpFunGroup()
{
}

void ArrayTypeConstructorGroup::CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 1)
    {
        ArrayTypeOpCache& cache = cacheMap[arrayType];
        Cm::Sym::FunctionSymbol* defaultConstructor = cache.GetDefaultConstructor(containerScope, boundCompileUnit, arrayType);
        if (defaultConstructor)
        {
            viableFunctions.insert(defaultConstructor);
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* plainArg1Type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[1].Type());
        if (Cm::Sym::TypesEqual(arrayType, plainArg1Type))
        {
            ArrayTypeOpCache& cache = cacheMap[arrayType];
            Cm::Sym::FunctionSymbol* copyConstructor = cache.GetCopyConstructor(containerScope, boundCompileUnit, arrayType);
            if (copyConstructor)
            {
                viableFunctions.insert(copyConstructor);
            }
        }
    }
}

void ArrayTypeAssignmentGroup::CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) 
{
    if (arity == 2)
    {
        Cm::Sym::TypeSymbol* plainArg1Type = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[1].Type());
        if (Cm::Sym::TypesEqual(arrayType, plainArg1Type))
        {
            ArrayTypeOpCache& cache = cacheMap[arrayType];
            Cm::Sym::FunctionSymbol* copyAssignment = cache.GetCopyAssignment(containerScope, boundCompileUnit, arrayType);
            if (copyAssignment)
            {
                viableFunctions.insert(copyAssignment);
            }
        }
    }
}

void ArrayTypeIndexGroup::CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit,
    const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity == 2)
    {
        Cm::Sym::TypeSymbol* indexType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePlainType(arguments[1].Type());
        if (indexType->IsIntegerTypeSymbol())
        {
            ArrayTypeOpCache& cache = cacheMap[arrayType];
            viableFunctions.insert(cache.GetIndexing(containerScope, boundCompileUnit, arrayType));
        }
    }
}

ArrayTypeOpRepository::ArrayTypeOpRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_) : compileUnit(compileUnit_)
{
    arrayTypeOpFunGroupMap["@constructor"] = &arrayTypeConstructorGroup;
    arrayTypeOpFunGroupMap["operator="] = &arrayTypeAssignmentGroup;
    arrayTypeOpFunGroupMap["operator[]"] = &arrayTypeIndexGroup;
}

void ArrayTypeOpRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ContainerScope* containerScope, 
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (arity < 1 || arity > 2) return;
    Cm::Sym::TypeSymbol* leftArgType = arguments[0].Type();
    Cm::Sym::TypeSymbol* arrayType = nullptr;
    if (groupName != "operator[]")
    {
        if (!leftArgType->IsArrayType() || leftArgType->IsReferenceType() || leftArgType->IsRvalueRefType() || !leftArgType->IsPointerType() || leftArgType->GetPointerCountAfterArray() != 1) return;
        Cm::Sym::DerivedTypeSymbol* ptrArrayType = static_cast<Cm::Sym::DerivedTypeSymbol*>(leftArgType);
        if (!Cm::Sym::HasPointerToArrayDerivation(ptrArrayType->Derivations())) return;
        Cm::Ast::DerivationList derivations = ptrArrayType->Derivations();
        derivations.RemoveLastPointer();
        arrayType = compileUnit.SymbolTable().GetTypeRepository().MakeDerivedType(derivations, ptrArrayType->GetBaseType(), ptrArrayType->GetArrayDimensions(), span);
    }
    else
    {
        if (!leftArgType->IsArrayType()) return;
        arrayType = leftArgType;
    }
    ArrayTypeOpFunGroupMapIt i = arrayTypeOpFunGroupMap.find(groupName);
    if (i != arrayTypeOpFunGroupMap.end())
    {
        ArrayTypeOpFunGroup* group = i->second;
        group->CollectViableFunctions(cacheMap, arrayType, containerScope, compileUnit, span, arity, arguments, viableFunctions);
    }
}

} } // namespace Cm::Bind
