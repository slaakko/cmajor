/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef DWARF_DWARF_INCLUDED
#define DWARF_DWARF_INCLUDED

namespace Dwarf {

// tag encodings:

const int DW_TAG_array_type = 0x01;
const int DW_TAG_class_type = 0x02;
const int DW_TAG_entry_point = 0x03;
const int DW_TAG_enumeration_type = 0x04;
const int DW_TAG_formal_parameter = 0x05;
const int DW_TAG_imported_declaration = 0x08;
const int DW_TAG_label = 0x0a;
const int DW_TAG_lexical_block = 0x0b;
const int DW_TAG_member = 0x0d;
const int DW_TAG_pointer_type = 0x0f;
const int DW_TAG_reference_type = 0x10;
const int DW_TAG_compile_unit = 0x11;
const int DW_TAG_string_type = 0x12;
const int DW_TAG_structure_type = 0x13;
const int DW_TAG_subroutine_type = 0x15;
const int DW_TAG_typedef = 0x16;
const int DW_TAG_union_type = 0x17;
const int DW_TAG_unspecified_parameters = 0x18;
const int DW_TAG_variant = 0x19;
const int DW_TAG_common_block = 0x1a;
const int DW_TAG_common_inclusion = 0x1b;
const int DW_TAG_inheritance = 0x1c;
const int DW_TAG_inlined_subroutine = 0x1d;
const int DW_TAG_module = 0x1e;
const int DW_TAG_ptr_to_member_type = 0x1f;
const int DW_TAG_set_type = 0x20;
const int DW_TAG_subrange_type = 0x21;
const int DW_TAG_with_stmt = 0x22;
const int DW_TAG_access_declaration = 0x23;
const int DW_TAG_base_type = 0x24;
const int DW_TAG_catch_block = 0x25;
const int DW_TAG_const_type = 0x26;
const int DW_TAG_constant = 0x27;
const int DW_TAG_enumerator = 0x28;
const int DW_TAG_file_type = 0x29;
const int DW_TAG_friend = 0x2a;
const int DW_TAG_namelist = 0x2b;
const int DW_TAG_namelist_item = 0x2c;
const int DW_TAG_packed_type = 0x2d;
const int DW_TAG_subprogram = 0x2e;
const int DW_TAG_template_type_parameter = 0x2f;
const int DW_TAG_template_value_parameter = 0x30;
const int DW_TAG_thrown_type = 0x31;
const int DW_TAG_try_block = 0x32;
const int DW_TAG_variant_part = 0x33;
const int DW_TAG_variable = 0x34;
const int DW_TAG_volatile_type = 0x35;
const int DW_TAG_dwarf_procedure = 0x36;
const int DW_TAG_restrict_type = 0x37;
const int DW_TAG_interface_type = 0x38;
const int DW_TAG_namespace = 0x39;
const int DW_TAG_imported_module = 0x3a;
const int DW_TAG_unspecified_type = 0x3b;
const int DW_TAG_partial_unit = 0x3c;
const int DW_TAG_imported_unit = 0x3d;
const int DW_TAG_condition = 0x3f;
const int DW_TAG_shared_type = 0x40;
const int DW_TAG_type_unit = 0x41;
const int DW_TAG_rvalue_reference_type = 0x42;
const int DW_TAG_template_alias = 0x43;
const int DW_TAG_lo_user = 0x4080;
const int DW_TAG_hi_user = 0xffff;

const int DW_TAG_auto_variable = 0x100;
const int DW_TAG_arg_variable = 0x101; 

// child determination encodings:

const int DW_CHILDREN_no = 0x00;
const int DW_CHIDREN_yes = 0x01;

// attribute encodings:

const int DW_AT_sibling = 0x01;
const int DW_AT_location = 0x02;
const int DW_AT_name = 0x03;
const int DW_AT_ordening = 0x09;
const int DW_AT_byte_size = 0x0b;
const int DW_AT_bit_offset = 0x0c;
const int DW_AT_bit_size = 0x0d;
const int DW_AT_stmt_list = 0x10;
const int DW_AT_low_pc = 0x11;
const int DW_AT_high_pc = 0x12;
const int DW_AT_language = 0x13;
const int DW_AT_discr = 0x15;
const int DW_AT_discr_value = 0x16;
const int DW_AT_visibility = 0x17;
const int DW_AT_import = 0x18;
const int DW_AT_string_length = 0x19;
const int DW_AT_common_reference = 0x1a;
const int DW_AT_comp_dir = 0x1b;
const int DW_AT_const_value = 0x1c;
const int DW_AT_containing_type = 0x1d;
const int DW_AT_default_value = 0x1e;
const int DW_AT_inline = 0x20;
const int DW_AT_is_optional = 0x21;
const int DW_AT_lower_bound = 0x22;
const int DW_AT_producer = 0x25;
const int DW_AT_prototyped = 0x27;
const int DW_AT_return_addr = 0x2a;
const int DW_AT_start_scope = 0x2c;
const int DW_AT_bit_stride = 0x2e;
const int DW_AT_upper_bound = 0x2f;
const int DW_AT_abstract_origin = 0x31;
const int DW_AT_accessibility = 0x32;
const int DW_AT_address_class = 0x33;
const int DW_AT_artificial = 0x34;
const int DW_AT_base_types = 0x35;
const int DW_AT_calling_convention = 0x36;
const int DW_AT_count = 0x37;
const int DW_AT_data_member_location = 0x38;
const int DW_AT_decl_column = 0x39;
const int DW_AT_decl_file = 0x3a;
const int DW_AT_decl_line = 0x3b;
const int DW_AT_declaration = 0x3c;
const int DW_AT_discr_list = 0x3d;
const int DW_AT_encoding = 0x3e;
const int DW_AT_external = 0x3f;
const int DW_AT_frame_base = 0x40;
const int DW_AT_friend = 0x41;
const int DW_AT_identifier_case = 0x42;
const int DW_AT_macro_info = 0x43;
const int DW_AT_namelist_item = 0x44;
const int DW_AT_priority = 0x45;
const int DW_AT_segment = 0x46;
const int DW_AT_specification = 0x47;
const int DW_AT_static_link = 0x48;
const int DW_AT_type = 0x49;
const int DW_AT_use_location = 0x4a;
const int DW_AT_variable_parameter = 0x4b;
const int DW_AT_virtuality = 0x4c;
const int DW_AT_vtable_elem_location = 0x4d;
const int DW_AT_allocated = 0x4e;
const int DW_AT_associated = 0x4f;
const int DW_AT_data_location = 0x50;
const int DW_AT_byte_stride = 0x51;
const int DW_AT_entry_pc = 0x52;
const int DW_AT_use_UTF8 = 0x53;
const int DW_AT_extension = 0x54;
const int DW_AT_ranges = 0x55;
const int DW_AT_trampoline = 0x56;
const int DW_AT_call_column = 0x57;
const int DW_AT_call_file = 0x58;
const int DW_AT_call_line = 0x59;
const int DW_AT_description = 0x5a;
const int DW_AT_binary_scale = 0x5b;
const int DW_AT_decimal_scale = 0x5c;
const int DW_AT_small = 0x5d;
const int DW_AT_decimal_sign = 0x5e;
const int DW_AT_digit_count = 0x5f;
const int DW_AT_picture_string = 0x60;
const int DW_AT_mutable = 0x61;
const int DW_AT_threads_scaled = 0x62;
const int DW_AT_explicit = 0x63;
const int DW_AT_object_pointer = 0x64;
const int DW_AT_endianity = 0x65;
const int DW_AT_elemental = 0x66;
const int DW_AT_pure = 0x67;
const int DW_AT_recursive = 0x68;
const int DW_AT_signature = 0x69;
const int DW_AT_main_subprogram = 0x6a;
const int DW_AT_data_bit_offset = 0x6b;
const int DW_AT_const_expr = 0x6c;
const int DW_AT_enum_class = 0x6d;
const int DW_AT_linkage_name = 0x6e;
const int DW_AT_low_user = 0x2000;
const int DW_AT_hi_user = 0x3fff;

//  attribute form encodings:

const int DW_FORM_addr = 0x01;
const int DW_FORM_block2 = 0x03;
const int DW_FORM_block4 = 0x04;
const int DW_FORM_data2 = 0x05;
const int DW_FORM_data4 = 0x06;
const int DW_FORM_data8 = 0x07;
const int DW_FORM_string = 0x08;
const int DW_FORM_block = 0x09;
const int DW_FORM_block1 = 0x0a;
const int DW_FORM_data1 = 0x0b;
const int DW_FORM_flag = 0x0c;
const int DW_FORM_sdata = 0x0d;
const int DW_FORM_strp = 0x0e;
const int DW_FORM_udata = 0x0f;
const int DW_FORM_ref_addr = 0x10;
const int DW_FORM_ref1 = 0x11;
const int DW_FORM_ref2 = 0x12;
const int DW_FORM_ref4 = 0x13;
const int DW_FORM_ref8 = 0x14;
const int DW_FORM_ref_udata = 0x15;
const int DW_FORM_indirect = 0x16;
const int DW_FORM_sec_offset = 0x17;
const int DW_FORM_exprloc = 0x18;
const int DW_FORM_flag_present = 0x19;
const int DW_FORM_ref_sig8 = 0x20;

// operation encodings:

typedef unsigned char byte;

const byte DW_OP_addr = 0x03;
const byte DW_OP_deref = 0x06;
const byte DW_OP_const1u = 0x08;
const byte DW_OP_const1s = 0x09;
const byte DW_OP_const2u = 0x0a;
const byte DW_OP_const2s = 0x0b;
const byte DW_OP_const4u = 0x0c;
const byte DW_OP_const4s = 0x0d;
const byte DW_OP_const8u = 0x0e;
const byte DW_OP_const8s = 0x0f;
const byte DW_OP_constu = 0x10;
const byte DW_OP_consts = 0x11;
const byte DW_OP_dup = 0x12;
const byte DW_OP_drop = 0x13;
const byte DW_OP_over = 0x14;
const byte DW_OP_pick = 0x15;
const byte DW_OP_swap = 0x16;
const byte DW_OP_rot = 0x17;
const byte DW_OP_xderef = 0x18;
const byte DW_OP_abs = 0x19;
const byte DW_OP_and = 0x1a;
const byte DW_OP_div = 0x1b;
const byte DW_OP_minus = 0x1c;
const byte DW_OP_mod = 0x1d;
const byte DW_OP_mul = 0x1e;
const byte DW_OP_neg = 0x1f;
const byte DW_OP_not = 0x20;
const byte DW_OP_or = 0x21;
const byte DW_OP_plus = 0x22;
const byte DW_OP_plus_uconst = 0x23;
const byte DW_OP_shl = 0x24;
const byte DW_OP_shr = 0x25;
const byte DW_OP_shra = 0x26;
const byte DW_OP_xor = 0x27;
const byte DW_OP_skip = 0x2f;
const byte DW_OP_bra = 0x28;
const byte DW_OP_eq = 0x29;
const byte DW_OP_ge = 0x2a;
const byte DW_OP_gt = 0x2b;
const byte DW_OP_le = 0x2c;
const byte DW_OP_lt = 0x2d;
const byte DW_OP_ne = 0x2e;
const byte DW_OP_lit0 = 0x30;
const byte DW_OP_lit1 = 0x31;
const byte DW_OP_lit2 = 0x32;
const byte DW_OP_lit3 = 0x33;
const byte DW_OP_lit4 = 0x34;
const byte DW_OP_lit5 = 0x35;
const byte DW_OP_lit6 = 0x36;
const byte DW_OP_lit7 = 0x37;
const byte DW_OP_lit8 = 0x38;
const byte DW_OP_lit9 = 0x39;
const byte DW_OP_lit10 = 0x3a;
const byte DW_OP_lit11 = 0x3b;
const byte DW_OP_lit12 = 0x3c;
const byte DW_OP_lit13 = 0x3d;
const byte DW_OP_lit14 = 0x3e;
const byte DW_OP_lit15 = 0x3f;
const byte DW_OP_lit16 = 0x40;
const byte DW_OP_lit17 = 0x41;
const byte DW_OP_lit18 = 0x42;
const byte DW_OP_lit19 = 0x43;
const byte DW_OP_lit20 = 0x44;
const byte DW_OP_lit21 = 0x45;
const byte DW_OP_lit22 = 0x46;
const byte DW_OP_lit23 = 0x47;
const byte DW_OP_lit24 = 0x48;
const byte DW_OP_lit25 = 0x49;
const byte DW_OP_lit26 = 0x4a;
const byte DW_OP_lit27 = 0x4b;
const byte DW_OP_lit28 = 0x4c;
const byte DW_OP_lit29 = 0x4d;
const byte DW_OP_lit30 = 0x4e;
const byte DW_OP_lit31 = 0x4f;
const byte DW_OP_reg0 = 0x50;
const byte DW_OP_reg1 = 0x51;
const byte DW_OP_reg2 = 0x52;
const byte DW_OP_reg3 = 0x53;
const byte DW_OP_reg4 = 0x54;
const byte DW_OP_reg5 = 0x55;
const byte DW_OP_reg6 = 0x56;
const byte DW_OP_reg7 = 0x57;
const byte DW_OP_reg8 = 0x58;
const byte DW_OP_reg9 = 0x59;
const byte DW_OP_reg10 = 0x5a;
const byte DW_OP_reg11 = 0x5b;
const byte DW_OP_reg12 = 0x5c;
const byte DW_OP_reg13 = 0x5d;
const byte DW_OP_reg14 = 0x5e;
const byte DW_OP_reg15 = 0x5f;
const byte DW_OP_reg16 = 0x60;
const byte DW_OP_reg17 = 0x61;
const byte DW_OP_reg18 = 0x62;
const byte DW_OP_reg19 = 0x63;
const byte DW_OP_reg20 = 0x64;
const byte DW_OP_reg21 = 0x65;
const byte DW_OP_reg22 = 0x66;
const byte DW_OP_reg23 = 0x67;
const byte DW_OP_reg24 = 0x68;
const byte DW_OP_reg25 = 0x69;
const byte DW_OP_reg26 = 0x6a;
const byte DW_OP_reg27 = 0x6b;
const byte DW_OP_reg28 = 0x6c;
const byte DW_OP_reg29 = 0x6d;
const byte DW_OP_reg30 = 0x6e;
const byte DW_OP_reg31 = 0x6f;
const byte DW_OP_breg0 = 0x70;
const byte DW_OP_breg1 = 0x71;
const byte DW_OP_breg2 = 0x72;
const byte DW_OP_breg3 = 0x73;
const byte DW_OP_breg4 = 0x74;
const byte DW_OP_breg5 = 0x75;
const byte DW_OP_breg6 = 0x76;
const byte DW_OP_breg7 = 0x77;
const byte DW_OP_breg8 = 0x78;
const byte DW_OP_breg9 = 0x79;
const byte DW_OP_breg10 = 0x7a;
const byte DW_OP_breg11 = 0x7b;
const byte DW_OP_breg12 = 0x7c;
const byte DW_OP_breg13 = 0x7d;
const byte DW_OP_breg14 = 0x7e;
const byte DW_OP_breg15 = 0x7f;
const byte DW_OP_breg16 = 0x80;
const byte DW_OP_breg17 = 0x81;
const byte DW_OP_breg18 = 0x82;
const byte DW_OP_breg19 = 0x83;
const byte DW_OP_breg20 = 0x84;
const byte DW_OP_breg21 = 0x85;
const byte DW_OP_breg22 = 0x86;
const byte DW_OP_breg23 = 0x87;
const byte DW_OP_breg24 = 0x88;
const byte DW_OP_breg25 = 0x89;
const byte DW_OP_breg26 = 0x8a;
const byte DW_OP_breg27 = 0x8b;
const byte DW_OP_breg28 = 0x8c;
const byte DW_OP_breg29 = 0x8d;
const byte DW_OP_breg30 = 0x8e;
const byte DW_OP_breg31 = 0x8f;
const byte DW_OP_regx = 0x90;
const byte DW_OP_fbreg = 0x91;
const byte DW_OP_bregx = 0x92;
const byte DW_OP_piece = 0x93;
const byte DW_OP_deref_size = 0x94;
const byte DW_OP_xdref_size = 0x95;
const byte DW_OP_nop = 0x96;
const byte DW_OP_push_object_address = 0x97;
const byte DW_OP_call2 = 0x98;
const byte DW_OP_call4 = 0x99;
const byte DW_OP_call_ref = 0x9a;
const byte DW_OP_form_tls_address = 0x9b;
const byte DW_OP_call_frame_cfa = 0x9c;
const byte DW_OP_bit_piece = 0x9d;
const byte DW_OP_implicit_value = 0x9e;
const byte DW_OP_stack_value = 0x9f;
const byte DW_OP_lo_user = 0xe0;
const byte DW_OP_hi_user = 0xff;

// base type encodings: (DW_AT_encoding values)

const int DW_ATE_address = 0x01;
const int DW_ATE_boolean = 0x02;
const int DW_ATE_complex_float = 0x03;
const int DW_ATE_float = 0x04;
const int DW_ATE_signed = 0x05;
const int DW_ATE_signed_char = 0x06;
const int DW_ATE_unsigned = 0x07;
const int DW_ATE_unsigned_char = 0x08;
const int DW_ATE_imaginary_float = 0x09;
const int DW_ATE_packed_decimal = 0x0a;
const int DW_ATE_numeric_string = 0x0b;
const int DW_ATE_edited = 0x0c;
const int DW_ATE_signed_fixed = 0x0d;
const int DW_ATE_unsigned_fixed = 0x0e;
const int DW_ATE_decimal_float = 0x0f;
const int DW_ATE_UTF = 0x10;
const int DW_ATE_lo_user = 0x80;
const int DW_ATE_hi_user = 0xff;

// decimal sign encodings: (DW_AT_decimal_sign values)

const int DW_DS_unsigned = 0x01;
const int DW_DS_leading_overpunch = 0x02;
const int DW_DS_trailing_overpunch = 0x03;
const int DW_DS_leading_separate = 0x04;
const int DW_DS_trailing_separate = 0x05;

// endianity encodings: (DW_AT_endianity values)

const int DW_END_default = 0x00;
const int DW_END_big = 0x01;
const int DW_END_little = 0x02;
const int DW_END_lo_user = 0x40;
const int DW_END_hi_user = 0xff;

// accessibility encodings: (DW_AT_accessibility values)

const int DW_ACCESS_public = 0x01;
const int DW_ACCESS_protected = 0x02;
const int DW_ACCESS_private = 0x03;

// visiblity encodings: (DW_AT_visibility values)

const int DW_VIS_local = 0x01;
const int DW_VIS_exported = 0x02;
const int DW_VIS_qualified = 0x03;

// virtuality encodings: (DW_AT_virtuality values)

const int DW_VIRTUALITY_none = 0x00;
const int DW_VIRTUALITY_virtual = 0x01;
const int DW_VIRTUALITY_pure_virtual = 0x02;

// language encodings: (DW_AT_language values)

const int DW_LANG_C89 = 0x0001;
const int DW_LANG_C = 0x0002;
const int DW_LANG_Ada83 = 0x0003;
const int DW_LANG_C_plus_plus = 0x0004;
const int DW_LANG_Cobol74 = 0x0005;
const int DW_LANG_Cobol85 = 0x0006;
const int DW_LANG_Fortran77 = 0x0007;
const int DW_LANG_Fortran90 = 0x0008;
const int DW_LANG_Pascal83 = 0x0009;
const int DW_LANG_Modula2 = 0x000a;
const int DW_LANG_Java = 0x000b;
const int DW_LANG_C99 = 0x000c;
const int DW_LANG_Ada95 = 0x000d;
const int DW_LANG_Fortran95 = 0x000e;
const int DW_LANG_PLI = 0x000f;
const int DW_LANG_ObjC = 0x0010;
const int DW_LANG_ObjC_plus_plus = 0x0011;
const int DW_LANG_UPC = 0x0012;
const int DW_LANG_D = 0x0013;
const int DW_LANG_Python = 0x0014;
const int DW_LANG_lo_user = 0x8000;
const int DW_LANG_hi_user = 0xffff;

// address class encodings:

const int DW_ADDR_none = 0;

// identifier case encodings: (DW_AT_identifier_case values)

const int DW_ID_case_sensitive = 0x00;
const int DW_ID_up_case = 0x01;
const int DW_ID_down_case = 0x02;
const int DW_ID_case_insensitive = 0x03;

// calling convention encodings: (DW_AT_calling_convention values)

const int DW_CC_normal = 0x01;
const int DW_CC_program = 0x02;
const int DW_CC_nocall = 0x03;
const int DW_CC_lo_user = 0x40;
const int DW_CC_hi_user = 0xff;

// inline encodings: (DW_AT_inline values)

const int DW_INL_not_inlined = 0x00;
const int DW_INL_inlined = 0x01;
const int DW_INL_declared_not_inlined = 0x02;
const int DW_INL_declared_inlined = 0x03;

// array ordering encodings: (DW_AT_ordering values)

const int DW_ORD_row_major = 0x00;
const int DW_ORD_col_major = 0x01;

// discriminant descriptor encodings: (DW_AT_discr_list values)

const int DW_DISC_label = 0x00;
const int DW_DISC_range = 0x01;

// line number standard opcode encodings:

const int DW_LNS_copy = 0x01;
const int DW_LNS_advance_pc = 0x02;
const int DW_LNS_advance_line = 0x03;
const int DW_LNS_set_file = 0x04;
const int DW_LNS_set_column = 0x05;
const int DW_LNS_negate_stmt = 0x06;
const int DW_LNS_set_basic_block = 0x07;
const int DW_LNS_const_add_pc = 0x08;
const int DW_LNS_fixed_advance_pc = 0x09;
const int DW_LNS_set_prologue_end = 0x0a;
const int DW_LNS_set_epilogue_begin = 0x0b;
const int DW_LNS_set_isa = 0x0c;

// line number extended opcode encodings:

const int DW_LNE_end_sequence = 0x01;
const int DW_LNE_set_address = 0x02;
const int DW_LNE_define_file = 0x03;
const int DW_LNE_set_discriminator = 0x04;
const int DW_LNE_lo_user = 0x80;
const int DW_LNE_hi_user = 0xff;

const byte DW_FLAG_true = 0x01;
const byte DW_FLAG_false = 0x00;

} // namespace Dwarf

#endif // DWARF_DWARF_INCLUDED
