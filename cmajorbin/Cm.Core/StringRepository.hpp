/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_STRING_REPOSITORY_INCLUDED
#define CM_CORE_STRING_REPOSITORY_INCLUDED
#include <Cm.Util/CodeFormatter.hpp>
#include <Ir.Intf/Constant.hpp>
#include <unordered_map>

namespace Cm { namespace Core {

class StringRepository
{
public:
    int Install(const std::string& str);
    Ir::Intf::Object* GetStringConstant(int id) const;
    Ir::Intf::Object* GetStringObject(int id) const;
    void Write(Cm::Util::CodeFormatter& codeFormatter);
private:
    typedef std::unordered_map<std::string, int> StringIntMap;
    typedef StringIntMap::const_iterator StringIntMapIt;
    StringIntMap stringIntMap;
    std::vector<std::unique_ptr<Ir::Intf::Object>> stringConstants;
    std::vector<std::unique_ptr<Ir::Intf::Object>> stringObjects;
};

} } // namespace Cm::Core

#endif // CM_CORE_STRING_REPOSITORY_INCLUDED
