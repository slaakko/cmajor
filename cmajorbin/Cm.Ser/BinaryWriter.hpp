/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SER_BINARYWRITER_INCLUDED
#define CM_SER_BINARYWRITER_INCLUDED
#include <string>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace Cm { namespace Ser {

class BinaryWriter
{
public:
    BinaryWriter(const std::string& fileName_);
    ~BinaryWriter();
    void Write(bool x);
    void Write(int8_t x);
    void Write(uint8_t x);
    void Write(int16_t x);
    void Write(uint16_t x);
    void Write(int32_t x);
    void Write(uint32_t x);
    void Write(int64_t x);
    void Write(uint64_t x);
    void Write(float x);
    void Write(double x);
    void Write(char x);
    void Write(const std::string& s);
    void Write(const void* data, int size);
    uint64_t Pos() const { return pos; }
    void Seek(uint64_t pos);
private:
    std::string fileName;
    FILE* file;
    uint64_t pos;
};

} } // Cm::Ser

#endif // CM_SER_BINARYWRITER_INCLUDED
