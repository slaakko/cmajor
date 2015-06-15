/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Module.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/ExceptionTable.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/ClassCounter.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Cm.Util/Path.hpp>
#include <iostream>

namespace Cm { namespace Sym {

ModuleFileFormatError::ModuleFileFormatError(const std::string& filePath_) : std::runtime_error("library file '" + filePath_ + "' format changed, please rebuild associated project")
{
}

ModuleFileVersionMismatch::ModuleFileVersionMismatch(const std::string& readVersion_, const std::string& expectedVersion_) : 
    std::runtime_error("library file version mismatch: " + readVersion_ + " read, " + expectedVersion_ + " expected."), readVersion(readVersion_), expectedVersion(expectedVersion_)
{
}

const char moduleFileId[4] = { 'M', 'C', '1', '0' };

Module::Module(const std::string& filePath_) : filePath(filePath_)
{
}

void Module::SetSourceFilePaths(const std::vector<std::string>& sourceFilePaths_)
{
    sourceFilePaths = sourceFilePaths_;
}

void Module::SetReferenceFilePaths(const std::vector<std::string>& referenceFilePaths_)
{
    referenceFilePaths = referenceFilePaths_;
}

void Module::SetCLibraryFilePaths(const std::vector<std::string>& cLibraryFilePaths_)
{
    cLibraryFilePaths = cLibraryFilePaths_;
}

void Module::SetDebugInfoFilePaths(const std::vector<std::string>& debugInfoFilePaths_)
{
    debugInfoFilePaths = debugInfoFilePaths_;
}

void Module::WriteModuleFileId(Writer& writer)
{
    for (int i = 0; i < 4; ++i)
    {
        writer.GetBinaryWriter().Write(moduleFileId[i]);
    }
}

void Module::WriteSourceFilePaths(Writer& writer)
{
    int32_t n = int32_t(sourceFilePaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(sourceFilePaths[i]);
    }
}

void Module::WriteReferenceFilePaths(Writer& writer)
{
    int32_t n = int32_t(referenceFilePaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(referenceFilePaths[i]);
    }
}

void Module::WriteCLibraryFilePaths(Writer& writer)
{
    int32_t n = int32_t(cLibraryFilePaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(cLibraryFilePaths[i]);
    }
}

void Module::WriteDebugInfoFilePaths(Writer& writer)
{
    int32_t n = int32_t(debugInfoFilePaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(debugInfoFilePaths[i]);
    }
}

void Module::Export(SymbolTable& symbolTable)
{
    Writer writer(filePath, &symbolTable);
    WriteModuleFileId(writer);
    WriteSourceFilePaths(writer);
    WriteReferenceFilePaths(writer);
    WriteCLibraryFilePaths(writer);
    WriteDebugInfoFilePaths(writer);
    symbolTable.Export(writer);
    ExportExceptionTable(writer);
	writer.GetBinaryWriter().Write(int32_t(GetMutexTable()->GetNumberOfMutexesInThisProject()));
    writer.GetBinaryWriter().Write(GetClassCounter()->GetNumberOfClassesInThisProject());
}

void Module::CheckModuleFileId(Reader& reader)
{
    char readModuleFileId[4];
    try
    {
        for (int i = 0; i < 4; ++i)
        {
            readModuleFileId[i] = reader.GetBinaryReader().ReadChar();
        }
    }
    catch (...)
    {
        throw ModuleFileFormatError(filePath);
    }
    if (readModuleFileId[0] != moduleFileId[0] || readModuleFileId[1] != moduleFileId[1])
    {
        throw ModuleFileFormatError(filePath);
    }
    if (readModuleFileId[2] != moduleFileId[2] || readModuleFileId[3] != moduleFileId[3])
    {
        std::string readVersion(1, readModuleFileId[2]);
        readVersion.append(".").append(1, readModuleFileId[3]);
        std::string expectedVersion(1, moduleFileId[2]);
        expectedVersion.append(".").append(1, moduleFileId[3]);
        throw ModuleFileVersionMismatch(readVersion, expectedVersion);
    }
}

void Module::ReadSourceFilePaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string sourceFilePath = reader.GetBinaryReader().ReadString();
        sourceFilePaths.push_back(sourceFilePath);
    }
}

void Module::ReadReferenceFilePaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string referenceFilePath = reader.GetBinaryReader().ReadString();
        referenceFilePaths.push_back(referenceFilePath);
    }
}

void Module::ReadCLibraryFilePaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string cLibraryFilePath = reader.GetBinaryReader().ReadString();
        cLibraryFilePaths.push_back(cLibraryFilePath);
    }
}

void Module::ReadDebugInfoFilePaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string debugInfoFilePath = reader.GetBinaryReader().ReadString();
        debugInfoFilePaths.push_back(debugInfoFilePath);
    }
}

