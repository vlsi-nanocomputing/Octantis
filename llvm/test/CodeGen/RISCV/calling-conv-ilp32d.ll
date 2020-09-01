; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=riscv32 -verify-machineinstrs -mattr=+d \
; RUN:     -target-abi ilp32d < %s \
; RUN:   | FileCheck -check-prefix=RV32-ILP32D %s

; This file contains tests that will have differing output for the ilp32 and
; ilp32f ABIs.

define i32 @callee_double_in_fpr(i32 %a, double %b) nounwind {
; RV32-ILP32D-LABEL: callee_double_in_fpr:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    fcvt.w.d a1, fa0, rtz
; RV32-ILP32D-NEXT:    add a0, a0, a1
; RV32-ILP32D-NEXT:    ret
  %b_fptosi = fptosi double %b to i32
  %1 = add i32 %a, %b_fptosi
  ret i32 %1
}

define i32 @caller_double_in_fpr() nounwind {
; RV32-ILP32D-LABEL: caller_double_in_fpr:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI1_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI1_0)(a0)
; RV32-ILP32D-NEXT:    addi a0, zero, 1
; RV32-ILP32D-NEXT:    call callee_double_in_fpr
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call i32 @callee_double_in_fpr(i32 1, double 2.0)
  ret i32 %1
}

; Must keep define on a single line due to an update_llc_test_checks.py limitation
define i32 @callee_double_in_fpr_exhausted_gprs(i64 %a, i64 %b, i64 %c, i64 %d, i32 %e, double %f) nounwind {
; RV32-ILP32D-LABEL: callee_double_in_fpr_exhausted_gprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    lw a0, 0(sp)
; RV32-ILP32D-NEXT:    fcvt.w.d a1, fa0, rtz
; RV32-ILP32D-NEXT:    add a0, a0, a1
; RV32-ILP32D-NEXT:    ret
  %f_fptosi = fptosi double %f to i32
  %1 = add i32 %e, %f_fptosi
  ret i32 %1
}

define i32 @caller_double_in_fpr_exhausted_gprs() nounwind {
; RV32-ILP32D-LABEL: caller_double_in_fpr_exhausted_gprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi a1, zero, 5
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI3_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI3_0)(a0)
; RV32-ILP32D-NEXT:    addi a0, zero, 1
; RV32-ILP32D-NEXT:    addi a2, zero, 2
; RV32-ILP32D-NEXT:    addi a4, zero, 3
; RV32-ILP32D-NEXT:    addi a6, zero, 4
; RV32-ILP32D-NEXT:    sw a1, 0(sp)
; RV32-ILP32D-NEXT:    mv a1, zero
; RV32-ILP32D-NEXT:    mv a3, zero
; RV32-ILP32D-NEXT:    mv a5, zero
; RV32-ILP32D-NEXT:    mv a7, zero
; RV32-ILP32D-NEXT:    call callee_double_in_fpr_exhausted_gprs
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call i32 @callee_double_in_fpr_exhausted_gprs(
      i64 1, i64 2, i64 3, i64 4, i32 5, double 6.0)
  ret i32 %1
}

; Must keep define on a single line due to an update_llc_test_checks.py limitation
define i32 @callee_double_in_gpr_exhausted_fprs(double %a, double %b, double %c, double %d, double %e, double %f, double %g, double %h, double %i) nounwind {
; RV32-ILP32D-LABEL: callee_double_in_gpr_exhausted_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw a0, 8(sp)
; RV32-ILP32D-NEXT:    sw a1, 12(sp)
; RV32-ILP32D-NEXT:    fld ft0, 8(sp)
; RV32-ILP32D-NEXT:    fcvt.w.d a0, fa7, rtz
; RV32-ILP32D-NEXT:    fcvt.w.d a1, ft0, rtz
; RV32-ILP32D-NEXT:    add a0, a0, a1
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %h_fptosi = fptosi double %h to i32
  %i_fptosi = fptosi double %i to i32
  %1 = add i32 %h_fptosi, %i_fptosi
  ret i32 %1
}

