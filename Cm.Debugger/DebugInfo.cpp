/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Ser/BinaryReader.hpp>

namespace Cm { namespace Debugger {

DebugInfo::DebugInfo(const std::string& debugInfoFilePath): state(State::idle), mainFunctionDebugInfo(nullptr), currentFunctionDebugInfo(nullptr)
{
    Cm::Ser::BinaryReader reader(debugInfoFilePath);
    int32_t n = reader.ReadInt();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::Core::CDebugInfoFile> file(new Cm::Core::CDebugInfoFile());
        file->Read(reader);
        Process(file.get());
        files.push_back(std::move(file));
    }
    if (!mainFunctionDebugInfo)
    {
        throw std::runtime_error("debug info file '" + debugInfoFilePath + "' contains no debug info for main function");
    }
}

void DebugInfo::SetState(State state_)
{
    state = state_;
    if (state == State::idle)
    {
        //currentPos = SourcePos();
        //posStack.clear();
    }
}

Cm::Core::CFunctionDebugInfo* DebugInfo::GetFunctionDebugInfo(const std::string& mangledFunctionName) const
{
    if (!currentFunctionDebugInfo)
    {
        throw std::runtime_error("current function debug info not set");
    }
    if (!currentFunctionDebugInfo->File())
    {
        throw std::runtime_error("file attribute set of current function debug info not set");
    }
    Cm::Core::CFunctionDebugInfo* functionDebugInfo = currentFunctionDebugInfo->File()->GetFunctionDebugInfo(mangledFunctionName);
    if (!functionDebugInfo)
    {
        UniqueFunctionDebugInfoMapIt i = uniqueFunctionDebugInfoMap.find(mangledFunctionName);
        if (i != uniqueFunctionDebugInfoMap.cend())
        {
            functionDebugInfo = i->second;
        }
    }
    return nullptr;
}

void DebugInfo::Process(Cm::Core::CDebugInfoFile* file)
{
    for (const std::unique_ptr<Cm::Core::CFunctionDebugInfo>& functionDebugInfo : file->FunctionDebugInfos())
    {
        functionDebugInfo->SetFile(file);
        if (functionDebugInfo->IsMain())
        {
            mainFunctionDebugInfo = functionDebugInfo.get();
        }
        else if (functionDebugInfo->IsUnique())
        {
            uniqueFunctionDebugInfoMap[functionDebugInfo->MangledFunctionName()] = functionDebugInfo.get();
        }
        else
        {
            file->AddFunctionDebugInfoToMap(functionDebugInfo.get());
        }
    }
}

} } // Cm::Debugger
