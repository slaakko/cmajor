/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Ast {

bool HasStaticSpecifier(Specifiers s)
{
    return (s & Specifiers::static_) != Specifiers::none;
}

std::string SpecifierStr(Specifiers s)
{
    std::string str;
    if ((s & Specifiers::external) != Specifiers::none)
    {
        str.append("extern");
    }
    Specifiers access = s & Specifiers::access;
    if ((access & Specifiers::public_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("public");
    }
    else if ((access & Specifiers::protected_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("protect");
    }
    else if ((access & Specifiers::internal_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("internal");
    }
    else if ((access & Specifiers::private_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("private");
    }
    if ((s & Specifiers::cdecl_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("cdecl");
    }
    if ((s & Specifiers::nothrow_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("nothrow");
    }
    else if ((s & Specifiers::throw_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("throw");
    }
    if ((s & Specifiers::static_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("static");
    }
    if ((s & Specifiers::virtual_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("virtual");
    }
    if ((s & Specifiers::abstract_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("abstract");
    }
    if ((s & Specifiers::override_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("override");
    }
    if ((s & Specifiers::suppress) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("suppress");
    }
    if ((s & Specifiers::default_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("default");
    }
    if ((s & Specifiers::explicit_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("explicit");
    }
    if ((s & Specifiers::inline_) != Specifiers::none)
    {
        if (!str.empty())
        {
            str.append(1, ' ');
        }
        str.append("inline");
    }
    return str;
}

} } // namespace Cm::Ast