/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    return "1.3.0";
}

const uint32_t startFunEvent = 0;
const uint32_t endFunEvent = 1;
const uint32_t startProfilingEvent = 2;
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
    Function() : fid(flushFid), name(), called(0), recursionCount(0), childCalled(false), elapsedInclusive(), elapsedExclusive(), elapsedInclusiveMs(), elapsedExclusiveMs(), percentInclusive(), percentExclusive() {}
    Function(uint32_t fid_, const std::string& name_) : fid(fid_), name(name_), called(0), recursionCount(0), childCalled(false), elapsedInclusive(), elapsedExclusive(), elapsedInclusiveMs(), elapsedExclusiveMs(), percentInclusive(), percentExclusive() {}
    uint32_t Fid() const { return fid; }
    const std::string& Name() const { return name; }
    int Called() const { return called; }
    void Start(const std::chrono::steady_clock::time_point& start_) 
    {
        childCalled = false;
        if (recursionCount == 0)
        {
            start = start_;
        }
        ++recursionCount;
    }
    void Stop(const std::chrono::steady_clock::time_point& stop)
    {
        --recursionCount;
        if (recursionCount == 0)
        {
            elapsedInclusive += (stop - start);
        }
    }
    const std::chrono::steady_clock::duration& ElapsedInclusive() const { return elapsedInclusive; }
    std::chrono::steady_clock::duration ElapsedExclusive() const { return elapsedExclusive; }
    void IncCalled() { ++called; }
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
    int RecursionCount() const
    {
        return recursionCount;
    }
    bool ChildCalled() const
    {
        return childCalled;
    }
    void SetChildCalled()
    {
        childCalled = true;
    }
    std::chrono::steady_clock::time_point StartTime() const
    {
        return start;
    }
    void AddToElapsedExclusive(const std::chrono::steady_clock::duration& elapsed)
    {
        elapsedExclusive += elapsed;
    }
private:
    std::uint32_t fid;
    std::string name;
    int called;
    int recursionCount;
    bool childCalled;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::duration elapsedInclusive;
    std::chrono::steady_clock::duration elapsedExclusive;
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
    }
    uint64_t totalInclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalInclusive).count();
    std::sort(functions.begin(), functions.end(), FidLess());
    std::ofstream reportFile(cmProfRptFileName);
    for (Function* fun : functions)
    {
        uint64_t elapsedIclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(fun->ElapsedInclusive()).count();
        uint64_t elapsedExclusiveMs = std::chrono::duration_cast<std::chrono::milliseconds>(fun->ElapsedExclusive()).count();
        int percentInclusive = int(100.0 * elapsedIclusiveMs / totalInclusiveMs);
        int percentExclusive = int(100.0 * elapsedExclusiveMs / totalInclusiveMs);
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
    uint32_t prevEvent = startProfilingEvent;
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
            uint32_t evnt = profileRec.evnt;
            if (evnt == startFunEvent)
            {
                if (!functionPath.empty())
                {
                    Function* parent = functionPath.back();
                    parent->AddToElapsedExclusive(elapsed);
                    parent->SetChildCalled();
                }
                fun->IncCalled();
                functionPath.push_back(fun);
                fun->Start(currentTime);
            }
            else if (profileRec.evnt == endFunEvent)
            {
                if (!functionPath.empty())
                {
                    Function* parent = functionPath.back();
                    if (prevEvent == endFunEvent)
                    {
                        parent->AddToElapsedExclusive(elapsed);
                    }
                }
                fun->Stop(currentTime);
                functionPath.pop_back();
                if (!fun->ChildCalled())
                {
                    fun->AddToElapsedExclusive(elapsed);
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
                "Analyzes profile data collected by executing instrumented Cmajor program compiled with -config=profile and generates a profiling reports." <<
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
            std::string cmProfReportFileName = boost::filesystem::path(cmProfFileName).replace_extension(".cmprofreport").generic_string();
            functionMap[flushFid] = std::unique_ptr<Function>(new Function(flushFid, "<flush>"));
            ReadFunctions(cmProfFileName);
            ReadProfileData(profDataFile);
            WriteProfileReports(cmProfReportFileName);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}