/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/System.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <iostream>
#include <stdexcept>

int main(int argc, const char** argv)
{
    try
    {
        if (argc < 4)
        {
            std::cout << "stdhandle_redirector { -<handle_to_redirect> <filename_to_redirect_to> }... <executable_to_launch> { <argument_to_executable> }..." << std::endl;
        }
        else
        {
            std::vector<std::pair<int, std::string>> redirections;
            int handle = 1;
            bool prevWasHandle = false;
            bool first = true;
            bool args = false;
            std::string command;
            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (!args && arg[0] == '-')
                {
                    std::string handleStr = arg.substr(1);
                    handle = std::atoi(handleStr.c_str());
                    prevWasHandle = true;
                }
                else if (prevWasHandle)
                {
                    redirections.push_back(std::make_pair(handle, arg));
                    prevWasHandle = false;
                }
                else if (first)
                {
                    command.append(Cm::Util::QuotedPath(arg));
                    first = false;
                    args = true;
                }
                else
                {
                    command.append(" ").append(Cm::Util::QuotedPath(arg));
                }
            }
            Cm::Util::System(command, redirections);
        }
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