void Module::Import(SymbolTable& symbolTable, std::unordered_set<std::string>& importedModules, std::vector<std::string>& assemblyFilePaths, std::vector<std::string>& cLibs, 
    std::vector<std::string>& allReferenceFilePaths, std::vector<std::string>& allDebugInfoFilePaths)
{
    boost::filesystem::path afp = filePath;
    afp.replace_extension(".cma");
    assemblyFilePaths.push_back(Cm::Util::GetFullPath(afp.generic_string()));
    bool quiet = GetGlobalFlag(GlobalFlags::quiet);
    allReferenceFilePaths.push_back(filePath);
    Reader reader(filePath, symbolTable);
    CheckModuleFileId(reader);
    ReadSourceFilePaths(reader);
    ReadReferenceFilePaths(reader);
    ReadCLibraryFilePaths(reader);
    ReadDebugInfoFilePaths(reader);
    allDebugInfoFilePaths.insert(allDebugInfoFilePaths.end(), debugInfoFilePaths.begin(), debugInfoFilePaths.end());
    cLibs.insert(cLibs.end(), cLibraryFilePaths.begin(), cLibraryFilePaths.end());
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        if (importedModules.find(referenceFilePath) == importedModules.end())
        {
            importedModules.insert(referenceFilePath);
            Module referencedModule(referenceFilePath);
            referencedModule.Import(symbolTable, importedModules, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths);
        }
    }
    Cm::Parser::FileRegistry* fileRegistry = Cm::Parser::FileRegistry::Instance();
    int fileIndexOffset = 0;
    if (fileRegistry)
    {
        fileIndexOffset = fileRegistry->GetNumberOfParsedFiles();
        reader.SetSpanFileIndexOffset(fileIndexOffset);
        reader.GetAstReader().SetSpanFileIndexOffset(fileIndexOffset);
        int n = int(sourceFilePaths.size());
        for (int i = 0; i < n; ++i)
        {
            fileRegistry->RegisterParsedFile(sourceFilePaths[i]);
        }
    }
    reader.MarkSymbolsBound();
    symbolTable.Import(reader);
    ImportExceptionTable(symbolTable, reader);
	int numLibraryMutexes = reader.GetBinaryReader().ReadInt();
	GetMutexTable()->AddLibraryMutexes(numLibraryMutexes);
    uint32_t numLibraryClasses = reader.GetBinaryReader().ReadUInt();
    GetClassCounter()->AddLibryClasses(numLibraryClasses);
    if (!quiet)
    {
        std::cout << "> " << filePath << std::endl;
    }
}

void Module::ExportExceptionTable(Writer& writer)
{
    ExceptionTable* exceptionTable = GetExceptionTable();
    std::vector<Cm::Sym::TypeSymbol*> exceptionTypes = exceptionTable->GetProjectExceptions();
    int32_t n = int32_t(exceptionTypes.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        Cm::Sym::TypeSymbol* exceptionType = exceptionTypes[i];
        const TypeId& exceptionTypeId = exceptionType->Id();
        writer.Write(exceptionTypeId);
    }
}

void Module::ImportExceptionTable(SymbolTable& symbolTable, Reader& reader)
{
    ExceptionTable* exceptionTable = GetExceptionTable();
    int32_t numExceptionIds = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < numExceptionIds; ++i)
    {
        TypeId exceptionTypeId = reader.ReadTypeId();
        TypeSymbol* exceptionType = symbolTable.GetTypeRepository().GetType(exceptionTypeId);
        exceptionTable->AddLibraryException(exceptionType);
    }
}

void Module::Dump()
{
    Cm::Core::GlobalConceptData globalConceptData;
    Cm::Core::SetGlobalConceptData(&globalConceptData);
    SymbolTable symbolTable;
    Cm::Sym::SymbolTypeSetCollection symbolTypeSetCollection;
    Cm::Sym::SetSymbolTypeSetCollection(&symbolTypeSetCollection);
    Cm::Core::InitSymbolTable(symbolTable, globalConceptData);
    Cm::Sym::ExceptionTable exceptionTable;
    Cm::Sym::SetExceptionTable(&exceptionTable);
    Cm::Sym::MutexTable mutexTable;
    Cm::Sym::SetMutexTable(&mutexTable);
    Cm::Sym::ClassCounter classCounter;
    Cm::Sym::SetClassCounter(&classCounter);
    Reader reader(filePath, symbolTable);
    char readModuleFileId[4];
    try
    {
        for (int i = 0; i < 4; ++i)
        {
            readModuleFileId[i] = reader.GetBinaryReader().ReadChar();
        }
    }
    catch (...)
    {
        throw std::runtime_error("corrupted module code file '" + filePath + "'");
    }
    std::string readVersion(1, readModuleFileId[2]);
    readVersion.append(".").append(1, readModuleFileId[3]);
    std::string expectedVersion(1, moduleFileId[2]);
    expectedVersion.append(".").append(1, moduleFileId[3]);
    Cm::Util::CodeFormatter formatter(std::cout);
    formatter.IndentSize() = 1;
    formatter.WriteLine("module code file '" + filePath + "' version " + readVersion + " (version " + expectedVersion + " expected).");
    ReadSourceFilePaths(reader);
    ReadReferenceFilePaths(reader);
    ReadCLibraryFilePaths(reader);
    ReadDebugInfoFilePaths(reader);
    std::unordered_set<std::string> importedModules;
    std::vector<std::string> assemblyFilePaths;
    std::vector<std::string> cLibs;
    std::vector<std::string> allReferenceFilePaths;
    std::vector<std::string> allDebugInfoFilePaths;
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        if (importedModules.find(referenceFilePath) == importedModules.end())
        {
            importedModules.insert(referenceFilePath);
            Module referencedModule(referenceFilePath);
            referencedModule.Import(symbolTable, importedModules, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths);
        }
    }
    reader.MarkSymbolsProject();
    symbolTable.Import(reader, false);
    int32_t nt = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < nt; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTemplateTypeSymbol())
        {
            TemplateTypeSymbol* templateTypeSymbol = static_cast<TemplateTypeSymbol*>(symbol);
        }
        else
        {
            throw std::runtime_error("template type symbol expected");
        }
    }
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTypeSymbol())
        {
            TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(symbol);
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
    symbolTable.GlobalNs().Dump(formatter);
}

} } // namespace Cm::Sym
