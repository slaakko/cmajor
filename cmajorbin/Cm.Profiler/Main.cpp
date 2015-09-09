/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Util/TextUtils.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <boost/filesystem.hpp>
#include <stdio.h>

const char* Version()
{
    return "1.2.0";
}

const uint32_t startFunEvent = 0;
const uint32_t endFunEvent = 1;
const uint32_t flushFid = (uint32_t)-1;

struct ProfileRec
{
    ProfileRec() : elapsed(), fid(), evnt() {}
    ProfileRec(const std::chrono::steady_clock::duration elapsed_, uint32_t fid_, uint32_t evnt_) : elapsed(elapsed_), fid(fid_), evnt(evnt_) {}
    std::chrono::steady_clock::duration elapsed;
    uint32_t fid;
    uint32_t evnt;
};

class Function
{
public:
    Function() : fid(flushFid), name(), called(0), elapsedInclusive(), elapsedChild(), elapsedInclusiveMs(), elapsedExclusiveMs(), percentInclusive(), percentExclusive() {}
    Function(uint32_t fid_, const std::string& name_) : fid(fid_), name(name_), called(0), elapsedInclusive(), elapsedChild(), elapsedInclusiveMs(), elapsedExclusiveMs(), percentInclusive(), percentExclusive() {}
    uint32_t Fid() const { return fid; }
    const std::string& Name() const { return name; }
    int Called() const { return called; }
    void PushTimestamp(const std::chrono::steady_clock::time_point& timestamp) { timestampStack.push(timestamp); }
    std::chrono::steady_clock::time_point PopTimestamp()
    {
        if (timestampStack.empty())
        {
            throw std::runtime_error("timestamp stack empty");
        }
        std::chrono::steady_clock::time_point timestamp = timestampStack.top();
        timestampStack.pop();
        return timestamp;
    }
    const std::chrono::steady_clock::duration& TotalElapsed() const { return totalElapsed; }
    const std::chrono::steady_clock::duration& ElapsedInclusive() const { return elapsedInclusive; }
    const std::chrono::steady_clock::duration& ElapsedChild() const { return elapsedChild; }
    std::chrono::steady_clock::duration ElapsedExclusive() const { return elapsedInclusive - elapsedChild; }
    void IncCalled() { ++called; }
    void AddToElapsedInclusive(const std::chrono::steady_clock::duration& duration)
    {
        elapsedInclusive += duration;
    }
    void AddToElapsedChild(const std::chrono::steady_clock::duration& duration)
    {
        elapsedChild += duration;
    }
    void SetData(uint64_t elapsedInclusiveMs_, uint64_t elapsedExclusiveMs_, int percentInclusive_, int percentExclusive_)
    {
        elapsedInclusiveMs = elapsedInclusiveMs_;
        elapsedExclusiveMs = elapsedExclusiveMs_;
        percentInclusive = percentInclusive_;
        percentExclusive = percentExclusive_;
    }
    uint64_t ElapsedInclusiveMs() const
    {
        return elapsedInclusiveMs;
    }
    uint64_t ElapsedExclusiveMs() const
    {
        return elapsedExclusiveMs;
    }
    int PercentInclusive() const
    {
        return percentInclusive;
    }
    int PercentExclusive() const
    {
        return percentExclusive;
    }
private:
    std::uint32_t fid;
    std::string name;
    int called;
    std::chrono::steady_clock::duration totalElapsed;
    std::chrono::steady_clock::duration elapsedInclusive;
    std::chrono::steady_clock::duration elapsedChild;
    std::stack<std::chrono::steady_clock::time_point> timestampStack;
    uint64_t elapsedInclusiveMs;
    uint64_t elapsedExclusiveMs;
    int percentInclusive;
    int percentExclusive;
};

std::unordered_map<uint32_t, std::unique_ptr<Function>> functionMap;
std::vector<Function*> functionPath;

struct FidLess
{
    bool operator()(Function* left, Function* right) const
    {
        return left->Fid() < right->Fid();
    }
};

struct ElapsedInclusiveGreater
{
    bool operator()(Function* left, Function* right) const
    {
        if (left->ElapsedInclusive() > right->ElapsedInclusive())
        {
            return true;
        }
        else if (left->ElapsedInclusive() < right->ElapsedInclusive())
        {
            return false;
        }
        else if (left->Called() < right->Called())
        {
            return true;
        }
        else if (left->Called() > right->Called())
        {
            return false;
        }
        else if (left->ElapsedExclusive() < right->ElapsedExclusive())
        {
            return true;
        }
        else if (left->ElapsedExclusive() > right->ElapsedExclusive())
        {
            return false;
        }
        else
        {
            return false;
        }
    }
};

