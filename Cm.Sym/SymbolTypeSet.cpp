/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/SymbolTypeSet.hpp>

namespace Cm { namespace Sym {

SymbolTypeSetCollection::SymbolTypeSetCollection()
{
    symbolTypeSets.resize(int(SymbolTypeSetId::max));
//  lookupAllSymbols:
    SymbolTypeSet allSymbols;
    for (uint8_t s = 0; s < uint8_t(SymbolType::maxSymbol); ++s)
    {
        allSymbols.insert(SymbolType(s));
    }
    symbolTypeSets[int(SymbolTypeSetId::lookupAllSymbols)] = std::move(allSymbols);
//  lookupTypeSymbols:
    SymbolTypeSet typeSymbols;
    typeSymbols.insert(SymbolType::boolSymbol);
    typeSymbols.insert(SymbolType::charSymbol);
    typeSymbols.insert(SymbolType::voidSymbol);
    typeSymbols.insert(SymbolType::sbyteSymbol);
    typeSymbols.insert(SymbolType::byteSymbol);
    typeSymbols.insert(SymbolType::shortSymbol);
    typeSymbols.insert(SymbolType::ushortSymbol);
    typeSymbols.insert(SymbolType::intSymbol);
    typeSymbols.insert(SymbolType::uintSymbol);
    typeSymbols.insert(SymbolType::longSymbol);
    typeSymbols.insert(SymbolType::ulongSymbol);
    typeSymbols.insert(SymbolType::floatSymbol);
    typeSymbols.insert(SymbolType::doubleSymbol);
    typeSymbols.insert(SymbolType::classSymbol);
    typeSymbols.insert(SymbolType::delegateSymbol);
    typeSymbols.insert(SymbolType::classDelegateSymbol);
    typeSymbols.insert(SymbolType::enumTypeSymbol);
    typeSymbols.insert(SymbolType::templateTypeSymbol);
    typeSymbols.insert(SymbolType::derivedTypeSymbol);
    typeSymbols.insert(SymbolType::typedefSymbol);
    typeSymbols.insert(SymbolType::boundTypeParameterSymbol);
    typeSymbols.insert(SymbolType::typeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupTypeSymbols)] = std::move(typeSymbols);
//  lookupTypeAndFunctionGroupSymbols:
    SymbolTypeSet typeAndFunctionGroupSymbols;
    typeAndFunctionGroupSymbols.insert(SymbolType::boolSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::charSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::voidSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::sbyteSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::byteSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::shortSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::ushortSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::intSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::uintSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::longSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::ulongSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::floatSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::doubleSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::classSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::delegateSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::classDelegateSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::enumTypeSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::templateTypeSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::derivedTypeSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::typedefSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::boundTypeParameterSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::typeParameterSymbol);
    typeAndFunctionGroupSymbols.insert(SymbolType::functionGroupSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupTypeAndFunctionGroupSymbols)] = std::move(typeAndFunctionGroupSymbols);
    //  lookupClassEnumAndNamespaceSymbols:
    SymbolTypeSet dotSubjectSymbols;
    dotSubjectSymbols.insert(SymbolType::classSymbol);
    dotSubjectSymbols.insert(SymbolType::typedefSymbol);
    dotSubjectSymbols.insert(SymbolType::enumTypeSymbol);
    dotSubjectSymbols.insert(SymbolType::namespaceSymbol);
    dotSubjectSymbols.insert(SymbolType::templateTypeSymbol);
    dotSubjectSymbols.insert(SymbolType::typeParameterSymbol);
    dotSubjectSymbols.insert(SymbolType::boundTypeParameterSymbol);
    dotSubjectSymbols.insert(SymbolType::localVariableSymbol);
    dotSubjectSymbols.insert(SymbolType::memberVariableSymbol);
    dotSubjectSymbols.insert(SymbolType::parameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupDotSubjectSymbols)] = std::move(dotSubjectSymbols);
//  lookupContainerSymbols:
    SymbolTypeSet containerSymbols;
    containerSymbols.insert(SymbolType::classSymbol);
    containerSymbols.insert(SymbolType::typedefSymbol);
    containerSymbols.insert(SymbolType::enumTypeSymbol);
    containerSymbols.insert(SymbolType::namespaceSymbol);
    containerSymbols.insert(SymbolType::templateTypeSymbol);
    containerSymbols.insert(SymbolType::typeParameterSymbol);
    containerSymbols.insert(SymbolType::boundTypeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupContainerSymbols)] = std::move(containerSymbols);
