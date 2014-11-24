/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_CPPOBJECTMODEL_OBJECT_INCLUDED
#define CM_PARSING_CPPOBJECTMODEL_OBJECT_INCLUDED
#include <Cm.Util/CodeFormatter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace Cm { namespace Parsing { namespace CppObjectModel {

using Cm::Util::CodeFormatter;
class Visitor;

class CppObject
{
public:
    CppObject(const std::string& name_);
    virtual ~CppObject();
    const std::string& Name() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    void Own(CppObject* object);
    bool IsOwned() const { return isOwned; }
    void SetOwned() { isOwned = true; }
    void ResetOwned() { isOwned = false; }
    virtual std::string ToString() const;
    virtual void Print(CodeFormatter& formatter);
    virtual void Accept(Visitor& visitor) = 0;
    virtual int Rank() const { return 100; }
private:
    std::string name;
    bool isOwned;
    std::vector<std::unique_ptr<CppObject>> ownedObjects;
};

} } } // namespace Cm::Parsing::CppObjectModel

#endif // CM_PARSING_CPPOBJECTMODEL_OBJECT_INCLUDED
