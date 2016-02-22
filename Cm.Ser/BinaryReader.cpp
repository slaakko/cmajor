/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ser/BinaryReader.hpp>
#include <Cm.Ser/BinaryWriter.hpp>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <sstream>

namespace Cm { namespace Ser {

BinaryReader::BinaryReader(const std::string& fileName_): fileName(fileName_), file(fileName), begin(file.Begin()), end(file.End()), pos(begin)
{
}

bool BinaryReader::ReadBool()
{ 
    if (log)
    {
        bool x = *reinterpret_cast<const bool*>(pos);
        Log() << seq << " : bool : " << std::boolalpha << x << std::endl;
        ++seq;
    }
    if (pos == end)
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading bool");
    }
    return *reinterpret_cast<const bool*>(pos++);
}

int8_t BinaryReader::ReadSByte()
{
    if (log)
    {
        int8_t x = *reinterpret_cast<const int8_t*>(pos);
        Log() << seq << " : sbyte : " << int(x) << std::endl;
        ++seq;
    }
    if (pos == end)
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading sbyte");
    }
    return *reinterpret_cast<const int8_t*>(pos++);
}

uint8_t BinaryReader::ReadByte()
{
    if (log)
    {
        uint8_t x = *reinterpret_cast<const uint8_t*>(pos);
        Log() << seq << " : byte : " << int(x) << std::endl;
        if (seq == 126)
        {
            int x = 0;
        }
        ++seq;
    }
    if (pos == end)
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading byte");
    }
    return *reinterpret_cast<const uint8_t*>(pos++);
}

int16_t BinaryReader::ReadShort()
{
    if (log)
    {
        int16_t x = *reinterpret_cast<const int16_t*>(pos);
        Log() << seq << " : short : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(int16_t) <= end)
    {
        int16_t x = *reinterpret_cast<const int16_t*>(pos);
        pos += sizeof(int16_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading short");
    }
}

uint16_t BinaryReader::ReadUShort()
{
    if (log)
    {
        uint16_t x = *reinterpret_cast<const uint16_t*>(pos);
        Log() << seq << " : ushort : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(uint16_t) <= end)
    {
        uint16_t x = *reinterpret_cast<const uint16_t*>(pos);
        pos += sizeof(uint16_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading ushort");
    }
}

int32_t BinaryReader::ReadInt()
{
    if (log)
    {
        int32_t x = *reinterpret_cast<const int32_t*>(pos);
        Log() << seq << " : int : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(int32_t) <= end)
    {
        int32_t x = *reinterpret_cast<const int32_t*>(pos);
        pos += sizeof(int32_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading int");
    }
}

uint32_t BinaryReader::ReadUInt()
{
    if (log)
    {
        uint32_t x = *reinterpret_cast<const uint32_t*>(pos);
        Log() << seq << " : uint : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(uint32_t) <= end)
    {
        uint32_t x = *reinterpret_cast<const uint32_t*>(pos);
        pos += sizeof(uint32_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading uint");
    }
}

int64_t BinaryReader::ReadLong()
{
    if (log)
    {
        int64_t x = *reinterpret_cast<const int64_t*>(pos);
        Log() << seq << " : long : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(int64_t) <= end)
    {
        int64_t x = *reinterpret_cast<const int64_t*>(pos);
        pos += sizeof(int64_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading long");
    }
}

uint64_t BinaryReader::ReadULong()
{
    if (log)
    {
        uint64_t x = *reinterpret_cast<const uint64_t*>(pos);
        Log() << seq << " : ulong : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(uint64_t) <= end)
    {
        uint64_t x = *reinterpret_cast<const uint64_t*>(pos);
        pos += sizeof(uint64_t);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading ulong");
    }
}

float BinaryReader::ReadFloat()
{
    if (log)
    {
        float x = *reinterpret_cast<const float*>(pos);
        Log() << seq << " : float : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(float) <= end)
    {
        float x = *reinterpret_cast<const float*>(pos);
        pos += sizeof(float);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading float");
    }
}

double BinaryReader::ReadDouble()
{
    if (log)
    {
        double x = *reinterpret_cast<const double*>(pos);
        Log() << seq << " : double : " << x << std::endl;
        ++seq;
    }
    if (pos + sizeof(double) <= end)
    {
        double x = *reinterpret_cast<const double*>(pos);
        pos += sizeof(double);
        return x;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading double");
    }
}

char BinaryReader::ReadChar()
{
    if (log)
    {
        char x = *pos;
        Log() << seq << " : char : " << int(x) << std::endl;
        ++seq;
    }
    if (pos == end)
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading char");
    }
    return *pos++;
}

std::string BinaryReader::ReadString()
{
    std::string s;
    while (pos != end && *pos)
    {
        s.append(1, *pos++);
    }
    if (pos != end && !*pos)
    {
        ++pos;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading string");
    }
    if (log)
    {
        Log() << seq << " : string : " << s << std::endl;
        ++seq;
    }
    return s;
}

void BinaryReader::Read(void* buf, int size)
{
    if (log)
    {
        std::string data;
        for (int i = 0; i < size; ++i)
        {
            std::stringstream s;
            s << std::hex << int(((uint8_t*)pos)[i]);
            data.append(s.str());
        }
        Log() << seq << " : data : " << data << " : " << size << std::endl;
        ++seq;
    }
    if (pos + size <= end)
    {
        std::memcpy(buf, pos, size);
        pos += size;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while reading buffer of size " + std::to_string(size));
    }
}

void BinaryReader::SetPos(uint64_t pos_)
{
    pos = begin + pos_;
}

void BinaryReader::Skip(uint64_t size)
{
    if (pos + size <= end)
    {
        pos += size;
    }
    else
    {
        throw std::runtime_error("unexpected end of file '" + fileName + "' while skipping size " + std::to_string(size));
    }
}

} } // Cm::Ser