//  lookupClassAndNamespaceSymbols:
    SymbolTypeSet classAndNamespaceSymbols;
    classAndNamespaceSymbols.insert(SymbolType::classSymbol);
    classAndNamespaceSymbols.insert(SymbolType::typedefSymbol);
    classAndNamespaceSymbols.insert(SymbolType::namespaceSymbol);
    classAndNamespaceSymbols.insert(SymbolType::templateTypeSymbol);
    classAndNamespaceSymbols.insert(SymbolType::typeParameterSymbol);
    classAndNamespaceSymbols.insert(SymbolType::boundTypeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupClassAndNamespaceSymbols)] = std::move(classAndNamespaceSymbols);
//  lookupClassSymbols:
    SymbolTypeSet classSymbols;
    classSymbols.insert(SymbolType::classSymbol);
    classSymbols.insert(SymbolType::typedefSymbol);
    classSymbols.insert(SymbolType::templateTypeSymbol);
    classSymbols.insert(SymbolType::typeParameterSymbol);
    classSymbols.insert(SymbolType::boundTypeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupClassSymbols)] = std::move(classSymbols);
//  lookupFunctionGroupAndMemberVariable:
    SymbolTypeSet functionGroupAndMemberVariableSymbols;
    functionGroupAndMemberVariableSymbols.insert(SymbolType::functionGroupSymbol);
    functionGroupAndMemberVariableSymbols.insert(SymbolType::memberVariableSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupFunctionGroupAndMemberVariable)] = std::move(functionGroupAndMemberVariableSymbols);
//  lookupLocalVariable:
    SymbolTypeSet localVariableSymbols;
    localVariableSymbols.insert(SymbolType::localVariableSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupLocalVariable)] = std::move(localVariableSymbols);
//  lookupMemberVariable:
    SymbolTypeSet memberVariableSymbols;
    memberVariableSymbols.insert(SymbolType::memberVariableSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupMemberVariable)] = std::move(memberVariableSymbols);
//  lookupParameter:
    SymbolTypeSet parameterSymbols;
    parameterSymbols.insert(SymbolType::parameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupParameter)] = std::move(parameterSymbols);
//  lookupTypedef:
    SymbolTypeSet typedefSymbols;
    typedefSymbols.insert(SymbolType::typedefSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupTypedef)] = std::move(typedefSymbols);
//  lookupVariableAndParameter:
    SymbolTypeSet variableAndParameterSymbols;
    variableAndParameterSymbols.insert(SymbolType::localVariableSymbol);
    variableAndParameterSymbols.insert(SymbolType::memberVariableSymbol);
    variableAndParameterSymbols.insert(SymbolType::parameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupVariableAndParameter)] = std::move(variableAndParameterSymbols);
//  lookupInvokeSubject
    SymbolTypeSet invokeSubjectSymbols;
    invokeSubjectSymbols.insert(SymbolType::boolSymbol);
    invokeSubjectSymbols.insert(SymbolType::charSymbol);
    invokeSubjectSymbols.insert(SymbolType::voidSymbol);
    invokeSubjectSymbols.insert(SymbolType::sbyteSymbol);
    invokeSubjectSymbols.insert(SymbolType::byteSymbol);
    invokeSubjectSymbols.insert(SymbolType::shortSymbol);
    invokeSubjectSymbols.insert(SymbolType::ushortSymbol);
    invokeSubjectSymbols.insert(SymbolType::intSymbol);
    invokeSubjectSymbols.insert(SymbolType::uintSymbol);
    invokeSubjectSymbols.insert(SymbolType::longSymbol);
    invokeSubjectSymbols.insert(SymbolType::ulongSymbol);
    invokeSubjectSymbols.insert(SymbolType::floatSymbol);
    invokeSubjectSymbols.insert(SymbolType::doubleSymbol);
    invokeSubjectSymbols.insert(SymbolType::classSymbol);
    invokeSubjectSymbols.insert(SymbolType::delegateSymbol);
    invokeSubjectSymbols.insert(SymbolType::classDelegateSymbol);
    invokeSubjectSymbols.insert(SymbolType::enumTypeSymbol);
    invokeSubjectSymbols.insert(SymbolType::templateTypeSymbol);
    invokeSubjectSymbols.insert(SymbolType::derivedTypeSymbol);
    invokeSubjectSymbols.insert(SymbolType::typedefSymbol);
    invokeSubjectSymbols.insert(SymbolType::boundTypeParameterSymbol);
    invokeSubjectSymbols.insert(SymbolType::typeParameterSymbol);
    invokeSubjectSymbols.insert(SymbolType::functionGroupSymbol);
    invokeSubjectSymbols.insert(SymbolType::localVariableSymbol);
    invokeSubjectSymbols.insert(SymbolType::memberVariableSymbol);
    invokeSubjectSymbols.insert(SymbolType::parameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupInvokeSubject)] = std::move(invokeSubjectSymbols);
