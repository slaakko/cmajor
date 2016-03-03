/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_STRING_REPOSITORY_INCLUDED
#define CM_CORE_STRING_REPOSITORY_INCLUDED
#include <Cm.Sym/BoundCompileUnitSerialization.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Ir.Intf/Constant.hpp>
#include <unordered_map>

namespace Cm { namespace Core {

class StringRepository : Cm::Sym::BcuItem
{
public:
    StringRepository();
    virtual ~StringRepository();
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    int InstallString(const std::string& str);
    int InstallWString(const std::string& str);
    int InstallUString(const std::string& str);
    Ir::Intf::Object* GetStringConstant(int id) const;
    Ir::Intf::Object* GetStringObject(int id) const;
    virtual void Write(Cm::Util::CodeFormatter& codeFormatter) = 0;
    const std::vector<std::unique_ptr<Ir::Intf::Object>>& StringObjects() const { return stringObjects; }
    const std::vector<std::unique_ptr<Ir::Intf::Object>>& StringConstants() const { return stringConstants; }
private:
    typedef std::unordered_map<std::string, int> StringIntMap;
    typedef StringIntMap::const_iterator StringIntMapIt;
    StringIntMap stringIntMap;
    StringIntMap wstringIntMap;
    StringIntMap ustringIntMap;
    std::vector<std::unique_ptr<Ir::Intf::Object>> stringConstants;
    std::vector<std::unique_ptr<Ir::Intf::Object>> stringObjects;
};

class LlvmStringRepository : public StringRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter) override;
};

class CStringRepository : public StringRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter) override;
};

} } // namespace Cm::Core

#endif // CM_CORE_STRING_REPOSITORY_INCLUDED
