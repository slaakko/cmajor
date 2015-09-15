/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_GLOBAL_SETTINGS_INCLUDED
#define CM_CORE_GLOBAL_SETTINGS_INCLUDED
#include <string>

namespace Cm { namespace Core {

class GlobalSettings
{
public:
    GlobalSettings();
    const std::string& Config() const;
    void SetConfig(const std::string& config_);
    int OptimizationLevel() const;
    void SetOptimizationLevel(int optimizationLevel_);
    void SetCurrentProjectName(const std::string& currentProjectName_);
    const std::string& CurrentProjectName() const { return currentProjectName; }
    void SetTargetTriple(const std::string& targetTriple_);
    const std::string& TargetTriple() const { return targetTriple; }
    void SetDatalayout(const std::string& datalayout_);
    const std::string& Datalayout() const { return datalayout; }
private:
    std::string config;
    int optimizationLevel;
    std::string currentProjectName;
    std::string targetTriple;
    std::string datalayout;
};

GlobalSettings* GetGlobalSettings();
void SetGlobalSettings(GlobalSettings* settings);

} } // namespace Cm::Core

#endif // CM_CORE_GLOBAL_SETTINGS_INCLUDED