//  lookupSizeOfSubject:
    SymbolTypeSet sizeOfSubjectSymbols;
    sizeOfSubjectSymbols.insert(SymbolType::localVariableSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::memberVariableSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::parameterSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::boolSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::charSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::voidSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::sbyteSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::byteSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::shortSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::ushortSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::intSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::uintSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::longSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::ulongSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::floatSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::doubleSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::classSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::delegateSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::classDelegateSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::enumTypeSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::templateTypeSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::derivedTypeSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::typedefSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::boundTypeParameterSymbol);
    sizeOfSubjectSymbols.insert(SymbolType::typeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupSizeOfSubject)] = std::move(sizeOfSubjectSymbols);
//  lookupCastSource:
    SymbolTypeSet castSourceSymbols;
    castSourceSymbols.insert(SymbolType::localVariableSymbol);
    castSourceSymbols.insert(SymbolType::memberVariableSymbol);
    castSourceSymbols.insert(SymbolType::parameterSymbol);
    castSourceSymbols.insert(SymbolType::constantSymbol);
    castSourceSymbols.insert(SymbolType::enumConstantSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupCastSource)] = std::move(castSourceSymbols);
//  lookupArgumentSymbol:
    SymbolTypeSet argumentSymbols;
    argumentSymbols.insert(SymbolType::localVariableSymbol);
    argumentSymbols.insert(SymbolType::memberVariableSymbol);
    argumentSymbols.insert(SymbolType::parameterSymbol);
    argumentSymbols.insert(SymbolType::constantSymbol);
    argumentSymbols.insert(SymbolType::enumConstantSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupArgumentSymbol)] = std::move(argumentSymbols);
//  lookupTypenameSubject:
    SymbolTypeSet typenameSubjectSymbols;
    typenameSubjectSymbols.insert(SymbolType::localVariableSymbol);
    typenameSubjectSymbols.insert(SymbolType::memberVariableSymbol);
    typenameSubjectSymbols.insert(SymbolType::parameterSymbol);
    typenameSubjectSymbols.insert(SymbolType::constantSymbol);
    typenameSubjectSymbols.insert(SymbolType::enumConstantSymbol);
    typenameSubjectSymbols.insert(SymbolType::boolSymbol);
    typenameSubjectSymbols.insert(SymbolType::charSymbol);
    typenameSubjectSymbols.insert(SymbolType::voidSymbol);
    typenameSubjectSymbols.insert(SymbolType::sbyteSymbol);
    typenameSubjectSymbols.insert(SymbolType::byteSymbol);
    typenameSubjectSymbols.insert(SymbolType::shortSymbol);
    typenameSubjectSymbols.insert(SymbolType::ushortSymbol);
    typenameSubjectSymbols.insert(SymbolType::intSymbol);
    typenameSubjectSymbols.insert(SymbolType::uintSymbol);
    typenameSubjectSymbols.insert(SymbolType::longSymbol);
    typenameSubjectSymbols.insert(SymbolType::ulongSymbol);
    typenameSubjectSymbols.insert(SymbolType::floatSymbol);
    typenameSubjectSymbols.insert(SymbolType::doubleSymbol);
    typenameSubjectSymbols.insert(SymbolType::classSymbol);
    typenameSubjectSymbols.insert(SymbolType::delegateSymbol);
    typenameSubjectSymbols.insert(SymbolType::classDelegateSymbol);
    typenameSubjectSymbols.insert(SymbolType::enumTypeSymbol);
    typenameSubjectSymbols.insert(SymbolType::templateTypeSymbol);
    typenameSubjectSymbols.insert(SymbolType::derivedTypeSymbol);
    typenameSubjectSymbols.insert(SymbolType::typedefSymbol);
    typenameSubjectSymbols.insert(SymbolType::boundTypeParameterSymbol);
    typenameSubjectSymbols.insert(SymbolType::typeParameterSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupTypenameSubject)] = std::move(typenameSubjectSymbols);
//  lookupFunctionGroup:
    SymbolTypeSet functionGroupSymbols;
    functionGroupSymbols.insert(SymbolType::functionGroupSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupFunctionGroup)] = std::move(functionGroupSymbols);
