/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/Warning.hpp>

namespace Cm { namespace Core {

GlobalSettings::GlobalSettings() : config("debug"), optimizationLevel(-1)
{
}

const std::string& GlobalSettings::Config() const
{ 
    return config; 
}

int GlobalSettings::OptimizationLevel() const
{
    if (optimizationLevel == -1)
    {
        if (config == "debug")
        {
            return 0;
        }
        else if (config == "release" || config == "profile" || config == "full")
        {
            return 3;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return optimizationLevel;
    }
}

void GlobalSettings::SetOptimizationLevel(int optimizationLevel_)
{
    optimizationLevel = optimizationLevel_;
}

void GlobalSettings::SetConfig(const std::string& config_)
{
    config = config_;
}

void GlobalSettings::SetCurrentProjectName(const std::string& currentProjectName_)
{
    currentProjectName = currentProjectName_;
    Cm::Sym::CompileWarningCollection::Instance().SetCurrentProjectName(currentProjectName);
}

void GlobalSettings::SetTargetTriple(const std::string& targetTriple_)
{
    targetTriple = targetTriple_;
}

void GlobalSettings::SetDatalayout(const std::string& datalayout_)
{
    datalayout = datalayout_;
}

GlobalSettings* globalSettings = nullptr;

GlobalSettings* GetGlobalSettings()
{
    return globalSettings;
}

void SetGlobalSettings(GlobalSettings* settings)
{
    globalSettings = settings;
}

} } // namespace Cm::Core
