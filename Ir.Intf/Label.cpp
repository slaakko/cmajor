/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/Label.hpp>
#include <Ir.Intf/Type.hpp>
#include <Cm.Util/TextUtils.hpp>

namespace Ir { namespace Intf {

LabelObject::LabelObject(Ir::Intf::Type* labelType): Object("", labelType), label()
{
}

LabelObject::LabelObject(Ir::Intf::Type* labelType, const std::string& label_, const std::string& objectName_): Object(objectName_, labelType), label(label_)
{
}

void Add(std::vector<LabelObject*>& to, LabelObject* label)
{
    for (LabelObject* t : to)
    {
        if (t == label)
        {
            return;
        }
    }
    to.push_back(label);
}

void Merge(std::vector<LabelObject*>& to, const std::vector<LabelObject*>& labels)
{
    for (LabelObject* label : labels)
    {
        Add(to, label);
    }
}

void Backpatch(std::vector<LabelObject*>& targets, LabelObject*from)
{
    for (LabelObject* target : targets)
    {
        target->Set(from);
    }
}

} } // namespace Ir::Intf