//  lookupNamespace:
    SymbolTypeSet namespaceSymbols;
    namespaceSymbols.insert(SymbolType::namespaceSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupNamespace)] = std::move(namespaceSymbols);
//  lookupTypeAndConceptSymbols:
    SymbolTypeSet typeAndConceptSymbols;
    typeAndConceptSymbols.insert(SymbolType::boolSymbol);
    typeAndConceptSymbols.insert(SymbolType::charSymbol);
    typeAndConceptSymbols.insert(SymbolType::voidSymbol);
    typeAndConceptSymbols.insert(SymbolType::sbyteSymbol);
    typeAndConceptSymbols.insert(SymbolType::byteSymbol);
    typeAndConceptSymbols.insert(SymbolType::shortSymbol);
    typeAndConceptSymbols.insert(SymbolType::ushortSymbol);
    typeAndConceptSymbols.insert(SymbolType::intSymbol);
    typeAndConceptSymbols.insert(SymbolType::uintSymbol);
    typeAndConceptSymbols.insert(SymbolType::longSymbol);
    typeAndConceptSymbols.insert(SymbolType::ulongSymbol);
    typeAndConceptSymbols.insert(SymbolType::floatSymbol);
    typeAndConceptSymbols.insert(SymbolType::doubleSymbol);
    typeAndConceptSymbols.insert(SymbolType::classSymbol);
    typeAndConceptSymbols.insert(SymbolType::delegateSymbol);
    typeAndConceptSymbols.insert(SymbolType::classDelegateSymbol);
    typeAndConceptSymbols.insert(SymbolType::enumTypeSymbol);
    typeAndConceptSymbols.insert(SymbolType::templateTypeSymbol);
    typeAndConceptSymbols.insert(SymbolType::derivedTypeSymbol);
    typeAndConceptSymbols.insert(SymbolType::typedefSymbol);
    typeAndConceptSymbols.insert(SymbolType::boundTypeParameterSymbol);
    typeAndConceptSymbols.insert(SymbolType::typeParameterSymbol);
    typeAndConceptSymbols.insert(SymbolType::conceptGroupSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupTypeAndConceptSymbols)] = std::move(typeAndConceptSymbols);
//  lookupConceptGroup:
    SymbolTypeSet conceptGroupSymbols;
    conceptGroupSymbols.insert(SymbolType::conceptGroupSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupConceptGroup)] = std::move(conceptGroupSymbols);
//  lookupConstant:
    SymbolTypeSet constantSymbols;
    constantSymbols.insert(SymbolType::constantSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupConstant)] = std::move(constantSymbols);
//  lookupDelegate:
    SymbolTypeSet delegateSymbols;
    delegateSymbols.insert(SymbolType::delegateSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupDelegate)] = std::move(delegateSymbols);
//  lookupClassDelegate:
    SymbolTypeSet classDelegateSymbols;
    classDelegateSymbols.insert(SymbolType::classDelegateSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupClassDelegate)] = std::move(classDelegateSymbols);
//  lookupEnumType:
    SymbolTypeSet enumTypeSymbols;
    enumTypeSymbols.insert(SymbolType::enumTypeSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupEnumType)] = std::move(enumTypeSymbols);
//  lookupEnumConstant:
    SymbolTypeSet enumConstantSymbols;
    enumConstantSymbols.insert(SymbolType::enumConstantSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupEnumConstant)] = std::move(enumConstantSymbols);
//  lookupConstantAndEnumConstantSymbols:
    SymbolTypeSet constantAndEnumConstantSymbols;
    constantAndEnumConstantSymbols.insert(SymbolType::constantSymbol);
    constantAndEnumConstantSymbols.insert(SymbolType::enumConstantSymbol);
    symbolTypeSets[int(SymbolTypeSetId::lookupConstantAndEnumConstantSymbols)] = std::move(constantAndEnumConstantSymbols);
}

SymbolTypeSet& SymbolTypeSetCollection::GetSymbolTypeSet(SymbolTypeSetId id)
{
    if (int(id) < 0 || int(id) >= int(symbolTypeSets.size()))
    {
        throw std::runtime_error("invalid symbol type set id");
    }
    return symbolTypeSets[int(id)];
}

SymbolTypeSetCollection* symbolTypeSetCollection = nullptr;

SymbolTypeSetCollection* GetSymbolTypeSetCollection()
{
    return symbolTypeSetCollection;
}

void SetSymbolTypeSetCollection(SymbolTypeSetCollection* collection)
{
    symbolTypeSetCollection = collection;
}

} } // namespace Cm::Sym
