/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_PY_EMIT_H
#define MICROPY_INCLUDED_PY_EMIT_H

#include "py/lexer.h"
#include "py/scope.h"

/* Notes on passes:
 * We don't know exactly the opcodes in pass 1 because they depend on the
 * closing over of variables (LOAD_CLOSURE, BUILD_TUPLE, MAKE_CLOSURE), which
 * depends on determining the scope of variables in each function, and this
 * is not known until the end of pass 1.
 * As a consequence, we don't know the maximum stack size until the end of pass 2.
 * This is problematic for some emitters (x64) since they need to know the maximum
 * stack size to compile the entry to the function, and this affects code size.
 */

typedef enum {
    MP_PASS_SCOPE = 1,      // work out id's and their kind, and number of labels
    MP_PASS_STACK_SIZE = 2, // work out maximum stack size
    MP_PASS_CODE_SIZE = 3,  // work out code size and label offsets
    MP_PASS_EMIT = 4,       // emit code
} pass_kind_t;

#define MP_EMIT_STAR_FLAG_SINGLE (0x01)
#define MP_EMIT_STAR_FLAG_DOUBLE (0x02)

#define MP_EMIT_BREAK_FROM_FOR (0x8000)

#define MP_EMIT_NATIVE_TYPE_ENABLE (0)
#define MP_EMIT_NATIVE_TYPE_RETURN (1)
#define MP_EMIT_NATIVE_TYPE_ARG    (2)

typedef struct _emit_t emit_t;

typedef struct _mp_emit_method_table_id_ops_t {
    void (*fast)(emit_t *emit, qstr qst, mp_uint_t local_num);
    void (*deref)(emit_t *emit, qstr qst, mp_uint_t local_num);
    void (*name)(emit_t *emit, qstr qst);
    void (*global)(emit_t *emit, qstr qst);
} mp_emit_method_table_id_ops_t;

typedef struct _emit_method_table_t {
    void (*set_native_type)(emit_t *emit, mp_uint_t op, mp_uint_t arg1, qstr arg2);
    void (*start_pass)(emit_t *emit, pass_kind_t pass, scope_t *scope);
    void (*end_pass)(emit_t *emit);
    bool (*last_emit_was_return_value)(emit_t *emit);
    void (*adjust_stack_size)(emit_t *emit, mp_int_t delta);
    void (*set_source_line)(emit_t *emit, mp_uint_t line);

    mp_emit_method_table_id_ops_t load_id;
    mp_emit_method_table_id_ops_t store_id;
    mp_emit_method_table_id_ops_t delete_id;

    void (*label_assign)(emit_t *emit, mp_uint_t l);
    void (*import_name)(emit_t *emit, qstr qst);
    void (*import_from)(emit_t *emit, qstr qst);
    void (*import_star)(emit_t *emit);
    void (*load_const_tok)(emit_t *emit, mp_token_kind_t tok);
    void (*load_const_small_int)(emit_t *emit, mp_int_t arg);
    void (*load_const_str)(emit_t *emit, qstr qst);
    void (*load_const_obj)(emit_t *emit, mp_obj_t obj);
    void (*load_null)(emit_t *emit);
    void (*load_attr)(emit_t *emit, qstr qst);
    void (*load_method)(emit_t *emit, qstr qst, bool is_super);
    void (*load_build_class)(emit_t *emit);
    void (*load_subscr)(emit_t *emit);
    void (*store_attr)(emit_t *emit, qstr qst);
    void (*store_subscr)(emit_t *emit);
    void (*delete_attr)(emit_t *emit, qstr qst);
    void (*delete_subscr)(emit_t *emit);
    void (*dup_top)(emit_t *emit);
    void (*dup_top_two)(emit_t *emit);
    void (*pop_top)(emit_t *emit);
    void (*rot_two)(emit_t *emit);
    void (*rot_three)(emit_t *emit);
    void (*jump)(emit_t *emit, mp_uint_t label);
    void (*pop_jump_if)(emit_t *emit, bool cond, mp_uint_t label);
    void (*jump_if_or_pop)(emit_t *emit, bool cond, mp_uint_t label);
    void (*break_loop)(emit_t *emit, mp_uint_t label, mp_uint_t except_depth);
    void (*continue_loop)(emit_t *emit, mp_uint_t label, mp_uint_t except_depth);
    void (*setup_with)(emit_t *emit, mp_uint_t label);
    void (*with_cleanup)(emit_t *emit, mp_uint_t label);
    void (*setup_except)(emit_t *emit, mp_uint_t label);
    void (*setup_finally)(emit_t *emit, mp_uint_t label);
    void (*end_finally)(emit_t *emit);
    void (*get_iter)(emit_t *emit, bool use_stack);
    void (*for_iter)(emit_t *emit, mp_uint_t label);
    void (*for_iter_end)(emit_t *emit);
    void (*pop_block)(emit_t *emit);
    void (*pop_except)(emit_t *emit);
    void (*unary_op)(emit_t *emit, mp_unary_op_t op);
    void (*binary_op)(emit_t *emit, mp_binary_op_t op);
    void (*build_tuple)(emit_t *emit, mp_uint_t n_args);
    void (*build_list)(emit_t *emit, mp_uint_t n_args);
    void (*build_map)(emit_t *emit, mp_uint_t n_args);
    void (*store_map)(emit_t *emit);
    #if MICROPY_PY_BUILTINS_SET
    void (*build_set)(emit_t *emit, mp_uint_t n_args);
    #endif
    #if MICROPY_PY_BUILTINS_SLICE
    void (*build_slice)(emit_t *emit, mp_uint_t n_args);
    #endif
    void (*store_comp)(emit_t *emit, scope_kind_t kind, mp_uint_t set_stack_index);
    void (*unpack_sequence)(emit_t *emit, mp_uint_t n_args);
    void (*unpack_ex)(emit_t *emit, mp_uint_t n_left, mp_uint_t n_right);
    void (*make_function)(emit_t *emit, scope_t *scope, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults);
    void (*make_closure)(emit_t *emit, scope_t *scope, mp_uint_t n_closed_over, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults);
    void (*call_function)(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags);
    void (*call_method)(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags);
    void (*return_value)(emit_t *emit);
    void (*raise_varargs)(emit_t *emit, mp_uint_t n_args);
    void (*yield_value)(emit_t *emit);
    void (*yield_from)(emit_t *emit);

    // these methods are used to control entry to/exit from an exception handler
    // they may or may not emit code
    void (*start_except_handler)(emit_t *emit);
    void (*end_except_handler)(emit_t *emit);
} emit_method_table_t;