struct ElapsedExclusiveGreater
{
    bool operator()(Function* left, Function* right) const
    {
        if (left->ElapsedExclusive() > right->ElapsedExclusive())
        {
            return true;
        }
        else if (left->ElapsedExclusive() < right->ElapsedExclusive())
        {
            return false;
        }
        else if (left->ElapsedInclusive() > right->ElapsedInclusive())
        {
            return true;
        }
        else if (left->ElapsedInclusive() < right->ElapsedInclusive())
        {
            return false;
        }
        else if (left->Called() > right->Called())
        {
            return true;
        }
        else if (left->Called() < right->Called())
        {
            return false;
        }
        else
        {
            return false;
        }
    }
};

struct CalledGreater
{
    bool operator()(Function* left, Function* right) const
    {
        if (left->Called() > right->Called())
        {
            return true;
        }
        else if (left->Called() < right->Called())
        {
            return false;
        }
        else if (left->ElapsedInclusive() > right->ElapsedInclusive())
        {
            return true;
        }
        else if (left->ElapsedInclusive() < right->ElapsedInclusive())
        {
            return false;
        }
        else if (left->ElapsedExclusive() > right->ElapsedExclusive())
        {
            return true;
        }
        else if (left->ElapsedExclusive() < right->ElapsedExclusive())
        {
            return false;
        }
        else
        {
            return false;
        }
    }
};

void WriteProfileReports(const std::string& cmProfRptFileName)
{
    std::chrono::steady_clock::duration totalInclusive = std::chrono::steady_clock::duration();
    std::chrono::steady_clock::duration totalExclusive = std::chrono::steady_clock::duration();
    std::vector<Function*> functions;
    std::unordered_map<uint32_t, std::unique_ptr<Function>>::iterator e = functionMap.end();
    for (std::unordered_map<uint32_t, std::unique_ptr<Function>>::iterator i = functionMap.begin(); i != e; ++i)
    {
        Function* fun = i->second.get();
        if (fun->Called() == 0) continue;
        functions.push_back(fun);
        if (fun->ElapsedInclusive() > totalInclusive)
        {
            totalInclusive = fun->ElapsedInclusive();
        }
        if (fun->ElapsedExclusive() > totalExclusive)
        {
            totalExclusive = fun->ElapsedExclusive();
        }
    }
    uint64_t totalInclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalInclusive).count();
    uint64_t totalExclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalExclusive).count();
    std::sort(functions.begin(), functions.end(), FidLess());
    std::ofstream reportFile(cmProfRptFileName);
    for (Function* fun : functions)
    {
        uint64_t elapsedIclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(fun->ElapsedInclusive()).count();
        uint64_t elapsedExclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(fun->ElapsedExclusive()).count();
        int percentInclusive = int(100.0 * elapsedIclusiveMs / totalInclusiveMs);
        int percentExclusive = int(100.0 * elapsedExclusiveMs / totalExclusiveMs);
        fun->SetData(elapsedIclusiveMs, elapsedExclusiveMs, percentInclusive, percentExclusive);
        reportFile << fun->Fid() << ":" << fun->Name() << ":" << fun->Called() << ":" << elapsedIclusiveMs << ":" << percentInclusive << ":" << elapsedExclusiveMs << ":" << percentExclusive << std::endl;
    }
    std::cout << "=> " << cmProfRptFileName << std::endl;
    std::sort(functions.begin(), functions.end(), CalledGreater());
    std::string countFileName = boost::filesystem::path(cmProfRptFileName).replace_extension(".count.txt").generic_string();
    std::ofstream countFile(countFileName);
    for (Function* fun : functions)
    {
        countFile << fun->Fid() << ":" << fun->Name() << ":" << fun->Called() << ":" << fun->ElapsedInclusiveMs() << ":" << fun->PercentInclusive() << ":" << fun->ElapsedExclusiveMs() << ":" << 
            fun->PercentExclusive() << std::endl;
    }
    std::cout << "=> " << countFileName << std::endl;
    std::sort(functions.begin(), functions.end(), ElapsedInclusiveGreater());
    std::string incFileName = boost::filesystem::path(cmProfRptFileName).replace_extension(".inclusive.txt").generic_string();
    std::ofstream incFile(incFileName);
    for (Function* fun : functions)
    {
        incFile << fun->Fid() << ":" << fun->Name() << ":" << fun->Called() << ":" << fun->ElapsedInclusiveMs() << ":" << fun->PercentInclusive() << ":" << fun->ElapsedExclusiveMs() << ":" <<
            fun->PercentExclusive() << std::endl;
    }
    std::cout << "=> " << incFileName << std::endl;
    std::sort(functions.begin(), functions.end(), ElapsedExclusiveGreater());
    std::string excFileName = boost::filesystem::path(cmProfRptFileName).replace_extension(".exclusive.txt").generic_string();
    std::ofstream excFile(excFileName);
    for (Function* fun : functions)
    {
        excFile << fun->Fid() << ":" << fun->Name() << ":" << fun->Called() << ":" << fun->ElapsedInclusiveMs() << ":" << fun->PercentInclusive() << ":" << fun->ElapsedExclusiveMs() << ":" <<
            fun->PercentExclusive() << std::endl;
    }
    std::cout << "=> " << excFileName << std::endl;
}

