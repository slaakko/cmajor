/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CDEBUG_INFO_INCLUDED
#define CM_CORE_CDEBUG_INFO_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Ser\BinaryReader.hpp>
#include <Cm.Ser\BinaryWriter.hpp>
#include <Ir.Intf/Metadata.hpp>
#include <unordered_map>
#include <unordered_set>
#include <stdint.h>

namespace Cm { namespace Core {

class SourceFileLine
{
public:
    SourceFileLine();
    SourceFileLine(const std::string& sourceFilePath_, int sourceLineNumber_);
    bool IsEmpty() const { return sourceLineNumber == 0; }
    const std::string& SourceFilePath() const { return sourceFilePath; }
    int SourceLineNumber() const { return sourceLineNumber; }
    void SetSourceFilePath(const std::string& sourceFilePath_) { sourceFilePath = sourceFilePath_; }
    void SetSourceLineNumber(int n) { sourceLineNumber = n; }
    std::string ToString() const;
private:
    std::string sourceFilePath;
    int sourceLineNumber;
};

bool operator==(const SourceFileLine& left, const SourceFileLine& right);
bool operator<(const SourceFileLine& left, const SourceFileLine& right);

inline bool operator!=(const SourceFileLine& left, const SourceFileLine& right)
{
    return std::rel_ops::operator!=(left, right);
}

inline bool operator>(const SourceFileLine& left, const SourceFileLine& right)
{
    return std::rel_ops::operator>(left, right);
}

inline bool operator>=(const SourceFileLine& left, const SourceFileLine& right)
{
    return std::rel_ops::operator>=(left, right);
}

inline bool operator<=(const SourceFileLine& left, const SourceFileLine& right)
{
    return std::rel_ops::operator<=(left, right);
}

class SourceSpan
{ 
public:
    SourceSpan() : line(-1), startCol(-1), endCol(-1) {}
    SourceSpan(int32_t line_, int32_t startCol_, int32_t endCol_) : line(line_), startCol(startCol_), endCol(endCol_) {}
    int32_t Line() const { return line; }
    int32_t StartCol() const { return startCol; }
    int32_t EndCol() const { return endCol; }
    bool Valid() const { return line != -1 && startCol != -1 && endCol != -1; }
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    bool IsNull() const { return line == 0 && startCol == 0 && endCol == 0; }
private:
    int32_t line;
    int32_t startCol;
    int32_t endCol;
};

SourceSpan FromSpan(const char* start, const char* end, const Cm::Parsing::Span& span);

class CfgNode;

class CFunCall : public Ir::Intf::CDebugNode
{
public:
    CFunCall();
    CFunCall(const std::vector<std::string>& funNames_);
    const std::vector<std::string>& FunNames() const { return funNames; }
    int32_t ReturnCLine() const { return cLine; }
    int32_t CallCLine() const { return cLine - 1; }
    void SetCLine(int32_t cLine_) override { cLine = cLine_; }
    void FixCLines(int32_t offset);
    CfgNode* Node() const { return cfgNode; }
    void SetNode(CfgNode* cfgNode_) { cfgNode = cfgNode_; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    SourceFileLine GetCallCFileLine() const;
    SourceFileLine GetReturnCFileLine() const;
    bool IsLastCall() const;
private:
    std::vector<std::string> funNames;
    int32_t cLine;
    CfgNode* cfgNode;
    int index;
};

enum class CfgNodeKind : uint8_t
{
    regularNode, exitNode, throwNode, catchNode
};

class CFunctionDebugInfo;

class CfgNode : public Ir::Intf::CDebugNode
{
public:
    CfgNode();
    CfgNode(int32_t id_, const SourceSpan& sourceSpan_);
    int32_t Id() const { return id; }
    const SourceSpan& GetSourceSpan() const { return sourceSpan; }
    int32_t CLine() const { return cLine; }
    void SetCLine(int32_t cLine_) override { cLine = cLine_; }
    const std::unordered_set<int32_t>& Next() const { return next; }
    const std::vector<std::unique_ptr<CFunCall>>& CFunCalls() const { return cFunCalls; }
    void AddCFunCall(CFunCall* cFunCall);
    CFunCall* GetCFunCall(int index) const;
    CfgNodeKind Kind() const { return kind; }
    void SetKind(CfgNodeKind kind_) { kind = kind_; }
    void AddNext(int32_t nextNodeId);
    void FixCLines(int32_t offset);
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    CFunctionDebugInfo* Function() const { return function; }
    void SetFunction(CFunctionDebugInfo* function_) { function = function_; }
    SourceFileLine GetCFileLine() const;
private:
    int32_t id;
    SourceSpan sourceSpan;
    int32_t cLine;
    std::unordered_set<int32_t> next;
    std::vector<std::unique_ptr<CFunCall>> cFunCalls;
    CfgNodeKind kind;
    CFunctionDebugInfo* function;
};

class ControlFlowGraph
{
public:
    ControlFlowGraph();
    CfgNode* CreateNode(const Cm::Parsing::Span& span, const char* start, const char* end);
    const std::vector<std::unique_ptr<CfgNode>>& Nodes() const { return nodes; }
    void AddNode(CfgNode* node);
    CfgNode* GetNode(int32_t nodeId) const;
    CfgNode* FindNode(int sourceLineNumber) const;
    std::unordered_set<int32_t> GetNodeSetIncluding(int32_t nodeId) const;
    void AddToPrevNodes(CfgNode* node);
    void AddToPrevNodes(const std::unordered_set<CfgNode*>& nodes);
    void PatchPrevNodes(CfgNode* nextNode);
    std::unordered_set<CfgNode*> RetrivePrevNodes();
    void Patch(std::unordered_set<CfgNode*> set, CfgNode* nextNode);
    void FixCLines(int32_t offset);
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    bool IsEmpty() const { return nodes.empty(); }
    const std::unordered_set<CfgNode*>& Exits() const { return exits; }
    void AddExit(CfgNode* exitNode);
private:
    std::vector<std::unique_ptr<CfgNode>> nodes;
    std::unordered_set<CfgNode*> prevNodes;
    std::unordered_set<CfgNode*> exits;
};

class CDebugInfoFile;

class CFunctionDebugInfo
{
public:
    CFunctionDebugInfo();
    CFunctionDebugInfo(const std::string& mangledFunctionName_);
    const std::string& MangledFunctionName() const { return mangledFunctionName; }
    bool IsMain() const { return isMain; }
    void SetMain() { isMain = true; }
    bool IsUnique() const { return isUnique; }
    void SetUnique() { isUnique = true; }
    void SetFunctionDisplayName(const std::string& functionDisplayName_);
    const std::string& FunctionDisplayName() const { return functionDisplayName; }
    ControlFlowGraph& Cfg() { return cfg; }
    void SetSourceFilePath(const std::string& sourceFilePath_);
    const std::string& SourceFilePath() const { return sourceFilePath; }
    void SetCFilePath(const std::string& cFilePath_);
    const std::string& CFilePath() const { return cFilePath; }
    void FixCLines(int32_t offset);
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    CDebugInfoFile* File() const { return file; }
    void SetFile(CDebugInfoFile* file_) { file = file_; }
    CfgNode* Entry() const;
private:
    std::string mangledFunctionName;
    bool isMain;
    bool isUnique;
    std::string functionDisplayName;
    ControlFlowGraph cfg;
    std::string sourceFilePath;
    std::string cFilePath;
    CDebugInfoFile* file;
};

class CDebugInfoFile
{
public:
    CDebugInfoFile();
    void AddFunctionDebugInfo(CFunctionDebugInfo* functionDebugInfo);
    const std::vector<std::unique_ptr<CFunctionDebugInfo>>& FunctionDebugInfos() const { return functionDebugInfos; }
    void FixCLines(int32_t offset);
    void Read(Cm::Ser::BinaryReader& reader);
    void Write(Cm::Ser::BinaryWriter& writer);
    void Dump(Cm::Util::CodeFormatter& formatter);
    void AddFunctionDebugInfoToMap(CFunctionDebugInfo* functionDebugInfo);
    CFunctionDebugInfo* GetFunctionDebugInfo(const std::string& mangledFunctionName) const;
private:
    std::vector<std::unique_ptr<CFunctionDebugInfo>> functionDebugInfos;
    typedef std::unordered_map<std::string, CFunctionDebugInfo*> FunctionDebugInfoMap;
    typedef FunctionDebugInfoMap::const_iterator FunctionDebugInfoMapIt;
    FunctionDebugInfoMap functionDebugInfoMap;
};

} } // namespace Cm::Core

#endif // CM_CORE_CDEBUG_INFO_INCLUDED
