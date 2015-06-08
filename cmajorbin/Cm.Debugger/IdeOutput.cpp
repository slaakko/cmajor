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

void IdePrintError(int sequenceNumber, const std::string& errorMessage)
{
    IdePrintError(sequenceNumber, errorMessage, false);
}

void IdePrintError(int sequenceNumber, const std::string& errorMessage, bool redirectError)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("error"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("errorMessage"), new Cm::Core::JsonString(errorMessage));
    reply.AddField(Cm::Core::JsonString("redirect"), new Cm::Core::JsonBool(redirectError));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintState(int sequenceNumber, const std::string& state, int exitCode, const std::string& signal, const std::string& signalCallStack)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("state"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("state"), new Cm::Core::JsonString(state));
    if (state == "exit")
    {
        reply.AddField(Cm::Core::JsonString("exitCode"), new Cm::Core::JsonNumber(exitCode));
    }
    else if (state == "signal")
    {
        reply.AddField(Cm::Core::JsonString("signal"), new Cm::Core::JsonString(signal));
        reply.AddField(Cm::Core::JsonString("signalCallStack"), new Cm::Core::JsonString(signalCallStack));
    }
    std::cout << reply.ToString() << std::endl;
}

void IdePrintOutput(int sequenceNumber, const std::string& output)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("output"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("output"), new Cm::Core::JsonString(output));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintPosition(int sequenceNumber, Cm::Core::CfgNode* node)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("position"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    Cm::Core::JsonObject* position = new Cm::Core::JsonObject();
    position->AddField(Cm::Core::JsonString("file"), new Cm::Core::JsonString(node->Function()->SourceFilePath()));
    position->AddField(Cm::Core::JsonString("line"), new Cm::Core::JsonNumber(node->GetSourceSpan().Line()));
    position->AddField(Cm::Core::JsonString("scol"), new Cm::Core::JsonNumber(node->GetSourceSpan().StartCol()));
    position->AddField(Cm::Core::JsonString("ecol"), new Cm::Core::JsonNumber(node->GetSourceSpan().EndCol()));
    reply.AddField(Cm::Core::JsonString("pos"), position);
    std::cout << reply.ToString() << std::endl;
}

void IdePrintBreakpointSet(int sequenceNumber, int bpNum)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpointSet"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("breakpoint"), new Cm::Core::JsonNumber(bpNum));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintBreakpointRemoved(int sequenceNumber, int bpNum)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpointRemoved"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("deleted"), new Cm::Core::JsonNumber(bpNum));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintCallStack(int sequenceNumber, const std::vector<Cm::Core::CfgNode*>& nodes)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("callStack"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
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

void IdePrintFrameReply(int sequenceNumber, int frame)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("frame"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("frame"), new Cm::Core::JsonNumber(frame));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintShowBreakpoints(int sequenceNumber, const std::vector<Breakpoint*>& breakpoints)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakpoints"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
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

void IdePrintBreakOnThrowReply(int sequenceNumber, bool enabled)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("breakOnThrow"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    reply.AddField(Cm::Core::JsonString("enabled"), new Cm::Core::JsonBool(enabled));
    std::cout << reply.ToString() << std::endl;
}

void IdePrintSources(int sequenceNumber, const std::vector<std::string>& sources)
{
    Cm::Core::JsonObject reply;
    reply.AddField(Cm::Core::JsonString("reply"), new Cm::Core::JsonString("sources"));
    reply.AddField(Cm::Core::JsonString("sequence"), new Cm::Core::JsonNumber(sequenceNumber));
    Cm::Core::JsonArray* sourcesArray = new Cm::Core::JsonArray();
    for (const std::string& source : sources)
    {
        sourcesArray->AddItem(new Cm::Core::JsonString(source));
    }
    reply.AddField(Cm::Core::JsonString("sources"), sourcesArray);
    std::cout << reply.ToString() << std::endl;
}

} } // Cm::Debugger
