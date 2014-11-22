/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_CPPOBJECTMODEL_LITERAL_INCLUDED
#define CM_PARSING_CPPOBJECTMODEL_LITERAL_INCLUDED
#include <Cm.Parsing.CppObjectModel/Object.hpp>

namespace Cm { namespace Parsing { namespace CppObjectModel {

class Literal: public CppObject
{
public:
    Literal(const std::string& name_);
    virtual int Rank() const { return 24; }
    virtual void Accept(Visitor& visitor);
};

} } } // namespace Cm::Parsing::CppObjectModel

#endif // CM_PARSING_CPPOBJECTMODEL_LITERAL_INCLUDED

