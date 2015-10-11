/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace Cm { namespace BoundTree {

CompileUnitMap::~CompileUnitMap()
{
}

CompileUnitMap* globalCompileUnitMap = nullptr;

void SetCompileUnitMap(CompileUnitMap* compileUnitMap)
{
    globalCompileUnitMap = compileUnitMap;
}

CompileUnitMap* GetCompileUnitMap()
{
    return globalCompileUnitMap;
}

BoundCompileUnit::BoundCompileUnit(Cm::Sym::SymbolTable& symbolTable_) : syntaxUnit(nullptr), symbolTable(symbolTable_), conversionTable(symbolTable.GetStandardConversionTable()), 
    classConversionTable(symbolTable.GetTypeRepository()), derivedTypeOpRepository(symbolTable.GetTypeRepository()), enumTypeOpRepository(symbolTable.GetTypeRepository()),
    irFunctionRepository(), hasGotos(false), isPrebindCompileUnit(false), isMainUnit(false), functionTemplateRepository(symbolTable)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        externalConstantRepository.reset(new Cm::Core::LlvmExternalConstantRepository());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        externalConstantRepository.reset(new Cm::Core::CExternalConstantRepository());
    }
}

BoundCompileUnit::BoundCompileUnit(Cm::Ast::CompileUnitNode* syntaxUnit_, const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_) : syntaxUnit(syntaxUnit_),
    fileScopes(), irFilePath(irFilePath_), symbolTable(symbolTable_), conversionTable(symbolTable.GetStandardConversionTable()), classConversionTable(symbolTable.GetTypeRepository()), 
    derivedTypeOpRepository(symbolTable.GetTypeRepository()), enumTypeOpRepository(symbolTable.GetTypeRepository()), irFunctionRepository(), hasGotos(false), isPrebindCompileUnit(false), 
    isMainUnit(false), functionTemplateRepository(symbolTable)
{
    objectFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".o").generic_string());
    optIrFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".opt.ll").generic_string());
    dependencyFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".dep").generic_string());
    changedFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".chg").generic_string());
    cDebugInfoFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".cdi").generic_string());
    bcuPath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".bcu").generic_string());
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        stringRepository.reset(new Cm::Core::LlvmStringRepository());
        irClassTypeRepository.reset(new Cm::Core::LlvmIrClassTypeRepository());
        externalConstantRepository.reset(new Cm::Core::LlvmExternalConstantRepository());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        stringRepository.reset(new Cm::Core::CStringRepository());
        irClassTypeRepository.reset(new Cm::Core::CIrClassTypeRepository());
        externalConstantRepository.reset(new Cm::Core::CExternalConstantRepository());
    }
    else
    {
        throw std::runtime_error("backend not set");
    }
}

void BoundCompileUnit::SetPaths(const std::string& basePath)
{
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        irFilePath = Cm::Util::GetFullPath(basePath + ".ll");
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        irFilePath = Cm::Util::GetFullPath(basePath + ".c");
    }
    objectFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".o").generic_string());
    optIrFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".opt.ll").generic_string());
}

void BoundCompileUnit::SetProjectName(const std::string& projectName_)
{
    projectName = projectName_;
}

void BoundCompileUnit::SetFileName(const std::string& fileName_)
{
    fileName = fileName_;
}

void BoundCompileUnit::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetBinaryWriter().Write(irFilePath);
    writer.GetBinaryWriter().Write(objectFilePath);
    writer.GetBinaryWriter().Write(optIrFilePath);
    writer.GetBinaryWriter().Write(dependencyFilePath);
    writer.GetBinaryWriter().Write(changedFilePath);
    writer.GetBinaryWriter().Write(cDebugInfoFilePath);
    writer.GetBinaryWriter().Write(bcuPath);
    writer.GetBinaryWriter().Write(fileName);
    writer.GetBinaryWriter().Write(projectName);
    stringRepository->Write(writer);
    classTemplateRepository->Write(writer);
    functionTemplateRepository.Write(writer);
    irClassTypeRepository->Write(writer);
    synthesizedClassFunRepository->Write(writer);
    inlineFunctionRepository->Write(writer);
    writer.GetBinaryWriter().Write(int(boundNodes.size()));
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        writer.Write(boundNode.get());
    }
}

