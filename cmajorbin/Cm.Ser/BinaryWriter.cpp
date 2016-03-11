/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ser/BinaryWriter.hpp>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Cm { namespace Ser {

int mode = 0;

std::ofstream logFile;

bool log = false;
int seq = 0;

void BeginLogging(bool read)
{
    log = true;
    seq = 0;
    if (mode == 0)
    {
        if (read)
        {
            logFile.open("C:\\Temp\\read.log");
        }
        else
        {
            logFile.open("C:\\Temp\\write.log");
        }
    }
}

void EndLogging()
{
    log = false;
    if (mode == 0)
    {
        logFile.close();
    }
}

std::ostream& Log()
{
    if (mode == 0)
    {
        return logFile;
    }
    else
    {
        return std::cout;
    }
}

BinaryWriter::BinaryWriter(const std::string& fileName_) : fileName(fileName_), pos(0)
{
    file = fopen(fileName.c_str(), "wb");
    if (!file)
    {
        throw std::runtime_error("could not open '" + fileName + " for writing: " + strerror(errno));
    }
}

BinaryWriter::~BinaryWriter()
{
    if (file)
    {
        fclose(file);
    }
}

void BinaryWriter::Write(bool x)
{
    if (log)
    {
        Log() << seq << " : bool : " << std::boolalpha << x << std::endl;
        ++seq;
    }
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(int8_t x)
{
    if (log)
    {
        Log() << seq << " : sbyte : " << int(x) << std::endl;
        ++seq;
    }
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(uint8_t x)
{
    if (log)
    {
        Log() << seq << " : byte : " << int(x) << std::endl;
        ++seq;
    }
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(int16_t x)
{
    if (log)
    {
        Log() << seq << " : short : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(int16_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(uint16_t x)
{
    if (log)
    {
        Log() << seq << " : ushort : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(uint16_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(int32_t x)
{
    if (log)
    {
        Log() << seq << " : int : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(int32_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(uint32_t x)
{
    if (log)
    {
        Log() << seq << " : uint : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(uint32_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(int64_t x)
{
    if (log)
    {
        Log() << seq << " : long : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(int64_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(uint64_t x)
{
    if (log)
    {
        Log() << seq << " : ulong : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(uint64_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(float x)
{
    if (log)
    {
        Log() << seq << " : float : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(float), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(double x)
{
    if (log)
    {
        Log() << seq << " : double : " << x << std::endl;
        ++seq;
    }
    size_t result = fwrite(&x, sizeof(double), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(char x)
{
    if (log)
    {
        Log() << seq << " : char : " << int(x) << std::endl;
        ++seq;
    }
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += sizeof(x);
}

void BinaryWriter::Write(const std::string& s)
{
    if (log)
    {
        Log() << seq << " : string : " << s << std::endl;
        ++seq;
    }
    for (char c : s)
    {
        int result = fputc(c, file);
        if (result == EOF)
        {
            throw std::runtime_error("could not write: " + std::string(strerror(errno)));
        }
    }
    int result = fputc(0, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += s.length() + 1;
}

void BinaryWriter::Write(const void* data, int size)
{
    if (log)
    {
        std::string datas;
        for (int i = 0; i < size; ++i)
        {
            std::stringstream s;
            s << std::hex << int(((uint8_t*)data)[i]);
            datas.append(s.str());
        }
        Log() << seq << " : data : " << datas << " : " << size << std::endl;
        ++seq;
    }
    size_t result = fwrite(data, size, 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
    pos += size;
}

void BinaryWriter::Seek(uint64_t pos_)
{
    int result = fseek(file, static_cast<long>(pos_), SEEK_SET);
    if (result != 0)
    {
        throw std::runtime_error("could not seek: " + std::string(strerror(errno)));
    }
    pos = pos_;
}

} } // Cm::Ser
