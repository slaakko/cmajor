/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_) : Node(span_)
{
}

FunctionGroupIdNode::FunctionGroupIdNode(const Span& span_, const std::string& functionGroupId_) : Node(span_), functionGroupId(functionGroupId_)
{
}

Node* FunctionGroupIdNode::Clone() const
{
    return new FunctionGroupIdNode(GetSpan(), functionGroupId);
}

void FunctionGroupIdNode::Read(Reader& reader)
{
    functionGroupId = reader.ReadString();
}

void FunctionGroupIdNode::Write(Writer& writer)
{
    writer.Write(functionGroupId);
}

} } // namespace Cm::Ast