void mp_emit_common_get_id_for_load(scope_t *scope, qstr qst);
void mp_emit_common_get_id_for_modification(scope_t *scope, qstr qst);
void mp_emit_common_id_op(emit_t *emit, const mp_emit_method_table_id_ops_t *emit_method_table, scope_t *scope, qstr qst);

extern const emit_method_table_t emit_bc_method_table;
extern const emit_method_table_t emit_native_x64_method_table;
extern const emit_method_table_t emit_native_x86_method_table;
extern const emit_method_table_t emit_native_thumb_method_table;
extern const emit_method_table_t emit_native_arm_method_table;
extern const emit_method_table_t emit_native_xtensa_method_table;

extern const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_load_id_ops;
extern const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_store_id_ops;
extern const mp_emit_method_table_id_ops_t mp_emit_bc_method_table_delete_id_ops;

emit_t *emit_bc_new(void);
emit_t *emit_native_x64_new(mp_obj_t *error_slot, mp_uint_t max_num_labels);
emit_t *emit_native_x86_new(mp_obj_t *error_slot, mp_uint_t max_num_labels);
emit_t *emit_native_thumb_new(mp_obj_t *error_slot, mp_uint_t max_num_labels);
emit_t *emit_native_arm_new(mp_obj_t *error_slot, mp_uint_t max_num_labels);
emit_t *emit_native_xtensa_new(mp_obj_t *error_slot, mp_uint_t max_num_labels);

void emit_bc_set_max_num_labels(emit_t* emit, mp_uint_t max_num_labels);

void emit_bc_free(emit_t *emit);
void emit_native_x64_free(emit_t *emit);
void emit_native_x86_free(emit_t *emit);
void emit_native_thumb_free(emit_t *emit);
void emit_native_arm_free(emit_t *emit);
void emit_native_xtensa_free(emit_t *emit);

void mp_emit_bc_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope);
void mp_emit_bc_end_pass(emit_t *emit);
bool mp_emit_bc_last_emit_was_return_value(emit_t *emit);
void mp_emit_bc_adjust_stack_size(emit_t *emit, mp_int_t delta);
void mp_emit_bc_set_source_line(emit_t *emit, mp_uint_t line);