define i32 @caller_double_in_gpr_exhausted_fprs() nounwind {
; RV32-ILP32D-LABEL: caller_double_in_gpr_exhausted_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI5_0)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_1)
; RV32-ILP32D-NEXT:    fld fa1, %lo(.LCPI5_1)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_2)
; RV32-ILP32D-NEXT:    fld fa2, %lo(.LCPI5_2)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_3)
; RV32-ILP32D-NEXT:    fld fa3, %lo(.LCPI5_3)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_4)
; RV32-ILP32D-NEXT:    fld fa4, %lo(.LCPI5_4)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_5)
; RV32-ILP32D-NEXT:    fld fa5, %lo(.LCPI5_5)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_6)
; RV32-ILP32D-NEXT:    fld fa6, %lo(.LCPI5_6)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI5_7)
; RV32-ILP32D-NEXT:    fld fa7, %lo(.LCPI5_7)(a0)
; RV32-ILP32D-NEXT:    lui a1, 262688
; RV32-ILP32D-NEXT:    mv a0, zero
; RV32-ILP32D-NEXT:    call callee_double_in_gpr_exhausted_fprs
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call i32 @callee_double_in_gpr_exhausted_fprs(
      double 1.0, double 2.0, double 3.0, double 4.0, double 5.0, double 6.0,
      double 7.0, double 8.0, double 9.0)
  ret i32 %1
}

; Must keep define on a single line due to an update_llc_test_checks.py limitation
define i32 @callee_double_in_gpr_and_stack_almost_exhausted_gprs_fprs(i64 %a, double %b, i64 %c, double %d, i64 %e, double %f, i32 %g, double %h, double %i, double %j, double %k, double %l, double %m) nounwind {
; RV32-ILP32D-LABEL: callee_double_in_gpr_and_stack_almost_exhausted_gprs_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    lw a0, 16(sp)
; RV32-ILP32D-NEXT:    sw a7, 8(sp)
; RV32-ILP32D-NEXT:    sw a0, 12(sp)
; RV32-ILP32D-NEXT:    fld ft0, 8(sp)
; RV32-ILP32D-NEXT:    fcvt.w.d a0, ft0, rtz
; RV32-ILP32D-NEXT:    add a0, a6, a0
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %m_fptosi = fptosi double %m to i32
  %1 = add i32 %g, %m_fptosi
  ret i32 %1
}

define i32 @caller_double_in_gpr_and_stack_almost_exhausted_gprs_fprs() nounwind {
; RV32-ILP32D-LABEL: caller_double_in_gpr_and_stack_almost_exhausted_gprs_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    lui a1, 262816
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI7_0)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_1)
; RV32-ILP32D-NEXT:    fld fa1, %lo(.LCPI7_1)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_2)
; RV32-ILP32D-NEXT:    fld fa2, %lo(.LCPI7_2)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_3)
; RV32-ILP32D-NEXT:    fld fa3, %lo(.LCPI7_3)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_4)
; RV32-ILP32D-NEXT:    fld fa4, %lo(.LCPI7_4)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_5)
; RV32-ILP32D-NEXT:    fld fa5, %lo(.LCPI7_5)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_6)
; RV32-ILP32D-NEXT:    fld fa6, %lo(.LCPI7_6)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI7_7)
; RV32-ILP32D-NEXT:    fld fa7, %lo(.LCPI7_7)(a0)
; RV32-ILP32D-NEXT:    addi a0, zero, 1
; RV32-ILP32D-NEXT:    addi a2, zero, 3
; RV32-ILP32D-NEXT:    addi a4, zero, 5
; RV32-ILP32D-NEXT:    addi a6, zero, 7
; RV32-ILP32D-NEXT:    sw a1, 0(sp)
; RV32-ILP32D-NEXT:    mv a1, zero
; RV32-ILP32D-NEXT:    mv a3, zero
; RV32-ILP32D-NEXT:    mv a5, zero
; RV32-ILP32D-NEXT:    mv a7, zero
; RV32-ILP32D-NEXT:    call callee_double_in_gpr_and_stack_almost_exhausted_gprs_fprs
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call i32 @callee_double_in_gpr_and_stack_almost_exhausted_gprs_fprs(
      i64 1, double 2.0, i64 3, double 4.0, i64 5, double 6.0, i32 7, double 8.0,
      double 9.0, double 10.0, double 11.0, double 12.0, double 13.0)
  ret i32 %1
}


