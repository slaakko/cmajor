/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_UTIL_MAPPED_INPUT_FILE_INCLUDED
#define CM_UTIL_MAPPED_INPUT_FILE_INCLUDED
#include <stdint.h>
#include <string>

namespace Cm { namespace Util {

class MappedInputFileImpl;

class MappedInputFile
{
public:
    MappedInputFile(const std::string& fileName_);
    ~MappedInputFile();
    const char* Begin() const;
    const char* End() const;
private:
    MappedInputFileImpl* impl;
};

} } // namespace Cm::Util

#endif // CM_UTIL_MAPPED_INPUT_FILE_INCLUDED
