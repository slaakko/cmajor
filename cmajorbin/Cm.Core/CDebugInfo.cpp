/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/CDebugInfo.hpp>

namespace Cm { namespace Core {

void SourceSpan::Read(Cm::Ser::BinaryReader& reader)
{
    line = reader.ReadInt();
    startCol = reader.ReadInt();
    endCol = reader.ReadInt();
}

void SourceSpan::Write(Cm::Ser::BinaryWriter& writer)
{
    writer.Write(line);
    writer.Write(startCol);
    writer.Write(endCol);
}

void SourceSpan::Dump(Cm::Util::CodeFormatter& formatter)
{
    formatter.WriteLine("span: " + std::to_string(line) + ":" + std::to_string(startCol) + ":" + std::to_string(endCol) + ";");
}

const char* LineStart(const char* start, const char* pos)
{
    while (pos > start && *pos != '\n')
    {
        --pos;
    }
    if (*pos == '\n')
    {
        ++pos;
    }
    return pos;
}

SourceSpan FromSpan(const char* start, const char* end, const Cm::Parsing::Span& span)
{
    const char* s = start + span.Start();
    if (s < start || s >= end) return SourceSpan();
    const char* e = start + span.End();
    if (e < start || e >= end) return SourceSpan();
    const char* sStart = LineStart(start, s);
    int32_t startCol = int32_t(s - sStart) + 1;
    const char* eStart = LineStart(start, e);
    int32_t endCol = int32_t(e - eStart) + 1;
    return SourceSpan(span.LineNumber(), startCol, endCol);
}

CFunCall::CFunCall() : funNames(), cLine(0)
{
}

CFunCall::CFunCall(const std::vector<std::string>& funNames_, int32_t cLine_) : funNames(funNames_), cLine(cLine_)
{
} 

void CFunCall::FixCLines(int32_t offset)
{
    cLine += offset;
}

void CFunCall::Read(Cm::Ser::BinaryReader& reader)
{
    int32_t numFunNames = reader.ReadInt();
    for (int32_t i = 0; i < numFunNames; ++i)
    {
        std::string funName = reader.ReadString();
        funNames.push_back(funName);
    }
    cLine = reader.ReadInt();
}

void CFunCall::Write(Cm::Ser::BinaryWriter& writer)
{
    int32_t n = int32_t(funNames.size());
    writer.Write(n);
    for (const std::string& funName : funNames)
    {
        writer.Write(funName);
    }
    writer.Write(cLine);
}

void CFunCall::Dump(Cm::Util::CodeFormatter& formatter)
{
    for (const std::string& funName : funNames)
    {
        formatter.WriteLine(funName);
    }
    formatter.WriteLine("cline: " + std::to_string(cLine) + ";");
}

std::string CfgNodeKindStr(CfgNodeKind kind)
{
    switch (kind)
    {
        case CfgNodeKind::regularNode: return "regular";
        case CfgNodeKind::throwNode: return "throw";
        case CfgNodeKind::catchNode: return "catch";
    }
    return "";
}

CfgNode::CfgNode() : id(-1), sourceSpan(), cLine(-1), kind(CfgNodeKind::regularNode)
{

}

CfgNode::CfgNode(int32_t id_, const SourceSpan& sourceSpan_) : id(id_), sourceSpan(sourceSpan_), cLine(-1), kind(CfgNodeKind::regularNode)
{
}

void CfgNode::AddNext(int32_t nextNodeId)
{
    next.insert(nextNodeId);
}

void CfgNode::FixCLines(int32_t offset)
{
    cLine += offset;
    for (CFunCall& cFunCall : cFunCalls)
    {
        cFunCall.FixCLines(offset);
    }
}

void CfgNode::Read(Cm::Ser::BinaryReader& reader)
{
    id = reader.ReadInt();
    sourceSpan.Read(reader);
    cLine = reader.ReadInt();
    int32_t nNext = reader.ReadInt();
    for (int32_t i = 0; i < nNext; ++i)
    {
        int32_t id = reader.ReadInt();
        next.insert(id);
    }
    int32_t nFunCalls = reader.ReadInt();
    for (int32_t i = 0; i < nFunCalls; ++i)
    {
        CFunCall cFunCall;
        cFunCall.Read(reader);
        cFunCalls.push_back(cFunCall);
    }
    kind = CfgNodeKind(reader.ReadByte());
}

void CfgNode::Write(Cm::Ser::BinaryWriter& writer)
{
    writer.Write(id);
    sourceSpan.Write(writer);
    writer.Write(cLine);
    int32_t nNext = int32_t(next.size());
    writer.Write(nNext);
    for (int32_t id : next)
    {
        writer.Write(id);
    }
    int32_t nFunCalls = int32_t(cFunCalls.size());
    writer.Write(nFunCalls);
    for (CFunCall& cFunCall : cFunCalls)
    {
        cFunCall.Write(writer);
    }
    writer.Write(uint8_t(kind));
}

void CfgNode::Dump(Cm::Util::CodeFormatter& formatter)
{
    formatter.Write("node " + std::to_string(id));
    if (!next.empty())
    {
        formatter.Write(": ");
        bool first = true;
        for (int32_t nextId : next)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                formatter.Write(", ");
            }
            formatter.Write(std::to_string(nextId));
        }
    }
    formatter.WriteLine(";");
    formatter.WriteLine("{");
    formatter.IncIndent();
    sourceSpan.Dump(formatter);
    formatter.WriteLine("cline: " + std::to_string(cLine) + ";");
    formatter.WriteLine("calls:");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (CFunCall& cFunCall : cFunCalls)
    {
        cFunCall.Dump(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("kind: " + CfgNodeKindStr(kind) + ";");
    formatter.DecIndent();
    formatter.WriteLine("}");
}

ControlFlowGraph::ControlFlowGraph()
{
}

CfgNode* ControlFlowGraph::CreateNode(const Cm::Parsing::Span& span, const char* start, const char* end)
{
    CfgNode* node = new CfgNode(int32_t(nodes.size()), FromSpan(start, end, span));
    nodes.push_back(std::unique_ptr<CfgNode>(node));
    return node;
}

void ControlFlowGraph::AddNode(CfgNode* node)
{
    nodes.push_back(std::unique_ptr<CfgNode>(node));
}

CfgNode* ControlFlowGraph::GetNode(int32_t nodeId) const
{
    if (nodeId < int32_t(0) || nodeId >= int32_t(nodes.size())) throw std::runtime_error("invalid C debug node id");
    return nodes[nodeId].get();
}

bool Found(const ControlFlowGraph& cfg, const std::unordered_set<int32_t>& nodeIds, int32_t nodeId, std::unordered_set<int32_t>& s, std::unordered_set<int32_t>& checked)
{
    if (nodeIds.find(nodeId) != nodeIds.end())
    {
        s.insert(nodeId);
        return true;
    }
    for (int32_t next : nodeIds)
    {
        if (checked.find(next) != checked.end()) continue;
        checked.insert(next);
        CfgNode* nextNode = cfg.GetNode(next);
        if (Found(cfg, nextNode->Next(), nodeId, s, checked))
        {
            s.insert(nextNode->Id());
            s.insert(nextNode->Next().begin(), nextNode->Next().end());
            return true;
        }
    }
    return false;
}

std::unordered_set<int32_t> ControlFlowGraph::GetNodeSetIncluding(int32_t nodeId) const
{
    std::unordered_set<int32_t> nodeIds;
    std::unordered_set<int32_t> checked;
    nodeIds.insert(int32_t(0));
    if (nodes.empty())
    {
        throw std::runtime_error("nodes empty");
    }
    CfgNode* first = nodes[0].get();
    if (Found(*this, first->Next(), nodeId, nodeIds, checked))
    {
        return nodeIds;
    }
    return nodeIds;
}

void ControlFlowGraph::AddToPrevNodes(CfgNode* node)
{
    prevNodes.insert(node);
}

void ControlFlowGraph::AddToPrevNodes(const std::unordered_set<CfgNode*>& nodes)
{
    prevNodes.insert(nodes.begin(), nodes.end());
}

void ControlFlowGraph::PatchPrevNodes(CfgNode* nextNode)
{
    for (CfgNode* prevNode : prevNodes)
    {
        prevNode->AddNext(nextNode->Id());
    }
    prevNodes.clear();
}

std::unordered_set<CfgNode*> ControlFlowGraph::RetrivePrevNodes()
{
    std::unordered_set<CfgNode*> copy = std::move(prevNodes);
    prevNodes.clear();
    return copy;
}

void ControlFlowGraph::Patch(std::unordered_set<CfgNode*> set, CfgNode* nextNode)
{
    for (CfgNode* prevNode : set)
    {
        prevNode->AddNext(nextNode->Id());
    }
}

void ControlFlowGraph::FixCLines(int32_t offset)
{
    for (const std::unique_ptr<CfgNode>& node : nodes)
    {
        node->FixCLines(offset);
    }
}

void ControlFlowGraph::Read(Cm::Ser::BinaryReader& reader)
{
    int32_t n = reader.ReadInt();
    for (int i = 0; i < n; ++i)
    {
        CfgNode* node = new CfgNode();
        node->Read(reader);
        AddNode(node);
    }
}

void ControlFlowGraph::Write(Cm::Ser::BinaryWriter& writer)
{
    int32_t n = int32_t(nodes.size());
    writer.Write(n);
    for (const std::unique_ptr<CfgNode>& node : nodes)
    {
        node->Write(writer);
    }
}

void ControlFlowGraph::Dump(Cm::Util::CodeFormatter& formatter)
{
    formatter.WriteLine("cfg:");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const std::unique_ptr<CfgNode>& node : nodes)
    {
        node->Dump(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

CFunctionDebugInfo::CFunctionDebugInfo() : isMain(false)
{
}

CFunctionDebugInfo::CFunctionDebugInfo(const std::string& mangledFunctionName_): isMain(false), mangledFunctionName(mangledFunctionName_), functionDisplayName(), cfg(), sourceFilePath()
{
}

void CFunctionDebugInfo::SetFunctionDisplayName(const std::string& functionDisplayName_)
{
    functionDisplayName = functionDisplayName_;
}

void CFunctionDebugInfo::SetSourceFilePath(const std::string& sourceFilePath_)
{
    sourceFilePath = sourceFilePath_;
}

void CFunctionDebugInfo::FixCLines(int32_t offset)
{
    cfg.FixCLines(offset);
}

void CFunctionDebugInfo::Read(Cm::Ser::BinaryReader& reader)
{
    mangledFunctionName = reader.ReadString();
    isMain = reader.ReadBool();
    functionDisplayName = reader.ReadString();
    cfg.Read(reader);
    sourceFilePath = reader.ReadString();
}

void CFunctionDebugInfo::Write(Cm::Ser::BinaryWriter& writer)
{
    writer.Write(mangledFunctionName);
    writer.Write(isMain);
    writer.Write(functionDisplayName);
    cfg.Write(writer);
    writer.Write(sourceFilePath);
}

void CFunctionDebugInfo::Dump(Cm::Util::CodeFormatter& formatter)
{
    formatter.WriteLine(mangledFunctionName);
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine(std::string("main: ") + (isMain ? "true;" : "false;"));
    formatter.WriteLine("display name: " + functionDisplayName + ";");
    formatter.WriteLine("source file path: " + sourceFilePath + ";");
    cfg.Dump(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

CDebugInfoFile::CDebugInfoFile()
{
}

void CDebugInfoFile::AddFunctionDebugInfo(CFunctionDebugInfo* functionDebugInfo)
{
    if (!functionDebugInfo->Cfg().IsEmpty())
    {
        functionDebugInfos.push_back(std::unique_ptr<CFunctionDebugInfo>(functionDebugInfo));
    }
    else
    {
        delete functionDebugInfo;
    }
}

void CDebugInfoFile::FixCLines(int32_t offset)
{
    for (const std::unique_ptr<CFunctionDebugInfo>& functionDebugInfo : functionDebugInfos)
    {
        functionDebugInfo->FixCLines(offset);
    }
}

void CDebugInfoFile::Read(Cm::Ser::BinaryReader& reader)
{
    int32_t nFunctionDebugInfos = reader.ReadInt();
    for (int i = 0; i < nFunctionDebugInfos; ++i)
    {
        CFunctionDebugInfo* functionDebugInfo = new CFunctionDebugInfo();
        functionDebugInfo->Read(reader);
        AddFunctionDebugInfo(functionDebugInfo);
    }
}

void CDebugInfoFile::Write(Cm::Ser::BinaryWriter& writer)
{
    int32_t nFunctionDebugInfos = int32_t(functionDebugInfos.size());
    writer.Write(nFunctionDebugInfos);
    for (const std::unique_ptr<CFunctionDebugInfo>& functionDebugInfo : functionDebugInfos)
    {
        functionDebugInfo->Write(writer);
    }
}

void CDebugInfoFile::Dump(Cm::Util::CodeFormatter& formatter)
{
    for (const std::unique_ptr<CFunctionDebugInfo>& functionDebugInfo : functionDebugInfos)
    {
        functionDebugInfo->Dump(formatter);
    }
}

} } // namespace Cm::Core
