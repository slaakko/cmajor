/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_DEBUG_INFO_INCLUDED
#define CM_DEBUGGER_DEBUG_INFO_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <map>

namespace Cm { namespace Debugger {

enum class State
{
    debugging, idle, exit
};

extern int numListLines;
extern bool ide;

class Breakpoint
{
public:
    Breakpoint();
    Breakpoint(const std::string& cFileLine_, Cm::Core::CfgNode* node_);
    const std::string& CFileLine() const { return cFileLine; }
    Cm::Core::CfgNode* Node() const { return node; }
    int Number() const { return number; }
    void SetNumber(int number_) { number = number_; }
private:
    std::string cFileLine;
    Cm::Core::CfgNode* node;
    int number;
};

typedef std::map<int, std::unique_ptr<Breakpoint>> BpNumBreakpointMap;
typedef BpNumBreakpointMap::const_iterator BpNumBreakpointMapIt;

class SourceFile
{
public:
    SourceFile(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    Cm::Core::CfgNode* GetNode(int lineNumber) const;
    void SetNode(int lineNumber, Cm::Core::CfgNode* node);
    void List(const BpNumBreakpointMap& breakpoints);
    void ListLine(int lineNumber, const BpNumBreakpointMap& breakpoints);
    void SetListLineNumber(int listLineNumber_) { listLineNumber = listLineNumber_; }
    void ResetListLineNumber() { listLineNumber = 0; }
    void SetActiveLineNumber(int activeLineNumber_) { activeLineNumber = activeLineNumber_; }
private:
    std::string filePath;
    typedef std::unordered_map<int, Cm::Core::CfgNode*> NodeMap;
    typedef NodeMap::const_iterator NodeMapIt;
    NodeMap nodeMap;
    bool indexed;
    int listLineNumber;
    int activeLineNumber;
    std::vector<std::string> lines;
    void Index();
};

class CFile
{
public:
    CFile(const std::string& cFilePath_);
    Cm::Core::CfgNode* GetNode(int lineNumber) const;
    void SetNode(int lineNumber, Cm::Core::CfgNode* node);
    Cm::Core::CFunCall* GetFunCallReturn(int returnLineNumber) const;
    void SetFunCallReturn(int returnLineNumber, Cm::Core::CFunCall* funCall);
private:
    std::string cFilePath;
    typedef std::unordered_map<int, Cm::Core::CfgNode*> NodeMap;
    typedef NodeMap::const_iterator NodeMapIt;
    NodeMap nodeMap;
    typedef std::unordered_map<int, Cm::Core::CFunCall*> FunCallMap;
    typedef FunCallMap::const_iterator FunCallMapIt;
    FunCallMap funCallMap;
};

class DebugInfo
{
public:
    DebugInfo(const std::string& debugInfoFilePath);
    Cm::Core::CFunctionDebugInfo* GetFunctionDebugInfo(const std::string& mangledFunctionName) const;
    State GetState() const { return state; }
    void SetState(State state_);
    Cm::Core::CFunctionDebugInfo* MainFunctionDebugInfo() const { return mainFunctionDebugInfo; }
    bool HasCBreakpoint(const std::string& cFileLine) const;
    Breakpoint* GetBreakpoint(const std::string& cFileLine) const;
    Breakpoint* GetBreakpoint(int bpNum) const;
    int SetBreakpoint(Breakpoint* bp);
    void SetBreakpoint(const std::string& cFileLine, Breakpoint* bp);
    void RemoveBreakpoint(int bpNum);
    void RemoveBreakpoint(const std::string& cFileLine);
    const BpNumBreakpointMap& Breakpoints() const { return bpNumBreakpointMap; }
    Cm::Core::CfgNode* CurrentNode() const { return currentNode; }
    void SetCurrentNode(Cm::Core::CfgNode* currentNode_) { currentNode = currentNode_; }
    Cm::Core::CfgNode* GetNode(const Cm::Core::SourceFileLine& sourceFileLine) const;
    Cm::Core::CfgNode* GetNodeByCFileLine(const Cm::Core::SourceFileLine& cFileLine) const;
    Cm::Core::CfgNode* GetNodeByCFileLine(const Cm::Core::SourceFileLine& cFileLine, bool throw_) const;
    Cm::Core::CFunCall* GetFunCallByCFileLine(const Cm::Core::SourceFileLine& cFileLine) const;
    Cm::Core::CFunCall* GetFunCallByCFileLine(const Cm::Core::SourceFileLine& cFileLine, bool throw_) const;
    SourceFile* GetSourceFile(const std::string& filePath) const;
    SourceFile* GetSourceFile(const std::string& filePath, bool throw_) const;
    SourceFile* GetCurrentSourceFile() const { return currentSourceFile; }
    void SetCurrentSourceFile(SourceFile* currentSourceFile_) { currentSourceFile = currentSourceFile_; }
    const std::unordered_set<Cm::Core::CfgNode*>& ThrowNodes() const { return throwNodes; }
    void RemoveThrowNode(Cm::Core::CfgNode* throwNode);
    void SetThrowCFileLines(const std::vector<std::string>& throwCFileLines_);
    const std::vector<std::string>& ThrowCFileLines() const { return throwCFileLines; }
    const std::unordered_set<Cm::Core::CfgNode*>& CatchNodes() const { return catchNodes; }
    void RemoveCatchNode(Cm::Core::CfgNode* catchNode);
    const std::vector<std::unique_ptr<SourceFile>>& SourceFiles() const { return sourceFiles; }
    Cm::Core::ClassDebugInfo* GetClassDebugInfo(const std::string& fullClassName) const;
private:
    State state;
    std::vector<std::unique_ptr<Cm::Core::CDebugInfoFile>> files;
    Cm::Core::CFunctionDebugInfo* mainFunctionDebugInfo;
    Cm::Core::CfgNode* currentNode;
    SourceFile* currentSourceFile;
    std::unordered_set<Cm::Core::CfgNode*> throwNodes;
    std::vector<std::string> throwCFileLines;
    std::unordered_set<Cm::Core::CfgNode*> catchNodes;
    typedef std::unordered_map<std::string, Cm::Core::CFunctionDebugInfo*> UniqueFunctionDebugInfoMap;
    typedef UniqueFunctionDebugInfoMap::const_iterator UniqueFunctionDebugInfoMapIt;
    UniqueFunctionDebugInfoMap uniqueFunctionDebugInfoMap;
    typedef std::unordered_map<std::string, Breakpoint*> CFileLineBreakpointMap;
    typedef CFileLineBreakpointMap::const_iterator CFileLineBreakpointMapIt;
    CFileLineBreakpointMap cFileLineBreakpointMap;
    int nextBreakpointNumber;
    BpNumBreakpointMap bpNumBreakpointMap;
    typedef std::unordered_map<std::string, SourceFile*> SourceFileMap;
    typedef SourceFileMap::const_iterator SourceFileMapIt;
    SourceFileMap sourceFileMap;
    std::vector<std::unique_ptr<SourceFile>> sourceFiles;
    typedef std::unordered_map<std::string, CFile*> CFileMap;
    typedef CFileMap::const_iterator CFileMapIt;
    CFileMap cFileMap;
    std::vector<std::unique_ptr<CFile>> cFiles;
    typedef std::unordered_map<std::string, Cm::Core::ClassDebugInfo*> ClassDebugInfoMap;
    typedef ClassDebugInfoMap::const_iterator ClassDebugInfoMapIt;
    ClassDebugInfoMap classDebugInfoMap;
    void SetSourceFile(const std::string& filePath, SourceFile* sourceFile);
    CFile* GetCFile(const std::string& cFilePath) const;
    void SetCFile(const std::string& cFilePath, CFile* cFile);
    void Process(Cm::Core::CDebugInfoFile* file);
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_DEBUG_INFO_INCLUDED