void BoundCompileUnit::Read(Cm::Sym::BcuReader& reader)
{
    reader.SetBoundCompileUnit(this);
    irFilePath = reader.GetBinaryReader().ReadString();
    objectFilePath = reader.GetBinaryReader().ReadString();
    optIrFilePath = reader.GetBinaryReader().ReadString();
    dependencyFilePath = reader.GetBinaryReader().ReadString();
    changedFilePath = reader.GetBinaryReader().ReadString();
    cDebugInfoFilePath = reader.GetBinaryReader().ReadString();
    bcuPath = reader.GetBinaryReader().ReadString();
    fileName = reader.GetBinaryReader().ReadString();
    projectName = reader.GetBinaryReader().ReadString();
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        stringRepository.reset(new Cm::Core::LlvmStringRepository());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        stringRepository.reset(new Cm::Core::CStringRepository());
    }
    stringRepository->Read(reader);
    if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::llvm)
    {
        irClassTypeRepository.reset(new Cm::Core::LlvmIrClassTypeRepository());
    }
    else if (Cm::IrIntf::GetBackEnd() == Cm::IrIntf::BackEnd::c)
    {
        irClassTypeRepository.reset(new Cm::Core::CIrClassTypeRepository());
    }
    classTemplateRepository->Read(reader);
    functionTemplateRepository.Read(reader);
    irClassTypeRepository->Read(reader);
    synthesizedClassFunRepository->Read(reader);
    inlineFunctionRepository->Read(reader);
    int n = reader.GetBinaryReader().ReadInt();
    std::vector<Cm::Bind::ClassDelegateEqualOp*> classDelegateEqualOps;
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::BcuItem* item = reader.ReadItem();
        if (item->IsBoundNode())
        {
            BoundNode* node = static_cast<BoundNode*>(item);
            if (node->IsBoundFunctionNode())
            {
                BoundFunction* boundFunction = static_cast<BoundFunction*>(node);
                Cm::Sym::FunctionSymbol* functionSymbol = boundFunction->GetFunctionSymbol();
                if (functionSymbol->GroupName() == "main")
                {
                    symbolTable.SetUserMainFunction(functionSymbol);
                }
                if (functionSymbol->IsClassDelegateEqualOp())
                {
                    classDelegateEqualOps.push_back(static_cast<Cm::Bind::ClassDelegateEqualOp*>(functionSymbol));
                    delete node;
                }
                else
                {
                    boundNodes.push_back(std::unique_ptr<BoundNode>(node));
                }
            }
            else
            {
                boundNodes.push_back(std::unique_ptr<BoundNode>(node));
            }
        }
        else
        {
            throw std::runtime_error("bound node expected");
        }
    }
    for (Cm::Bind::ClassDelegateEqualOp* equalOp : classDelegateEqualOps)
    {
        equalOp->CreateBoundNode(equalOp->Ns()->GetContainerScope(), *this);
    }
    if (!reader.GetSymbolReader().AllTypesFetched())
    {
        std::cout << bcuPath << ": not all types fetched!" << std::endl;
    }
    reader.GetSymbolReader().MakeIrTypes();
    reader.GetSymbolReader().InitVTables();
}

void BoundCompileUnit::AddFileScope(Cm::Sym::FileScope* fileScope_)
{
    fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope_));
}

void BoundCompileUnit::RemoveLastFileScope()
{
    fileScopes.pop_back();
}

void BoundCompileUnit::AddBoundNode(BoundNode* boundNode)
{
    boundNodes.push_back(std::unique_ptr<BoundNode>(boundNode));
}

void BoundCompileUnit::Own(Cm::Sym::FunctionSymbol* functionSymbol)
{
    if (!functionSymbol->Owned())
    {
        functionSymbol->SetOwned();
        ownedFunctionSymbols.push_back(std::unique_ptr<Cm::Sym::FunctionSymbol>(functionSymbol));
    }
}

