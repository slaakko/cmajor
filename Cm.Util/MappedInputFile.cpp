/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Util/MappedInputFile.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace Cm { namespace Util {

class MappedInputFileImpl
{
public:
    MappedInputFileImpl(const std::string& fileName_);
    const char* Data() const { return mappedFile.data(); }
    boost::iostreams::mapped_file_source::size_type Size() const { return mappedFile.size(); }
private:
    boost::iostreams::mapped_file_source mappedFile;
};

MappedInputFileImpl::MappedInputFileImpl(const std::string& fileName_): mappedFile(fileName_)
{
}

MappedInputFile::MappedInputFile(const std::string& fileName_): impl(new MappedInputFileImpl(fileName_))
{
}

MappedInputFile::~MappedInputFile()
{
    delete impl;
}

const char* MappedInputFile::Begin() const
{
    return impl->Data();
}

uint64_t MappedInputFile::Size() const
{
    return impl->Size();
}

} } // namespace Cm::Util
