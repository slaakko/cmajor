/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Sym/Warning.hpp>
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Cm.Util/Path.hpp>
#include <iostream>

namespace Cm { namespace Sym {

ModuleFileFormatError::ModuleFileFormatError(const std::string& filePath_) : std::runtime_error("library file '" + filePath_ + "' format changed, please rebuild associated project")
{
}

ModuleFileVersionMismatch::ModuleFileVersionMismatch(const std::string& libaryFilePath, const std::string& readVersion_, const std::string& expectedVersion_) :
    std::runtime_error("library file (" + libaryFilePath + ") version mismatch: " + readVersion_ + " read, " + expectedVersion_ + " expected, please rebuild."),
    readVersion(readVersion_), expectedVersion(expectedVersion_)
{
}

const char moduleFileId[4] = { 'M', 'C', '1', '3' };

Module::Module(const std::string& filePath_) : filePath(filePath_)
{
}

void Module::SetName(const std::string& name_)
{
    name = name_;
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

void Module::SetLibrarySearchPaths(const std::vector<std::string>& librarySearchPaths_)
{
    librarySearchPaths = librarySearchPaths_;
}

void Module::SetDebugInfoFilePaths(const std::vector<std::string>& debugInfoFilePaths_)
{
    debugInfoFilePaths = debugInfoFilePaths_;
}

void Module::SetNativeObjectFilePaths(const std::vector<std::string>& nativeObjectFilePaths_)
{
    nativeObjectFilePaths = nativeObjectFilePaths_;
}

void Module::SetBcuPaths(const std::vector<std::string>& bcuPaths_)
{
    bcuPaths = bcuPaths_;
}

void Module::WriteModuleFileId(Writer& writer)
{
    for (int i = 0; i < 4; ++i)
    {
        writer.GetBinaryWriter().Write(moduleFileId[i]);
    }
}

void Module::WriteName(Writer& writer)
{
    writer.GetBinaryWriter().Write(name);
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

void Module::WriteLibrarySearchPaths(Writer& writer)
{
    int32_t n = int32_t(librarySearchPaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(librarySearchPaths[i]);
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

void Module::WriteNativeObjectFilePaths(Writer& writer)
{
    int32_t n = int32_t(nativeObjectFilePaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(nativeObjectFilePaths[i]);
    }
}

void Module::WriteBcuPaths(Writer& writer)
{
    int32_t n = int32_t(bcuPaths.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(bcuPaths[i]);
    }
}

void Module::Export(SymbolTable& symbolTable)
{
    Writer writer(filePath, &symbolTable);
    WriteModuleFileId(writer);
    WriteName(writer);
    WriteSourceFilePaths(writer);
    WriteReferenceFilePaths(writer);
    WriteCLibraryFilePaths(writer);
    WriteLibrarySearchPaths(writer);
    WriteDebugInfoFilePaths(writer);
    WriteNativeObjectFilePaths(writer);
    WriteBcuPaths(writer);
    symbolTable.Export(writer);
    ExportExceptionTable(writer);
	writer.GetBinaryWriter().Write(int32_t(GetMutexTable()->GetNumberOfMutexesInThisProject()));
    std::vector<uint64_t> classHierarchyTable;
    for (ClassTypeSymbol* projectClass : symbolTable.ProjectClasses())
    {
        if (projectClass->IsVirtual())
        {
            classHierarchyTable.push_back(projectClass->Cid());
            if (projectClass->BaseClass())
            {
                classHierarchyTable.push_back(projectClass->BaseClass()->Cid());
            }
            else
            {
                classHierarchyTable.push_back(noCid);
            }
        }
    }
    int n = int(classHierarchyTable.size());
    writer.GetBinaryWriter().Write(n);
    for (int i = 0; i < n; ++i)
    {
        writer.GetBinaryWriter().Write(classHierarchyTable[i]);
    }
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
        throw ModuleFileVersionMismatch(filePath, readVersion, expectedVersion);
    }
}

void Module::ReadName(Reader& reader)
{
    name = reader.GetBinaryReader().ReadString();
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

void Module::ReadLibrarySearchPaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string librarySearchPath = reader.GetBinaryReader().ReadString();
        librarySearchPaths.push_back(librarySearchPath);
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

void Module::ReadNativeObjectFilePaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string nativeObjectFilePath = reader.GetBinaryReader().ReadString();
        nativeObjectFilePaths.push_back(nativeObjectFilePath);
    }
}

void Module::ReadBcuPaths(Reader& reader)
{
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        std::string bcuPath = reader.GetBinaryReader().ReadString();
        bcuPaths.push_back(bcuPath);
    }
}

void Module::Import(SymbolTable& symbolTable, std::unordered_set<std::string>& importedModules, std::vector<std::string>& assemblyFilePaths, std::vector<std::string>& cLibs, 
    std::vector<std::string>& allReferenceFilePaths, std::vector<std::string>& allDebugInfoFilePaths, std::vector<std::string>& allNativeObjectFilePaths, std::vector<std::string>& allBcuPaths, 
    std::vector<uint64_t>& classHierarchyTable, std::vector<std::string>& allLibrarySearchPaths)
{
    static int recursionCount = 0;
    ++recursionCount;
    boost::filesystem::path afp = filePath;
    afp.replace_extension(".cma");
    assemblyFilePaths.push_back(Cm::Util::GetFullPath(afp.generic_string()));
    bool quiet = GetGlobalFlag(GlobalFlags::quiet);
    allReferenceFilePaths.push_back(filePath);
    Reader reader(filePath, symbolTable);
    CheckModuleFileId(reader);
    ReadName(reader);
    ReadSourceFilePaths(reader);
    ReadReferenceFilePaths(reader);
    ReadCLibraryFilePaths(reader);
    ReadLibrarySearchPaths(reader);
    ReadDebugInfoFilePaths(reader);
    ReadNativeObjectFilePaths(reader);
    ReadBcuPaths(reader);
    allDebugInfoFilePaths.insert(allDebugInfoFilePaths.end(), debugInfoFilePaths.begin(), debugInfoFilePaths.end());
    allNativeObjectFilePaths.insert(allNativeObjectFilePaths.end(), nativeObjectFilePaths.begin(), nativeObjectFilePaths.end());
    allBcuPaths.insert(allBcuPaths.end(), bcuPaths.begin(), bcuPaths.end());
    cLibs.insert(cLibs.end(), cLibraryFilePaths.begin(), cLibraryFilePaths.end());
    allLibrarySearchPaths.insert(allLibrarySearchPaths.end(), librarySearchPaths.begin(), librarySearchPaths.end());
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        if (importedModules.find(referenceFilePath) == importedModules.end())
        {
            importedModules.insert(referenceFilePath);
            Module referencedModule(referenceFilePath);
            referencedModule.Import(symbolTable, importedModules, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths, allNativeObjectFilePaths, allBcuPaths, classHierarchyTable, allLibrarySearchPaths);
            referencedModule.CheckUpToDate();
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
    if (recursionCount == 1 && GetGlobalFlag(GlobalFlags::generate_docs))
    {
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
        return;
    }
    symbolTable.Import(reader);
    ImportExceptionTable(symbolTable, reader);
	int numLibraryMutexes = reader.GetBinaryReader().ReadInt();
	GetMutexTable()->AddLibraryMutexes(numLibraryMutexes);
    int numEntries = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < numEntries; ++i)
    {
        uint64_t cid = reader.GetBinaryReader().ReadULong();
        classHierarchyTable.push_back(cid);
    }
    if (!quiet)
    {
        std::cout << "> " << filePath << std::endl;
    }
    --recursionCount;
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
    ReadName(reader);
    ReadSourceFilePaths(reader);
    ReadReferenceFilePaths(reader);
    ReadCLibraryFilePaths(reader);
    ReadLibrarySearchPaths(reader);
    ReadDebugInfoFilePaths(reader);
    ReadNativeObjectFilePaths(reader);
    ReadBcuPaths(reader);
    std::unordered_set<std::string> importedModules;
    std::vector<std::string> assemblyFilePaths;
    std::vector<std::string> cLibs;
    std::vector<std::string> allReferenceFilePaths;
    std::vector<std::string> allDebugInfoFilePaths;
    std::vector<std::string> allNativeObjectFilePaths;
    std::vector<std::string> allBcuPaths;
    std::vector<uint64_t> classHierarchyTable;
    std::vector<std::string> allLibrarySearchPaths;
    for (const std::string& referenceFilePath : referenceFilePaths)
    {
        if (importedModules.find(referenceFilePath) == importedModules.end())
        {
            importedModules.insert(referenceFilePath);
            Module referencedModule(referenceFilePath);
            referencedModule.Import(symbolTable, importedModules, assemblyFilePaths, cLibs, allReferenceFilePaths, allDebugInfoFilePaths, allNativeObjectFilePaths, allBcuPaths, classHierarchyTable, allLibrarySearchPaths);
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

void Module::CheckFileVersion()
{
    SymbolTable symbolTable;
    Reader reader(filePath, symbolTable);
    CheckModuleFileId(reader);
}

void Module::CheckUpToDate()
{
    boost::filesystem::path mfp = filePath;
    bool changed = false;
    if (!boost::filesystem::exists(mfp))
    {
        std::string warningMessage = "library '" + name + "' is not up-to-date, because library file '" + filePath + "' does not exist";
        std::cout << "warning: " << warningMessage << std::endl;
        CompileWarningCollection::Instance().AddWarning(Warning(CompileWarningCollection::Instance().GetCurrentProjectName(), warningMessage));
        changed = true;
    }
    else
    {
        for (const std::string& sourceFilePath : sourceFilePaths)
        {
            boost::filesystem::path sfp = sourceFilePath;
            if (boost::filesystem::last_write_time(sfp) > boost::filesystem::last_write_time(mfp))
            {
                std::string warningMessage = "library '" + name + "' is not up-to-date, because source file '" + sourceFilePath + "' is modified after library file '" + filePath + "'";
                std::cout << "warning: " << warningMessage << std::endl;
                CompileWarningCollection::Instance().AddWarning(Warning(CompileWarningCollection::Instance().GetCurrentProjectName(), warningMessage));
                changed = true; 
                break;
            }
        }
    }
    if (!changed)
    {
        for (const std::string& referenceFilePath : referenceFilePaths)
        {
            boost::filesystem::path rfp = referenceFilePath;
            if (boost::filesystem::last_write_time(rfp) > boost::filesystem::last_write_time(mfp))
            {
                std::string warningMessage = "library '" + name + "' is not up-to-date, because referenced library file '" + referenceFilePath + " is modified after library file '" + filePath + "'";
                std::cout << "warning: " << warningMessage << std::endl;
                CompileWarningCollection::Instance().AddWarning(Warning(CompileWarningCollection::Instance().GetCurrentProjectName(), warningMessage));
            }
        }
    }
}

} } // namespace Cm::Sym
