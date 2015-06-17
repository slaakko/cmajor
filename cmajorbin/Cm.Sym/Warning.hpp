/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_WARNING_INCLUDED
#define CM_SYM_WARNING_INCLUDED
#include <memory>
#include <string>
#include <vector>

namespace Cm { namespace Sym {

class Warning
{
public:
    Warning(const std::string& project_, const std::string& message_);
    const std::string& Project() const { return project; }
    const std::string& Message() const { return message; }
private:
    std::string project;
    std::string message;
};

class CompileWarningCollection
{
public:
    static void Init();
    static void Done();
    static CompileWarningCollection& Instance();
    void SetCurrentProjectName(const std::string& currentProjectName_);
    const std::string& GetCurrentProjectName() const { return currentProjectName; }
    void AddWarning(const Warning& warning);
    const std::vector<Warning>& Warnings() const { return warnings; }
private:
    static std::unique_ptr<CompileWarningCollection> instance;
    CompileWarningCollection();
    std::string currentProjectName;
    std::vector<Warning> warnings;
};

void InitWarning();
void DoneWarning();

} } // namespace Cm::Sym

#endif // CM_SYM_WARNING_INCLUDED
