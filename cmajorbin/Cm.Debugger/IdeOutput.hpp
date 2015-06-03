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

void IdePrintError(const std::string& errorMessage);

void IdePrintState(const std::string& state, int exitCode);

void IdePrintOutput(const std::string& output);

void IdePrintPosition(Cm::Core::CfgNode* node);

void IdePrintBreakpointSet(int bpNum);

void IdePrintBreakpointRemoved(int bpNum);

void IdePrintCallStack(const std::vector<Cm::Core::CfgNode*>& nodes);

void IdePrintFrameReply(int frame);

class Breakpoint;

void IdePrintShowBreakpoints(const std::vector<Breakpoint*>& breakpoints);

void IdePrintBreakOnThrowReply(bool enabled);

} } // Cm::Debugger

#endif // CM_DEBUGGER_IDE_OUTPUT_INCLUDED
