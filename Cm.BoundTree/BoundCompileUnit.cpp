/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

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

void BoundCompileUnit::Write(Cm::Sym::BcuWriter& writer)
{
    writer.GetBinaryWriter().Write(irFilePath);
    if (irFilePath == "C:/Programming/cmajorbin/system/full/llvm/mt.ll")
    {
        int x = 0;
    }
    writer.GetBinaryWriter().Write(objectFilePath);
    writer.GetBinaryWriter().Write(optIrFilePath);
    writer.GetBinaryWriter().Write(dependencyFilePath);
    writer.GetBinaryWriter().Write(changedFilePath);
    writer.GetBinaryWriter().Write(cDebugInfoFilePath);
    writer.GetBinaryWriter().Write(bcuPath);
    stringRepository->Write(writer);
    irClassTypeRepository->Write(writer);
    functionTemplateRepository.Write(writer);
    classTemplateRepository->Write(writer);
    synthesizedClassFunRepository->Write(writer);
    inlineFunctionRepository->Write(writer);
    writer.GetBinaryWriter().Write(int(boundNodes.size()));
    int i = 0;
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        writer.Write(boundNode.get());
        ++i;
    }
}

void BoundCompileUnit::Read(Cm::Sym::BcuReader& reader)
{
    reader.SetBoundCompileUnit(this);
    irFilePath = reader.GetBinaryReader().ReadString();
    if (irFilePath == "C:/Programming/cmajorbin/system/full/llvm/mt.ll")
    {
        int x = 0;
    }
    objectFilePath = reader.GetBinaryReader().ReadString();
    optIrFilePath = reader.GetBinaryReader().ReadString();
    dependencyFilePath = reader.GetBinaryReader().ReadString();
    changedFilePath = reader.GetBinaryReader().ReadString();
    cDebugInfoFilePath = reader.GetBinaryReader().ReadString();
    bcuPath = reader.GetBinaryReader().ReadString();
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
    irClassTypeRepository->Read(reader);
    functionTemplateRepository.Read(reader);
    classTemplateRepository->Read(reader);
    synthesizedClassFunRepository->Read(reader);
    inlineFunctionRepository->Read(reader);
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::BcuItem* item = reader.ReadItem();
        if (item->IsBoundNode())
        {
            BoundNode* node = static_cast<BoundNode*>(item);
            boundNodes.push_back(std::unique_ptr<BoundNode>(node));
        }
        else
        {
            throw std::runtime_error("bound node expected");
        }
    }
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
    ownedFunctionSymbols.push_back(std::unique_ptr<Cm::Sym::FunctionSymbol>(functionSymbol));
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
