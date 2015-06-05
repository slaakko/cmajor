/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_IDE_OUTPUT_INCLUDED
#define CM_DEBUGGER_IDE_OUTPUT_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>
#include <string>

namespace Cm { namespace Debugger {

void IdePrintError(int sequenceNumber, const std::string& errorMessage);

void IdePrintError(int sequenceNumber, const std::string& errorMessage, bool redirectError);

void IdePrintState(int sequenceNumber, const std::string& state, int exitCode, const std::string& signal, const std::string& signaCallStack);

void IdePrintOutput(int sequenceNumber, const std::string& output);

void IdePrintPosition(int sequenceNumber, Cm::Core::CfgNode* node);

void IdePrintBreakpointSet(int sequenceNumber, int bpNum);

void IdePrintBreakpointRemoved(int sequenceNumber, int bpNum);

void IdePrintCallStack(int sequenceNumber, const std::vector<Cm::Core::CfgNode*>& nodes);

void IdePrintFrameReply(int sequenceNumber, int frame);

class Breakpoint;

void IdePrintShowBreakpoints(int sequenceNumber, const std::vector<Breakpoint*>& breakpoints);

void IdePrintBreakOnThrowReply(int sequenceNumber, bool enabled);

} } // Cm::Debugger

#endif // CM_DEBUGGER_IDE_OUTPUT_INCLUDED
