/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_LABEL_INCLUDED
#define IR_INTF_LABEL_INCLUDED
#include <Ir.Intf/Object.hpp>
#include <vector>

namespace Ir { namespace Intf {

class LabelObject: public Object
{
public:
    LabelObject(Ir::Intf::Type* labelType);
    LabelObject(Ir::Intf::Type* labelType, const std::string& label_, const std::string& objectName_);
    virtual std::string Target() const = 0;
    virtual void Set(LabelObject* from) = 0;
    void SetLabelName(std::string label_) { label = label_; }
    const std::string& GetLabelName() const { return label; }
private:
    std::string label;
};

void Add(std::vector<LabelObject*>& to, LabelObject* label);
void Merge(std::vector<LabelObject*>& to, const std::vector<LabelObject*>& labels);
void Backpatch(std::vector<LabelObject*>& targets, LabelObject* from);

} } // namespace Ir::Intf

#endif // IR_INTF_LABEL_INCLUDED