; Must keep define on a single line due to an update_llc_test_checks.py limitation
define i32 @callee_double_on_stack_exhausted_gprs_fprs(i64 %a, double %b, i64 %c, double %d, i64 %e, double %f, i64 %g, double %h, double %i, double %j, double %k, double %l, double %m) nounwind {
; RV32-ILP32D-LABEL: callee_double_on_stack_exhausted_gprs_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    fld ft0, 0(sp)
; RV32-ILP32D-NEXT:    fcvt.w.d a0, ft0, rtz
; RV32-ILP32D-NEXT:    add a0, a6, a0
; RV32-ILP32D-NEXT:    ret
  %g_trunc = trunc i64 %g to i32
  %m_fptosi = fptosi double %m to i32
  %1 = add i32 %g_trunc, %m_fptosi
  ret i32 %1
}

define i32 @caller_double_on_stack_exhausted_gprs_fprs() nounwind {
; RV32-ILP32D-LABEL: caller_double_on_stack_exhausted_gprs_fprs:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    lui a0, 262816
; RV32-ILP32D-NEXT:    sw a0, 4(sp)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI9_0)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_1)
; RV32-ILP32D-NEXT:    fld fa1, %lo(.LCPI9_1)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_2)
; RV32-ILP32D-NEXT:    fld fa2, %lo(.LCPI9_2)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_3)
; RV32-ILP32D-NEXT:    fld fa3, %lo(.LCPI9_3)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_4)
; RV32-ILP32D-NEXT:    fld fa4, %lo(.LCPI9_4)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_5)
; RV32-ILP32D-NEXT:    fld fa5, %lo(.LCPI9_5)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_6)
; RV32-ILP32D-NEXT:    fld fa6, %lo(.LCPI9_6)(a0)
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI9_7)
; RV32-ILP32D-NEXT:    fld fa7, %lo(.LCPI9_7)(a0)
; RV32-ILP32D-NEXT:    addi a0, zero, 1
; RV32-ILP32D-NEXT:    addi a2, zero, 3
; RV32-ILP32D-NEXT:    addi a4, zero, 5
; RV32-ILP32D-NEXT:    addi a6, zero, 7
; RV32-ILP32D-NEXT:    sw zero, 0(sp)
; RV32-ILP32D-NEXT:    mv a1, zero
; RV32-ILP32D-NEXT:    mv a3, zero
; RV32-ILP32D-NEXT:    mv a5, zero
; RV32-ILP32D-NEXT:    mv a7, zero
; RV32-ILP32D-NEXT:    call callee_double_on_stack_exhausted_gprs_fprs
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call i32 @callee_double_on_stack_exhausted_gprs_fprs(
      i64 1, double 2.0, i64 3, double 4.0, i64 5, double 6.0, i64 7, double 8.0,
      double 9.0, double 10.0, double 11.0, double 12.0, double 13.0)
  ret i32 %1
}

define double @callee_double_ret() nounwind {
; RV32-ILP32D-LABEL: callee_double_ret:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    lui a0, %hi(.LCPI10_0)
; RV32-ILP32D-NEXT:    fld fa0, %lo(.LCPI10_0)(a0)
; RV32-ILP32D-NEXT:    ret
  ret double 1.0
}

define i32 @caller_double_ret() nounwind {
; RV32-ILP32D-LABEL: caller_double_ret:
; RV32-ILP32D:       # %bb.0:
; RV32-ILP32D-NEXT:    addi sp, sp, -16
; RV32-ILP32D-NEXT:    sw ra, 12(sp)
; RV32-ILP32D-NEXT:    call callee_double_ret
; RV32-ILP32D-NEXT:    fsd fa0, 0(sp)
; RV32-ILP32D-NEXT:    lw a0, 0(sp)
; RV32-ILP32D-NEXT:    lw ra, 12(sp)
; RV32-ILP32D-NEXT:    addi sp, sp, 16
; RV32-ILP32D-NEXT:    ret
  %1 = call double @callee_double_ret()
  %2 = bitcast double %1 to i64
  %3 = trunc i64 %2 to i32
  ret i32 %3
}
