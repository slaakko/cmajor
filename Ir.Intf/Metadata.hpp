/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_METADATA_INCLUDED
#define IR_INTF_METADATA_INCLUDED
#include <Ir.Intf/Object.hpp>
#include <Cm.Util/CodeFormatter.hpp>

namespace Ir { namespace Intf {

using Cm::Util::CodeFormatter;

class MetadataNull: public Object
{
public:
    MetadataNull();
};

class MetadataString: public Object
{
public:
    MetadataString(const std::string& value_, Type* metadataType_);
};

typedef MetadataString* MetadataStringPtr;

class MetadataNode: public Object
{
public:
    MetadataNode(int id_, Type* metadataType_);
    void AddToContent(Object* object);
    void Write(CodeFormatter& formatter);
    int Id() const { return id; }
    void SetComment(const std::string& comment_) { comment = comment_; }
private:
    int id;
    std::vector<Object*> content;
    std::vector<std::unique_ptr<Object>> ownedContent;
    std::string comment;
};

class CDebugNode
{
public:
    virtual ~CDebugNode() {}
    virtual void SetCLine(int cline) = 0;
};

typedef CDebugNode* CDebugNodePtr;

} } // namespace Ir::Intf

#endif // IR_INTF_METADATA_INCLUDED
