/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_GLOBAL_SETTINGS_INCLUDED
#define CM_CORE_GLOBAL_SETTINGS_INCLUDED
#include <Cm.Ast/Project.hpp>
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
    void SetClassHierarchyDotFileName(const std::string& classHierarchyDotFileName_);
    const std::string& ClassHierarchyDotFileName() const { return classHierarchyDotFileName; }
    void SetTpgDotFileName(const std::string& tpgDotFileName_);
    const std::string& TpgDotFileName() const { return tpgDotFileName; }
    void SetVirtualCallFileName(const std::string& virtualCallFileName_);
    const std::string& VirtualCallFileName() const { return virtualCallFileName; }
    void SetLlvmVersion(const Cm::Ast::ProgramVersion& llvmVersion_);
    const Cm::Ast::ProgramVersion& LlvmVersion() const { return llvmVersion; }
private:
    std::string config;
    int optimizationLevel;
    std::string currentProjectName;
    std::string targetTriple;
    std::string datalayout;
    std::string classHierarchyDotFileName;
    std::string tpgDotFileName;
    std::string virtualCallFileName;
    Cm::Ast::ProgramVersion llvmVersion;
};

GlobalSettings* GetGlobalSettings();
void SetGlobalSettings(GlobalSettings* settings);

} } // namespace Cm::Core

#endif // CM_CORE_GLOBAL_SETTINGS_INCLUDED
