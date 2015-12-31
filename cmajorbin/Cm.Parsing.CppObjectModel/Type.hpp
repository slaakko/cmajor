/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_CPPOBJECTMODEL_TYPE_INCLUDED
#define CM_PARSING_CPPOBJECTMODEL_TYPE_INCLUDED
#include <Cm.Parsing.CppObjectModel/Object.hpp>

namespace Cm { namespace Parsing { namespace CppObjectModel {

class DeclSpecifier : public CppObject
{
public:
    DeclSpecifier(const std::string& name_);
};

class Typedef : public DeclSpecifier
{
public:
    Typedef();
    virtual void Accept(Visitor& visitor);
};

class TypeSpecifier : public DeclSpecifier
{
public:
    TypeSpecifier(const std::string& name_);
    virtual void Accept(Visitor& visitor);
};

class Const : public TypeSpecifier
{
public:
    Const();
};

class Volatile: public TypeSpecifier
{
public:
    Volatile();
};

class TypeName : public TypeSpecifier
{
public:
    TypeName(const std::string& name_);
    void AddTemplateArgument(CppObject* templateArgument);
    bool IsTemplate() const { return isTemplate; }
    bool& IsTemplate() { return isTemplate; }
    virtual std::string ToString() const;
    virtual void Print(CodeFormatter& formatter);
    virtual void Accept(Visitor& visitor);
private:
    bool isTemplate;
    std::vector<CppObject*> templateArguments;
};

class Type : public CppObject
{
public:
    Type();
    void Add(TypeSpecifier* typeSpecifier);
    virtual std::string ToString() const;
    virtual void Print(CodeFormatter& formatter);
    virtual void Accept(Visitor& visitor);
private:
    std::vector<TypeSpecifier*> typeSpecifiers;
};

typedef std::shared_ptr<Type> TypePtr;

class TypeId : public CppObject
{
public:
    typedef std::vector<TypeSpecifier*> TypeSpecifierVec;
    TypeId();
    void Add(TypeSpecifier* typeSpecifier);
    const TypeSpecifierVec& TypeSpecifiers() const { return typeSpecifiers; }
    TypeSpecifierVec& TypeSpecifiers() { return typeSpecifiers; }
    const std::string& Declarator() const { return declarator; }
    std::string& Declarator() { return declarator; }
    virtual std::string ToString() const;
    virtual void Print(CodeFormatter& formatter);
    virtual void Accept(Visitor& visitor);
private:
    TypeSpecifierVec typeSpecifiers;
    std::string declarator;
};

class StorageClassSpecifier : public DeclSpecifier
{
public:
    StorageClassSpecifier(const std::string& name_): DeclSpecifier(name_) {}
    virtual void Accept(Visitor& visitor);
};

} } } // namespace Cm::Parsing::CppObjectModel

#endif // CM_PARSING_CPPOBJECTMODEL_TYPE_INCLUDED
