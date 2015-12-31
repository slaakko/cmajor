/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/Factory.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>

namespace Cm { namespace BoundTree {

Cm::Sym::BcuItem* Factory::CreateItem(Cm::Sym::BcuItemType itemType) const
{
    switch (itemType)
    {
        case Cm::Sym::BcuItemType::bcuClass: return new BoundClass();
        case Cm::Sym::BcuItemType::bcuFunction: return new BoundFunction();
        case Cm::Sym::BcuItemType::bcuStringLiteral: return new BoundStringLiteral();
        case Cm::Sym::BcuItemType::bcuLiteral: return new BoundLiteral();
        case Cm::Sym::BcuItemType::bcuConstant: return new BoundConstant();
        case Cm::Sym::BcuItemType::bcuExceptionTableConstant: return new BoundExceptionTableConstant();
        case Cm::Sym::BcuItemType::bcuEnumConstant: return new BoundEnumConstant();
        case Cm::Sym::BcuItemType::bcuLocalVariable: return new BoundLocalVariable();
        case Cm::Sym::BcuItemType::bcuExceptionCodeVariable: return new BoundExceptionCodeVariable();
        case Cm::Sym::BcuItemType::bcuParameter: return new BoundParameter();
        case Cm::Sym::BcuItemType::bcuReturnValue: return new BoundReturnValue();
        case Cm::Sym::BcuItemType::bcuExceptionCodeParameter: return new BoundExceptionCodeParameter();
        case Cm::Sym::BcuItemType::bcuMemberVariable: return new BoundMemberVariable();
        case Cm::Sym::BcuItemType::bcuFunctionId: return new BoundFunctionId();
        case Cm::Sym::BcuItemType::bcuTypeExpression: return new BoundTypeExpression();
        case Cm::Sym::BcuItemType::bcuNamespaceExpression: return new BoundNamespaceExpression();
        case Cm::Sym::BcuItemType::bcuConversion: return new BoundConversion();
        case Cm::Sym::BcuItemType::bcuCast: return new BoundCast();
        case Cm::Sym::BcuItemType::bcuIsExpr: return new BoundIsExpression();
        case Cm::Sym::BcuItemType::bcuAsExpr: return new BoundAsExpression();
        case Cm::Sym::BcuItemType::bcuSizeOfExpression: return new BoundSizeOfExpression();
        case Cm::Sym::BcuItemType::bcuDynamicTypeNameExpression: return new BoundDynamicTypeNameExpression();
        case Cm::Sym::BcuItemType::bcuUnaryOp: return new BoundUnaryOp();
        case Cm::Sym::BcuItemType::bcuBinaryOp: return new BoundBinaryOp();
        case Cm::Sym::BcuItemType::bcuPostfixIncDecExpr: return new BoundPostfixIncDecExpr();
        case Cm::Sym::BcuItemType::bcuFunctionGroup: return new BoundFunctionGroup();
        case Cm::Sym::BcuItemType::bcuFunctionCall: return new BoundFunctionCall();
        case Cm::Sym::BcuItemType::bcuDelegateCall: return new BoundDelegateCall();
        case Cm::Sym::BcuItemType::bcuClassDelegateCall: return new BoundClassDelegateCall();
        case Cm::Sym::BcuItemType::bcuDisjunction: return new BoundDisjunction();
        case Cm::Sym::BcuItemType::bcuConjunction: return new BoundConjunction();
        case Cm::Sym::BcuItemType::bcuCompoundStatement: return new BoundCompoundStatement();
        case Cm::Sym::BcuItemType::bcuReceiveStatement: return new BoundReceiveStatement();
        case Cm::Sym::BcuItemType::bcuInitClassObjectStatement: return new BoundInitClassObjectStatement();
        case Cm::Sym::BcuItemType::bcuInitVPtrStatement: return new BoundInitVPtrStatement();
        case Cm::Sym::BcuItemType::bcuInitMemberVariableStatement: return new BoundInitMemberVariableStatement();
        case Cm::Sym::BcuItemType::bcuFunctionCallStatement: return new BoundFunctionCallStatement();
        case Cm::Sym::BcuItemType::bcuReturnStatement: return new BoundReturnStatement();
        case Cm::Sym::BcuItemType::bcuBeginTryStatement: return new BoundBeginTryStatement();
        case Cm::Sym::BcuItemType::bcuEndTryStatement: return new BoundEndTryStatement();
        case Cm::Sym::BcuItemType::bcuExitBlockStatement: return new BoundExitBlocksStatement();
        case Cm::Sym::BcuItemType::bcuPushGenDebugInfoStatement: return new BoundPushGenDebugInfoStatement();
        case Cm::Sym::BcuItemType::bcuPopGenDebugInfoStatement: return new BoundPopGenDebugInfoStatement();
        case Cm::Sym::BcuItemType::bcuBeginThrowStatement: return new BoundBeginThrowStatement();
        case Cm::Sym::BcuItemType::bcuEndThrowStatement: return new BoundEndThrowStatement();
        case Cm::Sym::BcuItemType::bcuBeginCatchStatement: return new BoundBeginCatchStatement();
        case Cm::Sym::BcuItemType::bcuConstructionStatement: return new BoundConstructionStatement();
        case Cm::Sym::BcuItemType::bcuAssignmentStatement: return new BoundAssignmentStatement();
        case Cm::Sym::BcuItemType::bcuSimpleStatement: return new BoundSimpleStatement();
        case Cm::Sym::BcuItemType::bcuSwitchStatement: return new BoundSwitchStatement();
        case Cm::Sym::BcuItemType::bcuCaseStatement: return new BoundCaseStatement();
        case Cm::Sym::BcuItemType::bcuDefaultStatement: return new BoundDefaultStatement();
        case Cm::Sym::BcuItemType::bcuBreakStatement: return new BoundBreakStatement();
        case Cm::Sym::BcuItemType::bcuContinueStatement: return new BoundContinueStatement();
        case Cm::Sym::BcuItemType::bcuGotoStatement: return new BoundGotoStatement();
        case Cm::Sym::BcuItemType::bcuGotoCaseStatement: return new BoundGotoCaseStatement();
        case Cm::Sym::BcuItemType::bcuGotoDefaultStatement: return new BoundGotoDefaultStatement();
        case Cm::Sym::BcuItemType::bcuConditionalStatement: return new BoundConditionalStatement();
        case Cm::Sym::BcuItemType::bcuWhileStatement: return new BoundWhileStatement();
        case Cm::Sym::BcuItemType::bcuDoStatement: return new BoundDoStatement();
        case Cm::Sym::BcuItemType::bcuForStatement: return new BoundForStatement();
        case Cm::Sym::BcuItemType::bcuTraceCallInfo: return new TraceCallInfo();
    }
    return nullptr;
}

} } // namespace Cm::BoundTree
