/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Opt/ClassId.hpp>
#include <Cm.Opt/TypePropagationGraph.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Util/Prime.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>

namespace Cm { namespace Opt {

int NumberOfAncestors(Cm::Sym::ClassTypeSymbol* cls)
{
    int numAncestors = 0;
    Cm::Sym::ClassTypeSymbol* baseClass = cls->BaseClass();
    while (baseClass != nullptr)
    {
        ++numAncestors;
        baseClass = baseClass->BaseClass();
    }
    return numAncestors;
}

void AssignPriorities(std::vector<Cm::Sym::ClassTypeSymbol*>& leaves)
{
    for (Cm::Sym::ClassTypeSymbol* leaf : leaves)
    {
        int priority = leaf->Level();
        leaf->SetPriority(priority);
        Cm::Sym::ClassTypeSymbol* base = leaf->BaseClass();
        while (base)
        {
            if (base->Priority() < priority)
            {
                base->SetPriority(priority);
            }
            else
            {
                priority = base->Priority();
            }
            base = base->BaseClass();
        }
    }
}

struct PriorityGreater
{
    bool operator()(Cm::Sym::ClassTypeSymbol* left, Cm::Sym::ClassTypeSymbol* right) const
    {
        if (left->Level() < right->Level())
        {
            return true;
        }
        else if (right->Level() < left->Level())
        {
            return false;
        }
        else if (left->Priority() > right->Priority())
        {
            return true;
        }
        else if (right->Priority() > left->Priority())
        {
            return false;
        }
        else
        {
            uint64_t leftCid = 0;
            uint64_t rightCid = 0;
            if (left->BaseClass())
            {
                leftCid = left->BaseClass()->Cid();
            }
            if (right->BaseClass())
            {
                rightCid = right->BaseClass()->Cid();
            }
            return leftCid < rightCid;
        }
    }
};

void AssignKeys(std::vector<Cm::Sym::ClassTypeSymbol*>& classesByPriority)
{
    uint64_t key = 2;
    uint64_t minLevelKey = key;
    uint64_t maxLevelKey = key;
    int predLevel = -1;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> bases;
    for (Cm::Sym::ClassTypeSymbol* cls : classesByPriority)
    {
        int level = cls->Level();
        if (level == 0)
        {
            cls->SetKey(key);
            key = Cm::Util::NextPrime(key + 1);
            maxLevelKey = key;
        }
        else
        {
            if (predLevel != level)
            {
                bases.clear();
                bases.insert(cls->BaseClass());
                key = Cm::Util::NextPrime(maxLevelKey + 1);
                minLevelKey = key;
                cls->SetKey(key);
                key = Cm::Util::NextPrime(key + 1);
                maxLevelKey = key;
            }
            else
            {
                if (bases.find(cls->BaseClass()) == bases.end())
                {
                    key = minLevelKey;
                }
                bases.insert(cls->BaseClass());
                cls->SetKey(key);
                key = Cm::Util::NextPrime(key + 1);
                if (key > maxLevelKey)
                {
                    maxLevelKey = key;
                }
            }
            predLevel = level;
        }
    }
}

uint64_t ComputeCid(Cm::Sym::ClassTypeSymbol* cls)
{
    uint64_t cid = cls->Key();
    Cm::Sym::ClassTypeSymbol* base = cls->BaseClass();
    while (base)
    {
        cid *= base->Key();
        base = base->BaseClass();
    }
    return cid;
}

void AssignCids(std::vector<Cm::Sym::ClassTypeSymbol*>& classesByPriority)
{
    for (Cm::Sym::ClassTypeSymbol* cls : classesByPriority)
    {
        cls->SetCid(ComputeCid(cls));
    }
}

void ProcessClasses(const std::unordered_set<Cm::Sym::ClassTypeSymbol*>& classes)
{
    std::vector<Cm::Sym::ClassTypeSymbol*> virtualClasses;
    for (Cm::Sym::ClassTypeSymbol* cls : classes)
    {
        if (cls->IsVirtual())
        {
            virtualClasses.push_back(cls);
        }
    }
    for (Cm::Sym::ClassTypeSymbol* cls : virtualClasses)
    {
        cls->SetLevel(NumberOfAncestors(cls));
        if (cls->BaseClass())
        {
            cls->BaseClass()->SetNonLeaf();
        }
    }
    std::vector<Cm::Sym::ClassTypeSymbol*> leaves;
    for (Cm::Sym::ClassTypeSymbol* cls : virtualClasses)
    {
        if (!cls->IsNonLeaf())
        {
            leaves.push_back(cls);
        }
    }
    AssignPriorities(leaves);
    std::vector<Cm::Sym::ClassTypeSymbol*> classesByPriority;
    for (Cm::Sym::ClassTypeSymbol* cls : virtualClasses)
    {
        classesByPriority.push_back(cls);
    }
    std::sort(classesByPriority.begin(), classesByPriority.end(), PriorityGreater());
    AssignKeys(classesByPriority);
    AssignCids(classesByPriority);
    std::string classHierarchyDotFileName = Cm::Core::GetGlobalSettings()->ClassHierarchyDotFileName();
    if (!classHierarchyDotFileName.empty())
    {
        if (boost::filesystem::path(classHierarchyDotFileName).extension().empty())
        {
            classHierarchyDotFileName.append(".dot");
        }
        std::ofstream classHierarchyDotFile(classHierarchyDotFileName);
        Cm::Util::CodeFormatter formatter(classHierarchyDotFile);
        formatter.WriteLine("digraph class_hierarchy");
        formatter.WriteLine("{");
        formatter.IncIndent();
        formatter.WriteLine("graph [rankdir=RL];");
        formatter.WriteLine("node [shape=record];");
        for (Cm::Sym::ClassTypeSymbol* cls : classesByPriority)
        {
            formatter.WriteLine("node" + std::to_string(cls->Cid()) + " [label=\"" + Cm::Opt::ToNodeLabel(cls->FullName()) + "|" + std::to_string(cls->Key()) + "|" + std::to_string(cls->Cid()) + "|" + (cls->IsLive() ? "+" : "-") + "\"];");
            if (cls->BaseClass())
            {
                formatter.WriteLine("node" + std::to_string(cls->Cid()) + " -> node" + std::to_string(cls->BaseClass()->Cid()));
            }
        }
        formatter.DecIndent();
        formatter.WriteLine("}");
    }
}

} } // namespace Cm::Opt