void mp_emit_bc_load_fast(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_load_deref(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_load_name(emit_t *emit, qstr qst);
void mp_emit_bc_load_global(emit_t *emit, qstr qst);
void mp_emit_bc_store_fast(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_store_deref(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_store_name(emit_t *emit, qstr qst);
void mp_emit_bc_store_global(emit_t *emit, qstr qst);
void mp_emit_bc_delete_fast(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_delete_deref(emit_t *emit, qstr qst, mp_uint_t local_num);
void mp_emit_bc_delete_name(emit_t *emit, qstr qst);
void mp_emit_bc_delete_global(emit_t *emit, qstr qst);

void mp_emit_bc_label_assign(emit_t *emit, mp_uint_t l);
void mp_emit_bc_import_name(emit_t *emit, qstr qst);
void mp_emit_bc_import_from(emit_t *emit, qstr qst);
void mp_emit_bc_import_star(emit_t *emit);
void mp_emit_bc_load_const_tok(emit_t *emit, mp_token_kind_t tok);
void mp_emit_bc_load_const_small_int(emit_t *emit, mp_int_t arg);
void mp_emit_bc_load_const_str(emit_t *emit, qstr qst);
void mp_emit_bc_load_const_obj(emit_t *emit, mp_obj_t obj);
void mp_emit_bc_load_null(emit_t *emit);
void mp_emit_bc_load_attr(emit_t *emit, qstr qst);
void mp_emit_bc_load_method(emit_t *emit, qstr qst, bool is_super);
void mp_emit_bc_load_build_class(emit_t *emit);
void mp_emit_bc_load_subscr(emit_t *emit);
void mp_emit_bc_store_attr(emit_t *emit, qstr qst);
void mp_emit_bc_store_subscr(emit_t *emit);
void mp_emit_bc_delete_attr(emit_t *emit, qstr qst);
void mp_emit_bc_delete_subscr(emit_t *emit);
void mp_emit_bc_dup_top(emit_t *emit);
void mp_emit_bc_dup_top_two(emit_t *emit);
void mp_emit_bc_pop_top(emit_t *emit);
void mp_emit_bc_rot_two(emit_t *emit);
void mp_emit_bc_rot_three(emit_t *emit);
void mp_emit_bc_jump(emit_t *emit, mp_uint_t label);
void mp_emit_bc_pop_jump_if(emit_t *emit, bool cond, mp_uint_t label);
void mp_emit_bc_jump_if_or_pop(emit_t *emit, bool cond, mp_uint_t label);
void mp_emit_bc_unwind_jump(emit_t *emit, mp_uint_t label, mp_uint_t except_depth);
#define mp_emit_bc_break_loop mp_emit_bc_unwind_jump
#define mp_emit_bc_continue_loop mp_emit_bc_unwind_jump
void mp_emit_bc_setup_with(emit_t *emit, mp_uint_t label);
void mp_emit_bc_with_cleanup(emit_t *emit, mp_uint_t label);
void mp_emit_bc_setup_except(emit_t *emit, mp_uint_t label);
void mp_emit_bc_setup_finally(emit_t *emit, mp_uint_t label);
void mp_emit_bc_end_finally(emit_t *emit);
void mp_emit_bc_get_iter(emit_t *emit, bool use_stack);
void mp_emit_bc_for_iter(emit_t *emit, mp_uint_t label);
void mp_emit_bc_for_iter_end(emit_t *emit);
void mp_emit_bc_pop_block(emit_t *emit);
void mp_emit_bc_pop_except(emit_t *emit);
void mp_emit_bc_unary_op(emit_t *emit, mp_unary_op_t op);
void mp_emit_bc_binary_op(emit_t *emit, mp_binary_op_t op);
void mp_emit_bc_build_tuple(emit_t *emit, mp_uint_t n_args);
void mp_emit_bc_build_list(emit_t *emit, mp_uint_t n_args);
void mp_emit_bc_build_map(emit_t *emit, mp_uint_t n_args);
void mp_emit_bc_store_map(emit_t *emit);
#if MICROPY_PY_BUILTINS_SET
void mp_emit_bc_build_set(emit_t *emit, mp_uint_t n_args);
#endif
#if MICROPY_PY_BUILTINS_SLICE
void mp_emit_bc_build_slice(emit_t *emit, mp_uint_t n_args);
#endif
void mp_emit_bc_store_comp(emit_t *emit, scope_kind_t kind, mp_uint_t list_stack_index);
void mp_emit_bc_unpack_sequence(emit_t *emit, mp_uint_t n_args);
void mp_emit_bc_unpack_ex(emit_t *emit, mp_uint_t n_left, mp_uint_t n_right);
void mp_emit_bc_make_function(emit_t *emit, scope_t *scope, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults);
void mp_emit_bc_make_closure(emit_t *emit, scope_t *scope, mp_uint_t n_closed_over, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults);
void mp_emit_bc_call_function(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags);
void mp_emit_bc_call_method(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags);
void mp_emit_bc_return_value(emit_t *emit);
void mp_emit_bc_raise_varargs(emit_t *emit, mp_uint_t n_args);
void mp_emit_bc_yield_value(emit_t *emit);
void mp_emit_bc_yield_from(emit_t *emit);
void mp_emit_bc_start_except_handler(emit_t *emit);
void mp_emit_bc_end_except_handler(emit_t *emit);

typedef struct _emit_inline_asm_t emit_inline_asm_t;

typedef struct _emit_inline_asm_method_table_t {
    void (*start_pass)(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot);
    void (*end_pass)(emit_inline_asm_t *emit, mp_uint_t type_sig);
    mp_uint_t (*count_params)(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params);
    bool (*label)(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id);
    void (*op)(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args);
} emit_inline_asm_method_table_t;

extern const emit_inline_asm_method_table_t emit_inline_thumb_method_table;
extern const emit_inline_asm_method_table_t emit_inline_xtensa_method_table;

emit_inline_asm_t *emit_inline_thumb_new(mp_uint_t max_num_labels);
emit_inline_asm_t *emit_inline_xtensa_new(mp_uint_t max_num_labels);

void emit_inline_thumb_free(emit_inline_asm_t *emit);
void emit_inline_xtensa_free(emit_inline_asm_t *emit);

#if MICROPY_WARNINGS
void mp_emitter_warning(pass_kind_t pass, const char *msg);
#else
#define mp_emitter_warning(pass, msg)
#endif

#endif // MICROPY_INCLUDED_PY_EMIT_H
