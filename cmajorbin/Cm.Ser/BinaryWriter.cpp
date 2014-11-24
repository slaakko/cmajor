/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ser/BinaryWriter.hpp>

namespace Cm { namespace Ser {

BinaryWriter::BinaryWriter(const std::string& fileName_) : fileName(fileName_)
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
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(int8_t x)
{
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(uint8_t x)
{
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(int16_t x)
{
    size_t result = fwrite(&x, sizeof(int16_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(uint16_t x)
{
    size_t result = fwrite(&x, sizeof(uint16_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(int32_t x)
{
    size_t result = fwrite(&x, sizeof(int32_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(uint32_t x)
{
    size_t result = fwrite(&x, sizeof(uint32_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(int64_t x)
{
    size_t result = fwrite(&x, sizeof(int64_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(uint64_t x)
{
    size_t result = fwrite(&x, sizeof(uint64_t), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(float x)
{
    size_t result = fwrite(&x, sizeof(float), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(double x)
{
    size_t result = fwrite(&x, sizeof(double), 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(char x)
{
    int result = fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

void BinaryWriter::Write(const std::string& s)
{
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
}

void BinaryWriter::Write(const void* data, int size)
{
    size_t result = fwrite(data, size, 1, file);
    if (result != 1)
    {
        throw std::runtime_error("could not write: " + std::string(strerror(errno)));
    }
}

} } // Cm::Ser