bool BoundCompileUnit::Instantiated(Cm::Sym::FunctionSymbol* functionSymbol) const
{
    return instantiatedFunctions.find(functionSymbol) != instantiatedFunctions.end();
}

void BoundCompileUnit::AddToInstantiated(Cm::Sym::FunctionSymbol* functionSymbol)
{
    instantiatedFunctions.insert(functionSymbol);
}

void BoundCompileUnit::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        boundNode->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundCompileUnit::SetSynthesizedClassFunRepository(Cm::Core::SynthesizedClassFunRepository* synthesizedClassFunRepository_)
{
    synthesizedClassFunRepository.reset(synthesizedClassFunRepository_);
}

void BoundCompileUnit::SetClassTemplateRepository(Cm::Core::ClassTemplateRepository* classTemplateRepository_)
{
    classTemplateRepository.reset(classTemplateRepository_);
}

void BoundCompileUnit::SetInlineFunctionRepository(Cm::Core::InlineFunctionRepository* inlineFunctionRepository_)
{
    inlineFunctionRepository.reset(inlineFunctionRepository_);
}

void BoundCompileUnit::SetDelegateTypeOpRepository(Cm::Core::DelegateTypeOpRepository* delegateTypeOpRepository_)
{
    delegateTypeOpRepository.reset(delegateTypeOpRepository_);
}

void BoundCompileUnit::SetClassDelegateTypeOpRepository(Cm::Core::ClassDelegateTypeOpRepository* classDelegateTypeOpRepository_)
{
    classDelegateTypeOpRepository.reset(classDelegateTypeOpRepository_);
}

void BoundCompileUnit::SetArrayTypeOpRepository(Cm::Core::ArrayTypeOpRepository* arrayTypeOpRepository_)
{
    arrayTypeOpRepository.reset(arrayTypeOpRepository_);
}

bool BoundCompileUnit::Changed() const
{
    boost::filesystem::path sfp = syntaxUnit->FilePath();
    boost::filesystem::path irp = irFilePath;
    boost::filesystem::path ofp = objectFilePath;
    if (!boost::filesystem::exists(irp)) return true;
    if (boost::filesystem::last_write_time(sfp) > boost::filesystem::last_write_time(irp)) return true;
    if (!boost::filesystem::exists(ofp)) return true;
    if (boost::filesystem::last_write_time(sfp) > boost::filesystem::last_write_time(ofp)) return true;
    if (boost::filesystem::last_write_time(irp) > boost::filesystem::last_write_time(ofp)) return true;
    return false;
}

void BoundCompileUnit::AddDependentUnit(Cm::BoundTree::BoundCompileUnit* dependentUnit)
{
    dependentUnits.insert(dependentUnit);
}

void BoundCompileUnit::ReadDependencyFile()
{
    if (boost::filesystem::exists(dependencyFilePath))
    {
        std::ifstream dependencyFile(dependencyFilePath);
        std::string dependentFilePath;
        while (std::getline(dependencyFile, dependentFilePath))
        {
            BoundCompileUnit* dependentUnit = GetCompileUnitMap()->GetBoundCompileUnit(dependentFilePath);
            if (dependentUnit)
            {
                AddDependentUnit(dependentUnit);
            }
        }
    }
}

void BoundCompileUnit::WriteDependencyFile()
{
    std::ofstream dependencyFile(dependencyFilePath);
    for (Cm::BoundTree::BoundCompileUnit* dependentUnit : dependentUnits)
    {
        dependencyFile << dependentUnit->SyntaxUnit()->FilePath() << std::endl;
    }
}

void BoundCompileUnit::WriteChangedFile()
{
    std::ofstream changedFile(changedFilePath);
    changedFile << "changed" << std::endl;
}

bool BoundCompileUnit::HasChangedFile() const
{
    return boost::filesystem::exists(changedFilePath);
}

void BoundCompileUnit::RemoveChangedFile()
{
    boost::filesystem::remove(changedFilePath);
}

} } // namespace Cm::BoundTree
