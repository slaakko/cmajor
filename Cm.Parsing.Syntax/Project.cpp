/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing.Syntax/Project.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace boost::filesystem;

Project::Project(const std::string& name_, const std::string& filePath_): name(name_), filePath(filePath_), base(filePath)
{
    base.remove_filename();
}

std::string Project::BasePath() const
{
    return base.generic_string();
}

void Project::AddSourceFile(const std::string& sourceFilePath)
{
    path sfp(sourceFilePath);
    path sfpa = absolute(sfp, base);
    sourceFiles.push_back(sfpa.generic_string());
}

void Project::AddReferenceFile(const std::string& referenceFilePath)
{
    referenceFiles.push_back(referenceFilePath);
}

} } } // namespace Cm::Parsing::Syntax
