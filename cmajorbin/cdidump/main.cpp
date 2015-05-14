#include <Cm.Core/CDebugInfo.hpp>
#include <Cm.Ser/BinaryReader.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

int main(int argc, const char** argv)
{
    try
    {
        if (argc < 2)
        {
            std::cout << "Usage: cdidump <file.cdi>" << std::endl;
            return 1;
        }
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            boost::filesystem::path a(arg);
            if (a.extension() != ".cdi")
            {
                throw std::runtime_error("unknown extension '" + a.extension().generic_string() + "'");
            }
            if (!exists(a))
            {
                throw std::runtime_error("file '" + arg + " does not exist");
            }
            Cm::Ser::BinaryReader reader(a.generic_string());
            Cm::Core::CDebugInfoFile debugInfoFile;
            debugInfoFile.Read(reader);
            Cm::Util::CodeFormatter formatter(std::cout);
            debugInfoFile.Dump(formatter);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}