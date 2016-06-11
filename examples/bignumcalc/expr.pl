namespace BigNumCalc
{
    grammar BigNumExprGrammar
    {
        Expr(EvaluationStack* s, var char op);
        Term(EvaluationStack* s, var char op);
        Factor(EvaluationStack* s, var char op);
        Number(EvaluationStack* s);
        BigFloatLiteral: Value*;
        ExponentPart;
        BigRationalLiteral: Value*;
        BigIntLiteral: Value*;
        Spaces;
    }
}
