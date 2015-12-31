/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Warning.hpp>

namespace Cm { namespace Sym {

Warning::Warning(const std::string& project_, const std::string& message_) : project(project_), message(message_)
{
}

CompileWarningCollection::CompileWarningCollection()
{
}

void CompileWarningCollection::Init() 
{
    instance.reset(new CompileWarningCollection());
}

void CompileWarningCollection::Done()
{
    instance.reset();
}

CompileWarningCollection& CompileWarningCollection::Instance()
{
    return *instance;
}

void CompileWarningCollection::SetCurrentProjectName(const std::string& currentProjectName_)
{
    currentProjectName = currentProjectName_;
}

std::unique_ptr<CompileWarningCollection> CompileWarningCollection::instance;

void CompileWarningCollection::AddWarning(const Warning& warning)
{
    warnings.push_back(warning);
}

void InitWarning()
{
    CompileWarningCollection::Init();
}

void DoneWarning()
{
    CompileWarningCollection::Done();
}

} } // namespace Cm::Sym