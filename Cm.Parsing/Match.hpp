/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_MATCH_INCLUDED
#define CM_PARSING_MATCH_INCLUDED

namespace Cm { namespace Parsing {

class Match
{
public:
    Match(bool hit_, int length_): hit(hit_), length(length_) {}
    bool Hit() const { return hit; }
    int Length() const { return length; }
    void Concatenate(const Match& that)
    {
        length += that.length;
    }
    static Match Empty() 
    {
        return Match(true, 0);
    }
    static Match One()
    {
        return Match(true, 1);
    }
    static Match Nothing()
    {
        return Match(false, -1);
    }
private:
    bool hit;
    int length;
};

} } // namespace Cm::Parsing

#endif // CM_PARSING_MATCH_INCLUDED

