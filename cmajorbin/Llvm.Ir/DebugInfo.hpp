/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_DEBUGINFO_INCLUDED
#define LLVM_IR_DEBUGINFO_INCLUDED
#include <Dwarf/Dwarf.hpp>

namespace Llvm { 

const int debugIdCmajor = 0x4444;
const int debugLangCmajor = Dwarf::DW_LANG_lo_user + debugIdCmajor; // 0x8000 + 0x4444 == 0xC444;

const int debugTagBegin = 0x000C0000;
const int debugTagCompileUnit = debugTagBegin + Dwarf::DW_TAG_compile_unit;
const int debugTagSubprogram = debugTagBegin + Dwarf::DW_TAG_subprogram;
const int debugTagFileType = debugTagBegin + Dwarf::DW_TAG_file_type;
const int debugTagSubroutineType = debugTagBegin + Dwarf::DW_TAG_subroutine_type;
const int debugTagBaseType = debugTagBegin + Dwarf::DW_TAG_base_type;
const int debugTagPointerType = debugTagBegin + Dwarf::DW_TAG_pointer_type;
const int debugTagReferenceType = debugTagBegin + Dwarf::DW_TAG_reference_type;
const int debugTagRvalueRefType = debugTagBegin + Dwarf::DW_TAG_rvalue_reference_type;
const int debugTagConstType = debugTagBegin + Dwarf::DW_TAG_const_type;
const int debugTagTypedef = debugTagBegin + Dwarf::DW_TAG_typedef;
const int debugTagLexicalBlock = debugTagBegin + Dwarf::DW_TAG_lexical_block;
const int debugTagClassType = debugTagBegin + Dwarf::DW_TAG_class_type;
const int debugTagEnumerationType = debugTagBegin + Dwarf::DW_TAG_enumeration_type;
const int debugTagEnumerator = debugTagBegin + Dwarf::DW_TAG_enumerator;
const int debugTagInheritance = debugTagBegin + Dwarf::DW_TAG_inheritance;
const int debugTagNamespace = debugTagBegin + Dwarf::DW_TAG_namespace;
const int debugTagFormalParameter = debugTagBegin + Dwarf::DW_TAG_formal_parameter;
const int debugTagVariable = debugTagBegin + Dwarf::DW_TAG_variable;
const int debugTagAutoVariable = debugTagBegin + Dwarf::DW_TAG_auto_variable;
const int debugTagArgVariable = debugTagBegin + Dwarf::DW_TAG_arg_variable;
const int debugTagMember = debugTagBegin + Dwarf::DW_TAG_member;
const int debugAttrEncodingBoolean = Dwarf::DW_ATE_boolean;
const int debugAttrEncodingUnsigned = Dwarf::DW_ATE_unsigned;
const int debugAttrEncodingSigned = Dwarf::DW_ATE_signed;
const int debugAttrEncodingSignedChar = Dwarf::DW_ATE_signed_char;
const int debugAttrEncodingUnsignedChar = Dwarf::DW_ATE_unsigned_char;
const int debugAttrEncodingFloat = Dwarf::DW_ATE_float;

} // namespace Llvm

#endif // LLVM_IR_DEBUGINFO_INCLUDED

