/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Ser/BinaryReader.hpp>
#include <Cm.Emit/SourceFileCache.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <algorithm>
#include <iostream>

namespace Cm { namespace Debugger {

int numListLines = 10;
bool ide = false;

Breakpoint::Breakpoint() : cFileLine(), node(nullptr), number(-1)
{
}

Breakpoint::Breakpoint(const std::string& cFileLine_, Cm::Core::CfgNode* node_) : cFileLine(cFileLine_), node(node_), number(-1)
{
}

SourceFile::SourceFile(const std::string& filePath_) : filePath(filePath_), indexed(false), listLineNumber(0), activeLineNumber(0)
{
}

void SourceFile::Index()
{
    if (indexed) return;
    indexed = true;
    Cm::Emit::SourceFile& sourceFile = Cm::Emit::SourceFileCache::Instance().GetSourceFile(filePath);
    int state = 0;
    const char* e = sourceFile.End();
    std::string line;
    for (const char* p = sourceFile.Begin(); p != e; ++p)
    {
        char c = *p;
        if (c == '\n')
        {
            lines.push_back(line);
            line.clear();
        }
        else if (c != '\r')
        {
            line.append(1, c);
        }
    }
    lines.push_back(line);
}

Cm::Core::CfgNode* SourceFile::GetNode(int lineNumber) const
{
    NodeMapIt i = nodeMap.find(lineNumber);
    if (i != nodeMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void SourceFile::SetNode(int lineNumber, Cm::Core::CfgNode* node)
{
    nodeMap[lineNumber] = node;
}

void SourceFile::List(const BpNumBreakpointMap& breakpoints)
{
    Index();
    std::unordered_set<int> bpLineNumbers;
    for (const std::pair<int, const std::unique_ptr<Breakpoint>&>& p : breakpoints)
    {
        const std::unique_ptr<Breakpoint>& bp = p.second;
        if (!bp->Node())
        {
            throw std::runtime_error("breakpoint node not set");
        }
        if (!bp->Node()->Function())
        {
            throw std::runtime_error("breakpoint node function attribute not set");
        }
        if (bp->Node()->Function()->SourceFilePath() == filePath)
        {
            bpLineNumbers.insert(bp->Node()->GetSourceSpan().Line());
        }
    }
    if (listLineNumber == 0)
    {
        listLineNumber = activeLineNumber;
    }
    int startLineNumber = std::max(1, listLineNumber - numListLines / 2);
    listLineNumber = startLineNumber + numListLines / 2 + 1;
    int endLineNumber = std::min(int(lines.size()), startLineNumber + numListLines);
    if (startLineNumber > endLineNumber)
    {
        throw std::runtime_error("no more lines");
    }
    for (int lineNumber = startLineNumber; lineNumber <= endLineNumber; ++lineNumber)
    {
        int lineIndex = lineNumber - 1;
        std::string lineNumberString = std::to_string(lineNumber);
        std::string line;
        line.append(5 - int(lineNumberString.length()), ' ');
        line.append(lineNumberString);
        line.append(1, ' ');
        if (lineNumber == activeLineNumber)
        {
            line.append(1, '*');
        }
        else
        {
            line.append(1, ' ');
        }
        if (bpLineNumbers.find(lineNumber) != bpLineNumbers.end())
        {
            line.append(1, '!');
        }
        else
        {
            line.append(1, ' ');
        }
        line.append(1, ' ');
        line.append(lines[lineIndex]);
        std::cout << line << std::endl;
    }
    listLineNumber += numListLines;
}

void SourceFile::ListLine(int lineNumber, const BpNumBreakpointMap& breakpoints)
{
    Index();
    std::unordered_set<int> bpLineNumbers;
    for (const std::pair<int, const std::unique_ptr<Breakpoint>&>& p : breakpoints)
    {
        const std::unique_ptr<Breakpoint>& bp = p.second;
        if (!bp->Node())
        {
            throw std::runtime_error("breakpoint node not set");
        }
        if (!bp->Node()->Function())
        {
            throw std::runtime_error("breakpoint node function attribute not set");
        }
        if (bp->Node()->Function()->SourceFilePath() == filePath)
        {
            bpLineNumbers.insert(bp->Node()->GetSourceSpan().Line());
        }
    }
    int lineIndex = lineNumber - 1;
    std::string lineNumberString = std::to_string(lineNumber);
    std::string line;
    line.append(5 - int(lineNumberString.length()), ' ');
    line.append(lineNumberString);
    line.append(1, ' ');
    if (lineNumber == activeLineNumber)
    {
        line.append(1, '*');
    }
    else
    {
        line.append(1, ' ');
    }
    if (bpLineNumbers.find(lineNumber) != bpLineNumbers.end())
    {
        line.append(1, '!');
    }
    else
    {
        line.append(1, ' ');
    }
    line.append(1, ' ');
    line.append(lines[lineIndex]);
    std::cout << line << std::endl;
}

CFile::CFile(const std::string& cFilePath_) : cFilePath(cFilePath_)
{
}

Cm::Core::CfgNode* CFile::GetNode(int lineNumber) const
{
    NodeMapIt i = nodeMap.find(lineNumber);
    if (i != nodeMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void CFile::SetNode(int lineNumber, Cm::Core::CfgNode* node)
{
    nodeMap[lineNumber] = node;
}

Cm::Core::CFunCall* CFile::GetFunCallReturn(int returnLineNumber) const
{
    FunCallMapIt i = funCallMap.find(returnLineNumber);
    if (i != funCallMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void CFile::SetFunCallReturn(int returnLineNumber, Cm::Core::CFunCall* funCall)
{
    funCallMap[returnLineNumber] = funCall;
}

DebugInfo::DebugInfo(const std::string& debugInfoFilePath): state(State::idle), mainFunctionDebugInfo(nullptr), currentNode(nullptr), nextBreakpointNumber(0), currentSourceFile(nullptr)
{
    Cm::Ser::BinaryReader reader(debugInfoFilePath);
    int32_t n = reader.ReadInt();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::Core::CDebugInfoFile> file(new Cm::Core::CDebugInfoFile());
        file->Read(reader);
        Process(file.get());
        files.push_back(std::move(file));
    }
    if (!mainFunctionDebugInfo)
    {
        throw std::runtime_error("debug info file '" + debugInfoFilePath + "' contains no debug info for main function");
    }
}

void DebugInfo::SetState(State state_)
{
    state = state_;
    if (state == State::idle)
    {
        currentNode = nullptr;
        currentSourceFile = nullptr;
    }
}

bool DebugInfo::HasCBreakpoint(const std::string& cFileLine) const
{ 
    return cFileLineBreakpointMap.find(cFileLine) != cFileLineBreakpointMap.end();
}

Breakpoint* DebugInfo::GetBreakpoint(const std::string& cFileLine) const
{
    CFileLineBreakpointMapIt i = cFileLineBreakpointMap.find(cFileLine);
    if (i != cFileLineBreakpointMap.end())
    {
        return i->second;
    }
    return nullptr;
}

Breakpoint* DebugInfo::GetBreakpoint(int bpNum) const
{
    BpNumBreakpointMapIt i = bpNumBreakpointMap.find(bpNum);
    if (i != bpNumBreakpointMap.end())
    {
        return i->second.get();
    }
    return nullptr;
}

int DebugInfo::SetBreakpoint(Breakpoint* bp)
{
    int bpNum = ++nextBreakpointNumber;
    bp->SetNumber(bpNum);
    bpNumBreakpointMap[bpNum] = std::unique_ptr<Breakpoint>(bp);
    cFileLineBreakpointMap[bp->CFileLine()] = bp;
    return bpNum;
}

void DebugInfo::SetBreakpoint(const std::string& cFileLine, Breakpoint* bp)
{
    cFileLineBreakpointMap[cFileLine] = bp;
}

void DebugInfo::RemoveBreakpoint(int bpNum)
{
    Breakpoint* bp = GetBreakpoint(bpNum);
    if (bp)
    {
        std::string cFileLine = bp->CFileLine();
        bpNumBreakpointMap.erase(bpNum);
        cFileLineBreakpointMap.erase(cFileLine);
    }
    else
    {
        throw std::runtime_error("breakpoint number " + std::to_string(bpNum) + " not found");
    }
}

void DebugInfo::RemoveBreakpoint(const std::string& cFileLine)
{
    cFileLineBreakpointMap.erase(cFileLine);
}

Cm::Core::CfgNode* DebugInfo::GetNode(const Cm::Core::SourceFileLine& sourceFileLine) const
{
    if (sourceFileLine.SourceFilePath().empty())
    {
        if (currentNode)
        {
            if (!currentNode->Function())
            {
                throw std::runtime_error("function attribute of current node not set");
            }
            Cm::Core::CfgNode* node = currentNode->Function()->Cfg().FindNode(sourceFileLine.SourceLineNumber());
            if (node)
            {
                return node;
            }
            else
            {
                throw std::runtime_error("node for line number " + std::to_string(sourceFileLine.SourceLineNumber()) + " not found in current function");
            }
        }
        else
        {
            throw std::runtime_error("current node not set, please specify source file also");
        }
    }
    SourceFile* sourceFile = GetSourceFile(sourceFileLine.SourceFilePath());
    Cm::Core::CfgNode* node = sourceFile->GetNode(sourceFileLine.SourceLineNumber());
    if (node)
    {
        return node;
    }
    else
    {
        throw std::runtime_error("node for line number " + std::to_string(sourceFileLine.SourceLineNumber()) + " not found in source file '" + sourceFile->FilePath() + "'");
    }
}

Cm::Core::CfgNode* DebugInfo::GetNodeByCFileLine(const Cm::Core::SourceFileLine& cFileLine) const
{
    return GetNodeByCFileLine(cFileLine, true);
}

Cm::Core::CfgNode* DebugInfo::GetNodeByCFileLine(const Cm::Core::SourceFileLine& cFileLine, bool throw_) const
{
    CFile* cFile = GetCFile(cFileLine.SourceFilePath());
    if (!cFile)
    {
        if (throw_)
        {
            throw std::runtime_error("C file '" + cFileLine.SourceFilePath() + "' not found");
        }
        return nullptr;
    }
    Cm::Core::CfgNode* node = cFile->GetNode(cFileLine.SourceLineNumber());
    if (!node)
    {
        if (throw_)
        {
            throw std::runtime_error("C node '" + cFileLine.ToString() + "' not found");
        }
    }
    return node;
}

Cm::Core::CFunCall* DebugInfo::GetFunCallByCFileLine(const Cm::Core::SourceFileLine& cFileLine) const
{
    return GetFunCallByCFileLine(cFileLine, true);
}

Cm::Core::CFunCall* DebugInfo::GetFunCallByCFileLine(const Cm::Core::SourceFileLine& cFileLine, bool throw_) const
{
    CFile* cFile = GetCFile(cFileLine.SourceFilePath());
    if (!cFile)
    {
        if (throw_)
        {
            throw std::runtime_error("C file '" + cFileLine.SourceFilePath() + "' not found");
        }
        return nullptr;
    }
    Cm::Core::CFunCall* funCall = cFile->GetFunCallReturn(cFileLine.SourceLineNumber());
    if (!funCall)
    {
        if (throw_)
        {
            throw std::runtime_error("C function call '" + cFileLine.ToString() + "' not found");
        }
    }
    return funCall;
}

Cm::Core::CFunctionDebugInfo* DebugInfo::GetFunctionDebugInfo(const std::string& mangledFunctionName) const
{
    if (!currentNode)
    {
        throw std::runtime_error("current node not set");
    }
    if (!currentNode->Function())
    {
        throw std::runtime_error("function attribute of current node not set");
    }
    Cm::Core::CFunctionDebugInfo* currentFunctionDebugInfo = currentNode->Function();
    if (!currentFunctionDebugInfo->File())
    {
        throw std::runtime_error("file attribute set of current function debug info not set");
    }
    Cm::Core::CFunctionDebugInfo* functionDebugInfo = currentFunctionDebugInfo->File()->GetFunctionDebugInfo(mangledFunctionName);
    if (!functionDebugInfo)
    {
        UniqueFunctionDebugInfoMapIt i = uniqueFunctionDebugInfoMap.find(mangledFunctionName);
        if (i != uniqueFunctionDebugInfoMap.cend())
        {
            functionDebugInfo = i->second;
        }
    }
    return functionDebugInfo;
}

SourceFile* DebugInfo::GetSourceFile(const std::string& filePath) const
{
    return GetSourceFile(filePath, true);
}

SourceFile* DebugInfo::GetSourceFile(const std::string& filePath, bool throw_) const
{
    SourceFileMapIt i = sourceFileMap.find(filePath);
    if (i != sourceFileMap.end())
    {
        return i->second;
    }
    std::vector<SourceFile*> matchingFiles;
    for (const std::unique_ptr<SourceFile>& sourceFile : sourceFiles)
    {
        if (Cm::Util::LastComponentsEqual(filePath, sourceFile->FilePath(), '/'))
        {
            matchingFiles.push_back(sourceFile.get());
        }
    }
    if (matchingFiles.empty())
    {
        if (throw_)
        {
            throw std::runtime_error("no matching source files found");
        }
    }
    else if (matchingFiles.size() == 1)
    {
        return matchingFiles[0];
    }
    else
    {
        if (throw_)
        {
            throw std::runtime_error("more than one source files match, please specify more path");
        }
    }
    return nullptr;
}

void DebugInfo::RemoveThrowNode(Cm::Core::CfgNode* throwNode)
{
    throwNodes.erase(throwNode);
}

void DebugInfo::SetThrowCFileLines(const std::vector<std::string>& throwCFileLines_)
{
    throwCFileLines = throwCFileLines_;
}

void DebugInfo::RemoveCatchNode(Cm::Core::CfgNode* catchNode)
{
    catchNodes.erase(catchNode);
}

void DebugInfo::SetSourceFile(const std::string& filePath, SourceFile* sourceFile)
{
    sourceFileMap[filePath] = sourceFile;
    sourceFiles.push_back(std::unique_ptr<SourceFile>(sourceFile));
}

CFile* DebugInfo::GetCFile(const std::string& cFilePath) const
{
    CFileMapIt i = cFileMap.find(cFilePath);
    if (i != cFileMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void DebugInfo::SetCFile(const std::string& cFilePath, CFile* cFile)
{
    cFileMap[cFilePath] = cFile;
    cFiles.push_back(std::unique_ptr<CFile>(cFile));
}

void DebugInfo::Process(Cm::Core::CDebugInfoFile* file)
{
    for (const std::unique_ptr<Cm::Core::CFunctionDebugInfo>& functionDebugInfo : file->FunctionDebugInfos())
    {
        functionDebugInfo->SetFile(file);
        SourceFile* sourceFile = GetSourceFile(functionDebugInfo->SourceFilePath(), false);
        if (!sourceFile)
        {
            sourceFile = new SourceFile(functionDebugInfo->SourceFilePath());
            SetSourceFile(sourceFile->FilePath(), sourceFile);
        }
        const std::string& cFilePath = functionDebugInfo->CFilePath();
        CFile* cFile = GetCFile(cFilePath);
        if (!cFile)
        {
            cFile = new CFile(cFilePath);
            SetCFile(cFilePath, cFile);
        }
        Cm::Core::ControlFlowGraph& cfg = functionDebugInfo->Cfg();
        for (const std::unique_ptr<Cm::Core::CfgNode>& node : cfg.Nodes())
        {
            node->SetFunction(functionDebugInfo.get());
            switch (node->Kind())
            {
                case Cm::Core::CfgNodeKind::exitNode:
                {
                    cfg.AddExit(node.get());
                    break;
                }
                case Cm::Core::CfgNodeKind::throwNode:
                {
                    throwNodes.insert(node.get());
                    break;
                }
                case Cm::Core::CfgNodeKind::catchNode:
                {
                    catchNodes.insert(node.get());
                    break;
                }
            }
            Cm::Core::CfgNode* cfgNode = cFile->GetNode(node->CLine());
            if (!cfgNode)
            {
                cFile->SetNode(node->CLine(), node.get());
            }
            else 
            {
                int x = 0;
            }
            int n = int(node->CFunCalls().size());
            for (int i = 0; i < n; ++i)
            {
                Cm::Core::CFunCall* cFunCall = node->CFunCalls()[i].get();
                cFunCall->SetIndex(i);
                cFunCall->SetNode(node.get());
                int32_t funCallReturnLine = cFunCall->ReturnCLine();
                Cm::Core::CFunCall* prev = cFile->GetFunCallReturn(funCallReturnLine);
                if (!prev)
                {
                    cFile->SetFunCallReturn(funCallReturnLine, cFunCall);
                }
                else
                {
                    int x = 0;
                }
            }
        }
        if (functionDebugInfo->IsMain())
        {
            mainFunctionDebugInfo = functionDebugInfo.get();
        }
        if (functionDebugInfo->IsUnique())
        {
            uniqueFunctionDebugInfoMap[functionDebugInfo->MangledFunctionName()] = functionDebugInfo.get();
            SourceFile* sourceFile = GetSourceFile(functionDebugInfo->SourceFilePath(), false);
            if (!sourceFile)
            {
                sourceFile = new SourceFile(functionDebugInfo->SourceFilePath());
                SetSourceFile(sourceFile->FilePath(), sourceFile);
            }
            for (const std::unique_ptr<Cm::Core::CfgNode>& node : cfg.Nodes())
            {
                Cm::Core::CfgNode* prev = sourceFile->GetNode(node->GetSourceSpan().Line());
                if (!prev)
                {
                    sourceFile->SetNode(node->GetSourceSpan().Line(), node.get());
                }
            }
        }
        else
        {
            file->AddFunctionDebugInfoToMap(functionDebugInfo.get());
        }
    }
}

} } // Cm::Debugger
