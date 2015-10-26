/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/System.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <iostream>
#include <stdexcept>

// Arguments to this program are:
// 1: handle to redirect
// 2: file name to redirect to handle to
// 3: executable to launch
// 4, 5, .. arguments to executable

int main(int argc, const char** argv)
{
    try
    {
        if (argc < 4)
        {
            std::cout << "stdhandle_redirector <handle_to_redirect> <filename> <executable_to_launch> <arguments_to_executable...>" << std::endl;
        }
        else
        {
            int handle = std::atoi(argv[1]);
            std::string handleFileName = argv[2];
            std::string command(Cm::Util::QuotedPath(argv[3]));
            for (int i = 4; i < argc; ++i)
            {
                command.append(1, ' ').append(Cm::Util::QuotedPath(argv[i]));
            }
            Cm::Util::System(command, handle, handleFileName);
        }
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
