/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SER_BINARYREADER_INCLUDED
#define CM_SER_BINARYREADER_INCLUDED
#include <Cm.Util/MappedInputFile.hpp>
#include <string>
#include <cstdint>

namespace Cm { namespace Ser {

class BinaryReader
{
public:
    BinaryReader(const std::string& fileName_);
    const std::string& FileName() const { return fileName; }
    bool ReadBool();
    int8_t ReadSByte();
    uint8_t ReadByte();
    int16_t ReadShort();
    uint16_t ReadUShort();
    int32_t ReadInt();
    uint32_t ReadUInt();
    int64_t ReadLong();
    uint64_t ReadULong();
    float ReadFloat();
    double ReadDouble();
    char ReadChar();
    std::string ReadString();
    void Read(void* buf, int size);
    uint64_t GetPos() const { return pos - begin; }
    void SetPos(uint64_t pos_);
    void Skip(uint64_t size);
private:
    std::string fileName;
    Cm::Util::MappedInputFile file;
    const char* begin;
    const char* end;
    const char* pos;
};

} } // Cm::Ser

#endif // CM_SER_BINARYREADER_INCLUDED
