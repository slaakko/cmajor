/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Ir.Intf/Metadata.hpp>
#include <Ir.Intf/Type.hpp>
#include <stdexcept>

namespace Ir { namespace Intf {

MetadataNull::MetadataNull(): Object("null", nullptr)
{
}

MetadataString::MetadataString(const std::string& value_, Type* metadataType_): Object(value_, metadataType_)
{
}

MetadataNode::MetadataNode(int id_, Type* metadataType_): Object("!" + std::to_string(id_), metadataType_), id(id_)
{
}

void MetadataNode::AddToContent(Object* object)
{
    content.push_back(object);
    if (!object->Owned())
    {
        object->SetOwned();
        ownedContent.push_back(std::unique_ptr<Object>(object));
    }
}

void MetadataNode::Write(CodeFormatter& formatter)
{
    formatter.Write(Name() + " = metadata !{");
    if (!content.empty())
    {
        bool first = true;
        for (Object* object : content)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                formatter.Write(", ");
            }
            if (object->GetType())
            {
                formatter.Write(object->GetType()->Name());
                formatter.Write(" ");
            }
            formatter.Write(object->Name());
        }
    }
    formatter.Write("}");
    if (!comment.empty())
    {
        formatter.Write(" ; " + comment);
    }
    formatter.WriteLine();
}

CDebugNode::~CDebugNode()
{
}

} } // namespace Ir::Intf
