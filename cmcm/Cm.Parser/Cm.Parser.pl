namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Node*;
    }
    grammar ClassGrammar
    {
        Class(ParsingContext* ctx, CompileUnitNode* compileUnit): ClassNode*;
        Inheritance(ParsingContext* ctx, ClassNode* cls);
        BaseClassTypeExpr(ParsingContext* ctx): Node*;
        ClassContent(ParsingContext* ctx, ClassNode* cls, CompileUnitNode* compileUnit);
        ClassMember(ParsingContext* ctx, ClassNode* cls, CompileUnitNode* compileUnit): Node*;
        InitializerList(ParsingContext* ctx, Node* owner);
        Initializer(ParsingContext* ctx): InitializerNode*;
        BaseInitializer(ParsingContext* ctx): BaseInitializerNode*;
        ThisInitializer(ParsingContext* ctx): ThisInitializerNode*;
        MemberInitializer(ParsingContext* ctx): MemberInitializerNode*;
        StaticConstructor(ParsingContext* ctx, ClassNode* cls, CompileUnitNode* compileUnit, var UniquePtr<IdentifierNode> id, var UniquePtr<IdentifierNode> refId): StaticConstructorNode*;
        Constructor(ParsingContext* ctx, ClassNode* cls, CompileUnitNode* compileUnit, var UniquePtr<IdentifierNode> id, var UniquePtr<IdentifierNode> refId, var UniquePtr<ConstructorNode> ctor): ConstructorNode*;
        Destructor(ParsingContext* ctx, ClassNode* cls, CompileUnitNode* compileUnit, var UniquePtr<IdentifierNode> id, var Span s): DestructorNode*;
        MemberFunction(ParsingContext* ctx, CompileUnitNode* compileUnit, var UniquePtr<MemberFunctionNode> memFun): MemberFunctionNode*;
        ConversionFunction(ParsingContext* ctx, CompileUnitNode* compileUnit, var bool setConst, var Span s): ConversionFunctionNode*;
        MemberVariable(ParsingContext* ctx): MemberVariableNode*;
    }
    grammar CompileUnitGrammar
    {
        CompileUnit(ParsingContext* ctx): CompileUnitNode*;
        NamespaceContent(ParsingContext* ctx, CompileUnitNode* compileUnit, NamespaceNode* ns);
        UsingDirectives(ParsingContext* ctx, NamespaceNode* ns);
        UsingDirective(ParsingContext* ctx, NamespaceNode* ns);
        UsingAliasDirective(var UniquePtr<IdentifierNode> id): AliasNode*;
        UsingNamespaceDirective: NamespaceImportNode*;
        Definitions(ParsingContext* ctx, CompileUnitNode* compileUnit, NamespaceNode* ns);
        Definition(ParsingContext* ctx, CompileUnitNode* compileUnit, NamespaceNode* ns): Node*;
        NamespaceDefinition(ParsingContext* ctx, CompileUnitNode* compileUnit, NamespaceNode* ns): NamespaceNode*;
        FunctionDefinition(ParsingContext* ctx, CompileUnitNode* compileUnit): FunctionNode*;
        ConstantDefinition(ParsingContext* ctx): Node*;
        EnumerationDefinition(ParsingContext* ctx): Node*;
        TypedefDefinition(ParsingContext* ctx): Node*;
        ClassDefinition(ParsingContext* ctx, CompileUnitNode* compileUnit): Node*;
        DelegateDefinition(ParsingContext* ctx): Node*;
        ClassDelegateDefinition(ParsingContext* ctx): Node*;
        ConceptDefinition(ParsingContext* ctx): Node*;
    }
    grammar ConceptGrammar
    {
        Concept(ParsingContext* ctx): ConceptNode*;
        Refinement: ConceptIdNode*;
        ConceptBody(ParsingContext* ctx, ConceptNode* con);
        ConceptBodyConstraint(ParsingContext* ctx, ConceptNode* con);
        Axiom(ParsingContext* ctx, ConceptNode* con, var UniquePtr<AxiomNode> ax);
        AxiomBody(ParsingContext* ctx, AxiomNode* ax);
        AxiomStatement(ParsingContext* ctx): AxiomStatementNode*;
        TypenameConstraint(ParsingContext* ctx): ConstraintNode*;
        SignatureConstraint(ParsingContext* ctx, string firstTypeParameter): ConstraintNode*;
        ConstructorConstraint(ParsingContext* ctx, string firstTypeParameter, var UniquePtr<IdentifierNode> id, var UniquePtr<ConstraintNode> ctorConstraint): ConstraintNode*;
        DestructorConstraint(ParsingContext* ctx, string firstTypeParameter, var UniquePtr<IdentifierNode> id): ConstraintNode*;
        MemberFunctionConstraint(ParsingContext* ctx, var UniquePtr<Node> returnType, var UniquePtr<IdentifierNode> typeParam): ConstraintNode*;
        FunctionConstraint(ParsingContext* ctx): ConstraintNode*;
        EmbeddedConstraint(ParsingContext* ctx): ConstraintNode*;
        WhereConstraint(ParsingContext* ctx): WhereConstraintNode*;
        ConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        DisjunctiveConstraintExpr(ParsingContext* ctx, var Span s): ConstraintNode*;
        ConjunctiveConstraintExpr(ParsingContext* ctx, var Span s): ConstraintNode*;
        PrimaryConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        AtomicConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        IsConstraint(ParsingContext* ctx, var UniquePtr<Node> typeExpr): ConstraintNode*;
        ConceptOrTypeName(ParsingContext* ctx): Node*;
        MultiParamConstraint(ParsingContext* ctx, var UniquePtr<MultiParamConstraintNode> constraint): ConstraintNode*;
    }
    grammar ConstantGrammar
    {
        Constant(ParsingContext* ctx, var UniquePtr<Node> typeExpr, var UniquePtr<IdentifierNode> id): Node*;
    }
    grammar DelegateGrammar
    {
        Delegate(ParsingContext* ctx): Node*;
        ClassDelegate(ParsingContext* ctx): Node*;
    }
    grammar EnumerationGrammar
    {
        EnumType(ParsingContext* ctx): EnumTypeNode*;
        UnderlyingType(ParsingContext* ctx): Node*;
        EnumConstants(ParsingContext* ctx, EnumTypeNode* enumType);
        EnumConstant(ParsingContext* ctx, EnumTypeNode* enumType, var Span s): EnumConstantNode*;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx): Node*;
        Equivalence(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Implication(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Disjunction(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Conjunction(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitOr(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitXor(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitAnd(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Equality(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Relational(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op, var UniquePtr<Node> typeExpr): Node*;
        Shift(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Additive(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Multiplicative(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Prefix(ParsingContext* ctx, var Span s, var Operator op): Node*;
        Postfix(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Primary(ParsingContext* ctx): Node*;
        SizeOfExpr(ParsingContext* ctx): Node*;
        CastExpr(ParsingContext* ctx): Node*;
        ConstructExpr(ParsingContext* ctx): Node*;
        NewExpr(ParsingContext* ctx): Node*;
        ArgumentList(ParsingContext* ctx, Node* node);
        ExpressionList(ParsingContext* ctx, Node* node);
    }
    grammar FunctionGrammar
    {
        Function(ParsingContext* ctx, var UniquePtr<FunctionNode> fun, var Span s): FunctionNode*;
        FunctionGroupId(ParsingContext* ctx): FunctionGroupIdNode*;
        OperatorFunctionGroupId(ParsingContext* ctx, var UniquePtr<Node> typeExpr): FunctionGroupIdNode*;
    }
    grammar IdentifierGrammar
    {
        Identifier: IdentifierNode*;
        QualifiedId: IdentifierNode*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar LiteralGrammar
    {
        Literal: Node*;
        BooleanLiteral: Node*;
        IntegerLiteral(var int start): Node*;
        FloatingLiteral(var int start): Node*;
        CharLiteral: Node*;
        StringLiteral(var string r): Node*;
        NullLiteral: Node*;
    }
    grammar LlvmVersionGrammar
    {
        LlvmVersion(var string versionText): ProgramVersion;
    }
    grammar ParameterGrammar
    {
        ParameterList(ParsingContext* ctx, Node* owner);
        Parameter(ParsingContext* ctx): ParameterNode*;
    }
    grammar ProjectGrammar
    {
        Project(string config, string backend, string os, int bits, ProgramVersion llvmVersion): Project*;
        Declarations(Project* project);
        Declaration(Project* project): ProjectDeclaration*;
        SourceFileDeclaration(Project* project): ProjectDeclaration*;
        AsmSourceFileDeclaration(Project* project): ProjectDeclaration*;
        CSourceFileDeclaration(Project* project): ProjectDeclaration*;
        CppSourceFileDeclaration(Project* project): ProjectDeclaration*;
        TextFileDeclaration(Project* project): ProjectDeclaration*;
        ReferenceFileDeclaration(Project* project): ProjectDeclaration*;
        CLibraryDeclaration(Project* project): ProjectDeclaration*;
        TargetDeclaration: ProjectDeclaration*;
        StackSizeDeclaration: ProjectDeclaration*;
        AssemblyFileDeclaration(Project* project): ProjectDeclaration*;
        ExecutableFileDeclaration(Project* project): ProjectDeclaration*;
        AddLibraryPathDeclaration(Project* project): ProjectDeclaration*;
        Properties: Properties;
        RelOp: RelOp;
        PropertyValue: string;
        FilePath: string;
    }
    grammar SolutionGrammar
    {
        Solution: Solution*;
        Declarations(Solution* solution);
        Declaration(Solution* solution): SolutionDeclaration*;
        ProjectFileDeclaration(Solution* solution): SolutionDeclaration*;
        ActiveProjectDeclaration: SolutionDeclaration*;
        ProjectDependencyDeclaration: ProjectDependencyDeclaration*;
        FilePath: string;
    }
    grammar SpecifierGrammar
    {
        Specifiers: Specifiers;
        Specifier: Specifiers;
    }
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx): StatementNode*;
        LabelId: string;
        Label: LabelNode*;
        TargetLabel: LabelNode*;
        LabeledStatement(ParsingContext* ctx): StatementNode*;
        SimpleStatement(ParsingContext* ctx, var UniquePtr<Node> expr): StatementNode*;
        ControlStatement(ParsingContext* ctx): StatementNode*;
        ReturnStatement(ParsingContext* ctx): StatementNode*;
        ConditionalStatement(ParsingContext* ctx): StatementNode*;
        SwitchStatement(ParsingContext* ctx): SwitchStatementNode*;
        CaseStatement(ParsingContext* ctx, var UniquePtr<CaseStatementNode> caseS): CaseStatementNode*;
        DefaultStatement(ParsingContext* ctx): DefaultStatementNode*;
        CaseList(ParsingContext* ctx, CaseStatementNode* caseS);
        GotoCaseStatement(ParsingContext* ctx): StatementNode*;
        GotoDefaultStatement: StatementNode*;
        WhileStatement(ParsingContext* ctx): StatementNode*;
        DoStatement(ParsingContext* ctx): StatementNode*;
        RangeForStatement(ParsingContext* ctx, var UniquePtr<Node> varType, var UniquePtr<IdentifierNode> varId): StatementNode*;
        ForStatement(ParsingContext* ctx): StatementNode*;
        ForInitStatement(ParsingContext* ctx): StatementNode*;
        CompoundStatement(ParsingContext* ctx): CompoundStatementNode*;
        BreakStatement: StatementNode*;
        ContinueStatement: StatementNode*;
        GotoStatement: StatementNode*;
        TypedefStatement(ParsingContext* ctx): StatementNode*;
        AssignmentStatement(ParsingContext* ctx, var UniquePtr<Node> targetExpr): StatementNode*;
        ConstructionStatement(ParsingContext* ctx): StatementNode*;
        DeleteStatement(ParsingContext* ctx): StatementNode*;
        DestroyStatement(ParsingContext* ctx): StatementNode*;
        ThrowStatement(ParsingContext* ctx): StatementNode*;
        TryStatement(ParsingContext* ctx): TryStatementNode*;
        Handlers(ParsingContext* ctx, TryStatementNode* tryS);
        Handler(ParsingContext* ctx): CatchNode*;
        AssertStatement(ParsingContext* ctx): StatementNode*;
        ConditionalCompilationStatement(ParsingContext* ctx): CondCompStatementNode*;
        ConditionalCompilationExpr: CondCompExprNode*;
        ConditionalCompilationDisjunction(var Span s): CondCompExprNode*;
        ConditionalCompilationConjunction(var Span s): CondCompExprNode*;
        ConditionalCompilationPrefix: CondCompExprNode*;
        ConditionalCompilationPrimary: CondCompExprNode*;
        ConditionalCompilationSymbol: CondCompSymbolNode*;
        Symbol: string;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var UniquePtr<TemplateIdNode> templateId): Node*;
        TemplateParameter(ParsingContext* ctx): TemplateParameterNode*;
        TemplateParameterList(ParsingContext* ctx, Node* owner);
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var UniquePtr<DerivedTypeExprNode> node): Node*;
        PrefixTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node, var Span s);
        PrimaryTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node);
    }
    grammar TypedefGrammar
    {
        Typedef(ParsingContext* ctx): Node*;
    }
    grammar VersionNumberGrammar
    {
        VersionNumber: ProgramVersion;
        Major: int;
        Minor: int;
        Revision: int;
        Build: int;
    }
}
