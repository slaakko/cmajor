/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_SPECIFIER_INCLUDED
#define CM_AST_SPECIFIER_INCLUDED
#include <string>
#include <stdint.h>

namespace Cm { namespace Ast {

enum class Specifiers: uint32_t
{
    none = 0,
    public_ = 1, protected_ = 2, private_ = 4, internal_ = 8, 
    access = public_ | protected_ | private_ | internal_,
    static_ = 16,
    virtual_ = 32, override_ = 64, abstract_ = 128, virtuality = virtual_ | override_ | abstract_,
    explicit_ = 256,
    external = 512,
    suppress = 1024,
    default_ = 2048,
    inline_ = 4096,
    constexpr_ = 8192,
    cdecl_ = 16384,
    nothrow_ = 32768,
    throw_ = 65536,
    new_ = 131072,
    unit_test = 262144,
    non_access_specifiers = static_ | virtuality | explicit_ | external | suppress | default_ | inline_ | constexpr_ | cdecl_ | nothrow_ | throw_ | unit_test
};

inline Specifiers operator|(Specifiers left, Specifiers right)
{
    return Specifiers(uint32_t(left) | uint32_t(right));
}

inline Specifiers operator&(Specifiers left, Specifiers right)
{
    return Specifiers(uint32_t(left) & uint32_t(right));
}

bool HasStaticSpecifier(Specifiers s);
std::string SpecifierStr(Specifiers s);

} } // namespace Cm::Ast

#endif // CM_AST_SPECIFIER_INCLUDED
