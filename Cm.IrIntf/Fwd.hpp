/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_IRINTF_FWD_INCLUDED
#define CM_IRINTF_FWD_INCLUDED

#include <Ir.Intf/Function.hpp>

namespace Cm { namespace IrIntf {

Ir::Intf::LabelObject* CreateTempLabel(int tempLabelCounter);
std::string CreateTempVarName(int tempVarCounter);

} } // namespace Cm::IrIntf

#endif // CM_IRINTF_FWD_INCLUDED
