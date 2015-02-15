namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Cm::Ast::Node*;
    }
    grammar FunctionGrammar
    {
        Function(ParsingContext* ctx, var std::unique_ptr<FunctionNode> fun, var Span s): Cm::Ast::FunctionNode*;
        FunctionGroupId(ParsingContext* ctx): Cm::Ast::FunctionGroupIdNode*;
        OperatorFunctionGroupId(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): Cm::Ast::FunctionGroupIdNode*;
    }
    grammar ClassGrammar
    {
        Class(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit): Cm::Ast::ClassNode*;
        Inheritance(ParsingContext* ctx, Cm::Ast::ClassNode* cls);
        BaseClassTypeExpr(ParsingContext* ctx): Cm::Ast::Node*;
        ClassContent(ParsingContext* ctx, Cm::Ast::ClassNode* cls, Cm::Ast::CompileUnitNode* compileUnit);
        ClassMember(ParsingContext* ctx, Cm::Ast::ClassNode* cls, Cm::Ast::CompileUnitNode* compileUnit): Cm::Ast::Node*;
        InitializerList(ParsingContext* ctx, Node* owner);
        Initializer(ParsingContext* ctx): Cm::Ast::InitializerNode*;
        MemberInitializer(ParsingContext* ctx): Cm::Ast::MemberInitializerNode*;
        BaseInitializer(ParsingContext* ctx): Cm::Ast::BaseInitializerNode*;
        ThisInitializer(ParsingContext* ctx): Cm::Ast::ThisInitializerNode*;
        StaticConstructor(ParsingContext* ctx, Cm::Ast::ClassNode* cls, Cm::Ast::CompileUnitNode* compileUnit, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<IdentifierNode> refId): Cm::Ast::StaticConstructorNode*;
        Constructor(ParsingContext* ctx, Cm::Ast::ClassNode* cls, Cm::Ast::CompileUnitNode* compileUnit, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<IdentifierNode> refId, var std::unique_ptr<ConstructorNode> ctor): Cm::Ast::ConstructorNode*;
        Destructor(ParsingContext* ctx, Cm::Ast::ClassNode* cls, Cm::Ast::CompileUnitNode* compileUnit, var std::unique_ptr<IdentifierNode> id, var Span s): Cm::Ast::DestructorNode*;
        MemberFunction(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, var std::unique_ptr<MemberFunctionNode> memFun): Cm::Ast::MemberFunctionNode*;
        ConversionFunction(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, var bool setConst, var Span s): Cm::Ast::ConversionFunctionNode*;
        MemberVariable(ParsingContext* ctx): Cm::Ast::MemberVariableNode*;
    }
    grammar LiteralGrammar
    {
        Literal: Cm::Ast::Node*;
        BooleanLiteral: Cm::Ast::Node*;
        IntegerLiteral(var int start): Cm::Ast::Node*;
        FloatingLiteral(var int start): Cm::Ast::Node*;
        CharLiteral: Cm::Ast::Node*;
        StringLiteral(var std::string r): Cm::Ast::Node*;
        NullLiteral: Cm::Ast::Node*;
    }
    grammar EnumerationGrammar
    {
        EnumType(ParsingContext* ctx): Cm::Ast::EnumTypeNode*;
        UnderlyingType(ParsingContext* ctx): Cm::Ast::Node*;
        EnumConstants(ParsingContext* ctx, Cm::Ast::EnumTypeNode* enumType);
        EnumConstant(ParsingContext* ctx, Cm::Ast::EnumTypeNode* enumType, var Span s): Cm::Ast::Node*;
    }
    grammar DelegateGrammar
    {
        Delegate(ParsingContext* ctx): Cm::Ast::Node*;
        ClassDelegate(ParsingContext* ctx): Cm::Ast::Node*;
    }
    grammar CompileUnitGrammar
    {
        CompileUnit(ParsingContext* ctx): Cm::Ast::CompileUnitNode*;
        NamespaceContent(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, Cm::Ast::NamespaceNode* ns);
        UsingDirectives(ParsingContext* ctx, Cm::Ast::NamespaceNode* ns);
        UsingDirective(ParsingContext* ctx, Cm::Ast::NamespaceNode* ns);
        UsingAliasDirective(var std::unique_ptr<IdentifierNode> id): Cm::Ast::AliasNode*;
        UsingNamespaceDirective: Cm::Ast::NamespaceImportNode*;
        Definitions(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, Cm::Ast::NamespaceNode* ns);
        Definition(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, Cm::Ast::NamespaceNode* ns): Cm::Ast::Node*;
        NamespaceDefinition(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit, Cm::Ast::NamespaceNode* ns): Cm::Ast::NamespaceNode*;
        FunctionDefinition(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit): Cm::Ast::FunctionNode*;
        ConstantDefinition(ParsingContext* ctx): Cm::Ast::Node*;
        EnumerationDefinition(ParsingContext* ctx): Cm::Ast::Node*;
        TypedefDefinition(ParsingContext* ctx): Cm::Ast::Node*;
        ClassDefinition(ParsingContext* ctx, Cm::Ast::CompileUnitNode* compileUnit): Cm::Ast::Node*;
        DelegateDefinition(ParsingContext* ctx): Cm::Ast::Node*;
        ClassDelegateDefinition(ParsingContext* ctx): Cm::Ast::Node*;
        ConceptDefinition(ParsingContext* ctx): Cm::Ast::Node*;
    }
    grammar ConstantGrammar
    {
        Constant(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var std::unique_ptr<IdentifierNode> id): Cm::Ast::Node*;
    }
    grammar ConceptGrammar
    {
        Concept(ParsingContext* ctx): Cm::Ast::ConceptNode*;
        Refinement: Cm::Ast::ConceptIdNode*;
        ConceptBody(ParsingContext* ctx, Cm::Ast::ConceptNode* concept);
        ConceptBodyConstraint(ParsingContext* ctx, Cm::Ast::ConceptNode* concept);
        Axiom(ParsingContext* ctx, Cm::Ast::ConceptNode* concept, var std::unique_ptr<AxiomNode> axiom);
        AxiomBody(ParsingContext* ctx, Cm::Ast::AxiomNode* axiom);
        AxiomStatement(ParsingContext* ctx): Cm::Ast::AxiomStatementNode*;
        TypenameConstraint(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        SignatureConstraint(ParsingContext* ctx, std::string firstTypeParameter): Cm::Ast::ConstraintNode*;
        ConstructorConstraint(ParsingContext* ctx, std::string firstTypeParameter, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<ConstraintNode> ctorConstraint): Cm::Ast::ConstraintNode*;
        DestructorConstraint(ParsingContext* ctx, std::string firstTypeParameter, var std::unique_ptr<IdentifierNode> id): Cm::Ast::ConstraintNode*;
        MemberFunctionConstraint(ParsingContext* ctx, var std::unique_ptr<Node> returnType, var std::unique_ptr<IdentifierNode> typeParam): Cm::Ast::ConstraintNode*;
        FunctionConstraint(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        EmbeddedConstraint(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        WhereConstraint(ParsingContext* ctx): Cm::Ast::WhereConstraintNode*;
        ConstraintExpr(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        DisjunctiveConstraintExpr(ParsingContext* ctx, var Span s): Cm::Ast::ConstraintNode*;
        ConjunctiveConstraintExpr(ParsingContext* ctx, var Span s): Cm::Ast::ConstraintNode*;
        PrimaryConstraintExpr(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        AtomicConstraintExpr(ParsingContext* ctx): Cm::Ast::ConstraintNode*;
        IsConstraint(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): Cm::Ast::ConstraintNode*;
        ConceptOrTypeName(ParsingContext* ctx): Cm::Ast::Node*;
        MultiParamConstraint(ParsingContext* ctx, var std::unique_ptr<MultiParamConstraintNode> constraint): Cm::Ast::ConstraintNode*;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx): Cm::Ast::Node*;
        Equivalence(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Implication(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Disjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Conjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitOr(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitXor(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitAnd(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Equality(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Relational(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Shift(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Additive(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Multiplicative(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Prefix(ParsingContext* ctx, var Span s, var Operator op): Cm::Ast::Node*;
        Postfix(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Primary(ParsingContext* ctx): Cm::Ast::Node*;
        SizeOfExpr(ParsingContext* ctx): Cm::Ast::Node*;
        CastExpr(ParsingContext* ctx): Cm::Ast::Node*;
        ConstructExpr(ParsingContext* ctx): Cm::Ast::Node*;
        NewExpr(ParsingContext* ctx): Cm::Ast::Node*;
        ArgumentList(ParsingContext* ctx, Cm::Ast::Node* node);
        ExpressionList(ParsingContext* ctx, Cm::Ast::Node* node);
    }
    grammar IdentifierGrammar
    {
        Identifier: Cm::Ast::IdentifierNode*;
        QualifiedId: Cm::Ast::IdentifierNode*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar SolutionGrammar
    {
        Solution: Cm::Ast::Solution*;
        Declarations(Cm::Ast::Solution* solution);
        Declaration(Cm::Ast::Solution* solution): Cm::Ast::SolutionDeclaration*;
        ProjectFileDeclaration(Cm::Ast::Solution* solution): Cm::Ast::SolutionDeclaration*;
        ActiveProjectDeclaration: Cm::Ast::SolutionDeclaration*;
        ProjectDependencyDeclaration: Cm::Ast::ProjectDependencyDeclaration*;
        FilePath: std::string;
    }
    grammar ParameterGrammar
    {
        ParameterList(ParsingContext* ctx, Cm::Ast::Node* owner);
        Parameter(ParsingContext* ctx): Cm::Ast::ParameterNode*;
    }
    grammar ProjectGrammar
    {
        Project(std::string config, std::string backend, std::string os): Cm::Ast::Project*;
        Declarations(Cm::Ast::Project* project);
        Declaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        SourceFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        AsmSourceFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        CSourceFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        TextFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        ReferenceFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        AssemblyFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        ExecutableFileDeclaration(Cm::Ast::Project* project): Cm::Ast::ProjectDeclaration*;
        CLibraryDeclaration: Cm::Ast::ProjectDeclaration*;
        TargetDeclaration: Cm::Ast::ProjectDeclaration*;
        Properties: Cm::Ast::Properties;
        FilePath: std::string;
    }
    grammar SpecifierGrammar
    {
        Specifiers: Cm::Ast::Specifiers;
        Specifier: Cm::Ast::Specifiers;
    }
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        LabelId: std::string;
        Label(var std::string label): Cm::Ast::LabelNode*;
        TargetLabel(var std::string label): Cm::Ast::LabelNode*;
        LabeledStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        SimpleStatement(ParsingContext* ctx, var std::unique_ptr<Node> expr): Cm::Ast::StatementNode*;
        ControlStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ReturnStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ConditionalStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        SwitchStatement(ParsingContext* ctx): Cm::Ast::SwitchStatementNode*;
        CaseStatement(ParsingContext* ctx, var std::unique_ptr<CaseStatementNode> caseS): Cm::Ast::CaseStatementNode*;
        DefaultStatement(ParsingContext* ctx): Cm::Ast::DefaultStatementNode*;
        CaseList(ParsingContext* ctx, Cm::Ast::CaseStatementNode* caseS);
        GotoCaseStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        GotoDefaultStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        WhileStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DoStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        RangeForStatement(ParsingContext* ctx, var std::unique_ptr<Node> varType, var std::unique_ptr<IdentifierNode> varId): Cm::Ast::StatementNode*;
        ForStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ForInitStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        CompoundStatement(ParsingContext* ctx): Cm::Ast::CompoundStatementNode*;
        BreakStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ContinueStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        GotoStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        TypedefStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        AssignmentStatement(ParsingContext* ctx, var std::unique_ptr<Node> targetExpr): Cm::Ast::StatementNode*;
        ConstructionStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DeleteStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DestroyStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ThrowStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        TryStatement(ParsingContext* ctx): Cm::Ast::TryStatementNode*;
        Handlers(ParsingContext* ctx, Cm::Ast::TryStatementNode* tryS);
        Handler(ParsingContext* ctx): Cm::Ast::CatchNode*;
        AssertStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ConditionalCompilationStatement(ParsingContext* ctx): Cm::Ast::CondCompStatementNode*;
        ConditionalCompilationExpr: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationDisjunction(var Span s): Cm::Ast::CondCompExprNode*;
        ConditionalCompilationConjunction(var Span s): Cm::Ast::CondCompExprNode*;
        ConditionalCompilationPrefix: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationPrimary: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationSymbol: Cm::Ast::CondCompSymbolNode*;
        Symbol: std::string;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var std::unique_ptr<TemplateIdNode> templateId): Cm::Ast::Node*;
        TemplateParameter(ParsingContext* ctx): Cm::Ast::TemplateParameterNode*;
        TemplateParameterList(ParsingContext* ctx, Node* owner);
    }
    grammar ToolErrorGrammar
    {
        ToolError: Cm::Util::ToolError;
        ToolName: std::string;
        FilePath: std::string;
        Drive;
        Line: int;
        Column: int;
        Message: std::string;
    }
    grammar TypedefGrammar
    {
        Typedef(ParsingContext* ctx): Cm::Ast::Node*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var std::unique_ptr<DerivedTypeExprNode> node): Cm::Ast::Node*;
        PrefixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node, var Span s);
        PrimaryTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
    }
}