class FileHandle
{
public:
    FileHandle(FILE* handle_) : handle(handle_) {}
    ~FileHandle() { if (handle) { fclose(handle); } }
    bool IsOpen() const { return handle != nullptr; }
    operator FILE*() const { return handle; }
private:
    FILE* handle;
};

void ReadProfileData(const std::string& profDataFile)
{
    FileHandle file = fopen(profDataFile.c_str(), "rb");
    if (!file.IsOpen()) 
    {
        throw std::runtime_error("could not open '" + profDataFile + "' for reading");
    }
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::time_point();
    std::unique_ptr<ProfileRec[]> profileRecs(new ProfileRec[(1024 * 1024) / sizeof(ProfileRec)]);
    int n = int(fread(profileRecs.get(), sizeof(ProfileRec), (1024 * 1024) / sizeof(ProfileRec), file));
    while (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            ProfileRec& profileRec = profileRecs.get()[i];
            uint32_t fid = profileRec.fid;
            std::unordered_map<uint32_t, std::unique_ptr<Function>>::iterator f = functionMap.find(fid);
            if (f == functionMap.end())
            {
                throw std::runtime_error("function " + std::to_string(profileRec.fid) + " not found");
            }
            Function* fun = f->second.get();
            std::chrono::steady_clock::duration elapsed = profileRec.elapsed;
            currentTime += elapsed;
            if (profileRec.evnt == startFunEvent)
            {
                fun->IncCalled();
                functionPath.push_back(fun);
                fun->PushTimestamp(currentTime);
            }
            else if (profileRec.evnt == endFunEvent)
            {
                std::chrono::steady_clock::duration funElapsed = currentTime - fun->PopTimestamp();
                fun->AddToElapsedInclusive(funElapsed);
                functionPath.pop_back();
                if (!functionPath.empty())
                {
                    Function* parent = functionPath.back();
                    parent->AddToElapsedChild(funElapsed);
                }
            }
            else
            {
                throw std::runtime_error("invalid profile event detected");
            }
        }
        n = int(fread(profileRecs.get(), sizeof(ProfileRec), (1024 * 1024) / sizeof(ProfileRec), file));
    }
}

void ReadFunctions(const std::string& cmProfFileName)
{
    std::ifstream cmProfFile(cmProfFileName);
    std::string line;
    while (std::getline(cmProfFile, line))
    {
        std::vector<std::string> components;
        components = Cm::Util::Split(line, ':');
        if (components.size() != 2)
        {
            throw std::runtime_error("two fields expected");
        }
        else
        {
            uint32_t fid = uint32_t(std::stoi(components[0]));
            const std::string& functionName = components[1];
            if (functionMap.find(fid) != functionMap.end())
            {
                std::cout << "warning: duplicate fid detected" << std::endl;
            }
            functionMap[fid] = std::unique_ptr<Function>(new Function(fid, functionName));
        }
    }
}

int main(int argc, const char** argv)
{
    try
    {
        if (argc < 2)
        {
            std::cout << "Cmajor profiler version " << Version() << "\n" <<
                "Usage: cmprof file.cmprof\n" << 
                "Analyzes profile data collected by executing instrumented Cmajor program compiled with -config=profile and generates a report to file.cmprof.txt." <<
                std::endl;
        }
        else
        {
            std::string cmProfFileName = argv[1];
            if (!boost::filesystem::exists(cmProfFileName))
            {
                throw std::runtime_error("profile file '" + cmProfFileName + "' not found");
            }
            std::string profDataFile = boost::filesystem::path(cmProfFileName).replace_extension(".profdata").generic_string();
            if (!boost::filesystem::exists(profDataFile))
            {
                throw std::runtime_error("profile data file '" + profDataFile + "' not found, run executable compiled with -config=profile to collect profile data");
            }
            std::string cmProfRptFileName = boost::filesystem::path(cmProfFileName).replace_extension(".cmprofrpt").generic_string();
            functionMap[flushFid] = std::unique_ptr<Function>(new Function(flushFid, "<flush>"));
            ReadFunctions(cmProfFileName);
            ReadProfileData(profDataFile);
            WriteProfileReports(cmProfRptFileName);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}