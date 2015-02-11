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
#include <Cm.Core/InitSymbolTable.hpp>
#include <Cm.Parser/FileRegistry.hpp>
#include <Cm.Util/CodeFormatter.hpp>
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

void Module::Export(SymbolTable& symbolTable)
{
    Writer writer(filePath, &symbolTable);
    WriteModuleFileId(writer);
    WriteSourceFilePaths(writer);
    symbolTable.Export(writer);
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

void Module::ImportTo(SymbolTable& symbolTable)
{
    Reader reader(filePath, symbolTable);
    CheckModuleFileId(reader);
    ReadSourceFilePaths(reader);
    Cm::Parser::FileRegistry* fileRegistry = Cm::Parser::GetCurrentFileRegistry();
    int fileIndexOffset = 0;
    if (fileRegistry)
    {
        fileIndexOffset = fileRegistry->GetNumberOfParsedFiles();
        reader.SetSpanFileIndexOffset(fileIndexOffset);
        int n = int(sourceFilePaths.size());
        for (int i = 0; i < n; ++i)
        {
            fileRegistry->RegisterParsedFile(sourceFilePaths[i]);
        }
    }
    reader.MarkSymbolsBound();
    symbolTable.Import(reader);
}

void Module::Dump()
{
    SymbolTable symbolTable;
    Cm::Core::GlobalConceptData globalConceptData;
    Cm::Core::InitSymbolTable(symbolTable, globalConceptData);
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
    std::unique_ptr<Symbol> symbol(reader.ReadSymbol());
    if (symbol->IsNamespaceSymbol())
    {
        NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol.get());
        ns->Dump(formatter);
    }
    else
    {
        throw std::runtime_error("namespace symbol expected");
    }
    formatter.WriteLine("derived and template types:");
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTypeSymbol())
        {
            std::unique_ptr<TypeSymbol> typeSymbol(static_cast<TypeSymbol*>(symbol));
            typeSymbol->Dump(formatter);
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
    formatter.WriteLine("end of module code file");
    if (!reader.AllTypesFetched())
    {
        formatter.WriteLine("not all types fetched!");
    }
}

} } // namespace Cm::Sym
