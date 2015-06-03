/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/IdeOutput.hpp>
#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Core/Json.hpp>
#include <iostream>

namespace Cm { namespace Debugger {

void IdePrintError(const std::string& errorMessage)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("error"));
    reply.AddField(Cm::Core::JsonString("errorMessage"), new Cm::Core::JsonString(errorMessage));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintState(const std::string& state, int exitCode)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("state"));
    reply.AddField(Cm::Core::JsonString("state"), new Cm::Core::JsonString(state));
    if (state == "exit")
    {
        reply.AddField(Cm::Core::JsonString("exitCode"), new Cm::Core::JsonNumber(exitCode));
    }
    std::cout << reply.ToString() << std::endl;
}

void IdePrintOutput(const std::string& output)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("output"));
    reply.AddField(Cm::Core::JsonString("output"), new Cm::Core::JsonString(output));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintPosition(Cm::Core::CfgNode* node)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("position"));
    Cm::Core::JsonObject* position = new Cm::Core::JsonObject();
    position->AddField(Cm::Core::JsonString("file"), new Cm::Core::JsonString(node->Function()->SourceFilePath()));
    position->AddField(Cm::Core::JsonString("line"), new Cm::Core::JsonNumber(node->GetSourceSpan().Line()));
    position->AddField(Cm::Core::JsonString("scol"), new Cm::Core::JsonNumber(node->GetSourceSpan().StartCol()));
    position->AddField(Cm::Core::JsonString("ecol"), new Cm::Core::JsonNumber(node->GetSourceSpan().EndCol()));
    reply.AddField(Cm::Core::JsonString("pos"), position);
    std::cout << reply.ToString() << std::endl;
}

void IdePrintBreakpointSet(int bpNum)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpointSet"));
    reply.AddField(Cm::Core::JsonString("breakpoint"), new Cm::Core::JsonNumber(bpNum));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintBreakpointRemoved(int bpNum)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpointRemoved"));
    reply.AddField(Cm::Core::JsonString("deleted"), new Cm::Core::JsonNumber(bpNum));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintCallStack(const std::vector<Cm::Core::CfgNode*>& nodes)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("callStack"));
    Cm::Core::JsonArray* frameArray = new Cm::Core::JsonArray();
    for (Cm::Core::CfgNode* node : nodes)
    {
        Cm::Core::JsonObject* frame = new Cm::Core::JsonObject();
        frame->AddField(Cm::Core::JsonString("func"), new Cm::Core::JsonString(node->Function()->FunctionDisplayName()));
        frame->AddField(Cm::Core::JsonString("file"), new Cm::Core::JsonString(node->Function()->SourceFilePath()));
        frame->AddField(Cm::Core::JsonString("line"), new Cm::Core::JsonNumber(node->GetSourceSpan().Line()));
        frameArray->AddItem(frame);
    }
    reply.AddField(Cm::Core::JsonString("frames"), frameArray);
    std::cout << reply.ToString() << std::endl;
}

void IdePrintFrameReply(int frame)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("frame"));
    reply.AddField(Cm::Core::JsonString("frame"), new Cm::Core::JsonNumber(frame));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintShowBreakpoints(const std::vector<Breakpoint*>& breakpoints)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpoints"));
    Cm::Core::JsonArray* breakpointArray = new Cm::Core::JsonArray();
    for (Breakpoint* bp : breakpoints)
    {
        Cm::Core::JsonObject* breakpoint = new Cm::Core::JsonObject();
        breakpoint->AddField(Cm::Core::JsonString("number"), new Cm::Core::JsonNumber(bp->Number()));
        Cm::Core::CfgNode* node = bp->Node();
        breakpoint->AddField(Cm::Core::JsonString("file"), new Cm::Core::JsonString(node->Function()->SourceFilePath()));
        breakpoint->AddField(Cm::Core::JsonString("line"), new Cm::Core::JsonNumber(node->GetSourceSpan().Line()));
        breakpointArray->AddItem(breakpoint);
    }
    reply.AddField(Cm::Core::JsonString("breakpoints"), breakpointArray);
    std::cout << reply.ToString() << std::endl;
}

void IdePrintBreakOnThrowReply(bool enabled)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakOnThrow"));
    reply.AddField(Cm::Core::JsonString("enabled"), new Cm::Core::JsonBool(enabled));
    std::cout << reply.ToString() << std::endl;
}

} } // Cm::Debugger
