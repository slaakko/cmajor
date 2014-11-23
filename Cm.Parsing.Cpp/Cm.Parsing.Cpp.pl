namespace Cm.Parsing.Cpp
{
    grammar DeclarationGrammar
    {
        BlockDeclaration: Cm::Parsing::CppObjectModel::CppObject*;
        SimpleDeclaration(var std::unique_ptr<SimpleDeclaration> sd): Cm::Parsing::CppObjectModel::SimpleDeclaration*;
        DeclSpecifierSeq(Cm::Parsing::CppObjectModel::SimpleDeclaration* declaration);
        DeclSpecifier: Cm::Parsing::CppObjectModel::DeclSpecifier*;
        StorageClassSpecifier: Cm::Parsing::CppObjectModel::StorageClassSpecifier*;
        TypeSpecifier: Cm::Parsing::CppObjectModel::TypeSpecifier*;
        SimpleTypeSpecifier: Cm::Parsing::CppObjectModel::TypeSpecifier*;
        TypeName: Cm::Parsing::CppObjectModel::TypeName*;
        TemplateArgumentList(Cm::Parsing::CppObjectModel::TypeName* typeName);
        TemplateArgument: Cm::Parsing::CppObjectModel::CppObject*;
        Typedef: Cm::Parsing::CppObjectModel::DeclSpecifier*;
        CVQualifier: Cm::Parsing::CppObjectModel::TypeSpecifier*;
        NamespaceAliasDefinition: Cm::Parsing::CppObjectModel::UsingObject*;
        UsingDeclaration: Cm::Parsing::CppObjectModel::UsingObject*;
        UsingDirective: Cm::Parsing::CppObjectModel::UsingObject*;
    }
    grammar LiteralGrammar
    {
        Literal: Cm::Parsing::CppObjectModel::Literal*;
        IntegerLiteral: Cm::Parsing::CppObjectModel::Literal*;
        DecimalLiteral;
        OctalLiteral;
        OctalDigit;
        HexadecimalLiteral;
        IntegerSuffix;
        UnsignedSuffix;
        LongLongSuffix;
        LongSuffix;
        CharacterLiteral: Cm::Parsing::CppObjectModel::Literal*;
        NarrowCharacterLiteral;
        UniversalCharacterLiteral;
        WideCharacterLiteral;
        CCharSequence: std::string;
        CChar;
        EscapeSequence;
        SimpleEscapeSequence;
        OctalEscapeSequence;
        HexadecimalEscapeSequence;
        FloatingLiteral: Cm::Parsing::CppObjectModel::Literal*;
        FractionalConstant;
        DigitSequence;
        ExponentPart;
        Sign;
        FloatingSuffix;
        StringLiteral: Cm::Parsing::CppObjectModel::Literal*;
        EncodingPrefix;
        SCharSequence;
        SChar;
        BooleanLiteral: Cm::Parsing::CppObjectModel::Literal*;
        PointerLiteral: Cm::Parsing::CppObjectModel::Literal*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar IdentifierGrammar
    {
        Identifier: std::string;
        QualifiedId: std::string;
    }
    grammar DeclaratorGrammar
    {
        InitDeclaratorList(var std::unique_ptr<InitDeclaratorList> idl): Cm::Parsing::CppObjectModel::InitDeclaratorList*;
        InitDeclarator: Cm::Parsing::CppObjectModel::InitDeclarator*;
        Declarator: std::string;
        DirectDeclarator(var std::unique_ptr<CppObject> o);
        DeclaratorId(var std::unique_ptr<CppObject> o);
        TypeId(var std::unique_ptr<TypeId> ti): Cm::Parsing::CppObjectModel::TypeId*;
        Type(var std::unique_ptr<Type> t): Cm::Parsing::CppObjectModel::Type*;
        TypeSpecifierSeq(Cm::Parsing::CppObjectModel::TypeId* typeId);
        AbstractDeclarator: std::string;
        DirectAbstractDeclarator(var std::unique_ptr<CppObject> o);
        PtrOperator;
        CVQualifierSeq(var std::unique_ptr<CppObject> o);
        Initializer: Cm::Parsing::CppObjectModel::Initializer*;
        InitializerClause: Cm::Parsing::CppObjectModel::AssignInit*;
        InitializerList(Cm::Parsing::CppObjectModel::AssignInit* init);
    }
    grammar ExpressionGrammar
    {
        Expression: Cm::Parsing::CppObjectModel::CppObject*;
        ConstantExpression: Cm::Parsing::CppObjectModel::CppObject*;
        AssignmentExpression(var std::unique_ptr<CppObject> lor): Cm::Parsing::CppObjectModel::CppObject*;
        AssingmentOp: Operator;
        ThrowExpression: Cm::Parsing::CppObjectModel::CppObject*;
        ConditionalExpression: Cm::Parsing::CppObjectModel::CppObject*;
        LogicalOrExpression: Cm::Parsing::CppObjectModel::CppObject*;
        LogicalAndExpression: Cm::Parsing::CppObjectModel::CppObject*;
        InclusiveOrExpression: Cm::Parsing::CppObjectModel::CppObject*;
        ExclusiveOrExpression: Cm::Parsing::CppObjectModel::CppObject*;
        AndExpression: Cm::Parsing::CppObjectModel::CppObject*;
        EqualityExpression: Cm::Parsing::CppObjectModel::CppObject*;
        EqOp: Operator;
        RelationalExpression: Cm::Parsing::CppObjectModel::CppObject*;
        RelOp: Operator;
        ShiftExpression: Cm::Parsing::CppObjectModel::CppObject*;
        ShiftOp: Operator;
        AdditiveExpression: Cm::Parsing::CppObjectModel::CppObject*;
        AddOp: Operator;
        MultiplicativeExpression: Cm::Parsing::CppObjectModel::CppObject*;
        MulOp: Operator;
        PmExpression: Cm::Parsing::CppObjectModel::CppObject*;
        PmOp: Operator;
        CastExpression(var std::unique_ptr<CppObject> ce, var std::unique_ptr<CppObject> ti): Cm::Parsing::CppObjectModel::CppObject*;
        UnaryExpression(var std::unique_ptr<CppObject> ue): Cm::Parsing::CppObjectModel::CppObject*;
        UnaryOperator: Operator;
        NewExpression(var bool global, var TypeId* typeId, var bool parens): Cm::Parsing::CppObjectModel::CppObject*;
        NewTypeId(var std::unique_ptr<TypeId> ti): Cm::Parsing::CppObjectModel::TypeId*;
        NewDeclarator: std::string;
        DirectNewDeclarator(var std::unique_ptr<CppObject> e);
        NewPlacement: std::vector<Cm::Parsing::CppObjectModel::CppObject*>;
        NewInitializer: std::vector<Cm::Parsing::CppObjectModel::CppObject*>;
        DeleteExpression(var bool global, var bool array): Cm::Parsing::CppObjectModel::CppObject*;
        PostfixExpression(var std::unique_ptr<CppObject> pe): Cm::Parsing::CppObjectModel::CppObject*;
        TypeSpecifierOrTypeName: Cm::Parsing::CppObjectModel::CppObject*;
        PostCastExpression: Cm::Parsing::CppObjectModel::CppObject*;
        ExpressionList: std::vector<Cm::Parsing::CppObjectModel::CppObject*>;
        PrimaryExpression(var std::unique_ptr<CppObject> pe): Cm::Parsing::CppObjectModel::CppObject*;
        IdExpression: Cm::Parsing::CppObjectModel::IdExpr*;
        OperatorFunctionId;
        Operator;
    }
    grammar StatementGrammar
    {
        Statement: Cm::Parsing::CppObjectModel::Statement*;
        LabeledStatement: Cm::Parsing::CppObjectModel::Statement*;
        Label: std::string;
        EmptyStatement: Cm::Parsing::CppObjectModel::Statement*;
        ExpressionStatement: Cm::Parsing::CppObjectModel::Statement*;
        CompoundStatement(var std::unique_ptr<CompoundStatement> cs): Cm::Parsing::CppObjectModel::CompoundStatement*;
        SelectionStatement: Cm::Parsing::CppObjectModel::Statement*;
        IfStatement: Cm::Parsing::CppObjectModel::Statement*;
        SwitchStatement: Cm::Parsing::CppObjectModel::Statement*;
        IterationStatement: Cm::Parsing::CppObjectModel::Statement*;
        WhileStatement: Cm::Parsing::CppObjectModel::Statement*;
        DoStatement: Cm::Parsing::CppObjectModel::Statement*;
        ForStatement: Cm::Parsing::CppObjectModel::Statement*;
        ForInitStatement: Cm::Parsing::CppObjectModel::CppObject*;
        JumpStatement: Cm::Parsing::CppObjectModel::Statement*;
        BreakStatement: Cm::Parsing::CppObjectModel::Statement*;
        ContinueStatement: Cm::Parsing::CppObjectModel::Statement*;
        ReturnStatement: Cm::Parsing::CppObjectModel::Statement*;
        GotoStatement: Cm::Parsing::CppObjectModel::Statement*;
        GotoTarget: std::string;
        DeclarationStatement: Cm::Parsing::CppObjectModel::Statement*;
        Condition(var std::unique_ptr<TypeId> ti): Cm::Parsing::CppObjectModel::CppObject*;
        TryStatement: Cm::Parsing::CppObjectModel::TryStatement*;
        HandlerSeq(TryStatement* st);
        Handler: Cm::Parsing::CppObjectModel::Handler*;
        ExceptionDeclaration(var std::unique_ptr<ExceptionDeclaration> ed): Cm::Parsing::CppObjectModel::ExceptionDeclaration*;
    }
}
