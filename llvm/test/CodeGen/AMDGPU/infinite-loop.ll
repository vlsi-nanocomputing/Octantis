; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -march=amdgcn -verify-machineinstrs < %s | FileCheck -check-prefix=SI %s
; RUN: opt -mtriple=amdgcn-- -S -amdgpu-unify-divergent-exit-nodes -verify %s | FileCheck -check-prefix=IR %s

define amdgpu_kernel void @infinite_loop(i32 addrspace(1)* %out) {
; SI-LABEL: infinite_loop:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx2 s[0:1], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s3, 0xf000
; SI-NEXT:    s_mov_b32 s2, -1
; SI-NEXT:    v_mov_b32_e32 v0, 0x3e7
; SI-NEXT:  BB0_1: ; %loop
; SI-NEXT:    ; =>This Inner Loop Header: Depth=1
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; SI-NEXT:    s_branch BB0_1
; IR-LABEL: @infinite_loop(
; IR-NEXT:  entry:
; IR-NEXT:    br label [[LOOP:%.*]]
; IR:       loop:
; IR-NEXT:    store volatile i32 999, i32 addrspace(1)* [[OUT:%.*]], align 4
; IR-NEXT:    br label [[LOOP]]
;
entry:
  br label %loop

loop:
  store volatile i32 999, i32 addrspace(1)* %out, align 4
  br label %loop
}

define amdgpu_kernel void @infinite_loop_ret(i32 addrspace(1)* %out) {
; SI-LABEL: infinite_loop_ret:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    v_cmp_eq_u32_e32 vcc, 1, v0
; SI-NEXT:    s_and_saveexec_b64 s[2:3], vcc
; SI-NEXT:    s_cbranch_execz BB1_3
; SI-NEXT:  ; %bb.1: ; %loop.preheader
; SI-NEXT:    s_load_dwordx2 s[0:1], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s3, 0xf000
; SI-NEXT:    s_mov_b32 s2, -1
; SI-NEXT:    v_mov_b32_e32 v0, 0x3e7
; SI-NEXT:    s_and_b64 vcc, exec, -1
; SI-NEXT:  BB1_2: ; %loop
; SI-NEXT:    ; =>This Inner Loop Header: Depth=1
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; SI-NEXT:    s_cbranch_vccnz BB1_2
; SI-NEXT:  BB1_3: ; %UnifiedReturnBlock
; SI-NEXT:    s_endpgm
; IR-LABEL: @infinite_loop_ret(
; IR-NEXT:  entry:
; IR-NEXT:    [[TMP:%.*]] = tail call i32 @llvm.amdgcn.workitem.id.x()
; IR-NEXT:    [[COND:%.*]] = icmp eq i32 [[TMP]], 1
; IR-NEXT:    br i1 [[COND]], label [[LOOP:%.*]], label [[UNIFIEDRETURNBLOCK:%.*]]
; IR:       loop:
; IR-NEXT:    store volatile i32 999, i32 addrspace(1)* [[OUT:%.*]], align 4
; IR-NEXT:    br i1 true, label [[LOOP]], label [[UNIFIEDRETURNBLOCK]]
; IR:       UnifiedReturnBlock:
; IR-NEXT:    ret void
;
entry:
  %tmp = tail call i32 @llvm.amdgcn.workitem.id.x()
  %cond = icmp eq i32 %tmp, 1
  br i1 %cond, label %loop, label %return

loop:
  store volatile i32 999, i32 addrspace(1)* %out, align 4
  br label %loop

return:
  ret void
}

define amdgpu_kernel void @infinite_loops(i32 addrspace(1)* %out) {
; SI-LABEL: infinite_loops:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx2 s[0:1], s[0:1], 0x9
; SI-NEXT:    s_mov_b64 s[2:3], -1
; SI-NEXT:    s_cbranch_scc1 BB2_4
; SI-NEXT:  ; %bb.1:
; SI-NEXT:    s_mov_b32 s3, 0xf000
; SI-NEXT:    s_mov_b32 s2, -1
; SI-NEXT:    v_mov_b32_e32 v0, 0x378
; SI-NEXT:    s_and_b64 vcc, exec, -1
; SI-NEXT:  BB2_2: ; %loop2
; SI-NEXT:    ; =>This Inner Loop Header: Depth=1
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; SI-NEXT:    s_cbranch_vccnz BB2_2
; SI-NEXT:  ; %bb.3: ; %Flow
; SI-NEXT:    s_mov_b64 s[2:3], 0
; SI-NEXT:  BB2_4: ; %Flow2
; SI-NEXT:    s_and_b64 vcc, exec, s[2:3]
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b64 vcc, vcc
; SI-NEXT:    s_cbranch_vccz BB2_7
; SI-NEXT:  ; %bb.5:
; SI-NEXT:    s_mov_b32 s3, 0xf000
; SI-NEXT:    s_mov_b32 s2, -1
; SI-NEXT:    s_waitcnt expcnt(0)
; SI-NEXT:    v_mov_b32_e32 v0, 0x3e7
; SI-NEXT:    s_and_b64 vcc, exec, 0
; SI-NEXT:  BB2_6: ; %loop1
; SI-NEXT:    ; =>This Inner Loop Header: Depth=1
; SI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; SI-NEXT:    s_cbranch_vccz BB2_6
; SI-NEXT:  BB2_7: ; %DummyReturnBlock
; SI-NEXT:    s_endpgm
; IR-LABEL: @infinite_loops(
; IR-NEXT:  entry:
; IR-NEXT:    br i1 undef, label [[LOOP1:%.*]], label [[LOOP2:%.*]]
; IR:       loop1:
; IR-NEXT:    store volatile i32 999, i32 addrspace(1)* [[OUT:%.*]], align 4
; IR-NEXT:    br i1 true, label [[LOOP1]], label [[DUMMYRETURNBLOCK:%.*]]
; IR:       loop2:
; IR-NEXT:    store volatile i32 888, i32 addrspace(1)* [[OUT]], align 4
; IR-NEXT:    br i1 true, label [[LOOP2]], label [[DUMMYRETURNBLOCK]]
; IR:       DummyReturnBlock:
; IR-NEXT:    ret void
;
entry:
  br i1 undef, label %loop1, label %loop2

loop1:
  store volatile i32 999, i32 addrspace(1)* %out, align 4
  br label %loop1

loop2:
  store volatile i32 888, i32 addrspace(1)* %out, align 4
  br label %loop2
}

define amdgpu_kernel void @infinite_loop_nest_ret(i32 addrspace(1)* %out) {
; SI-LABEL: infinite_loop_nest_ret:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    v_cmp_eq_u32_e32 vcc, 1, v0
; SI-NEXT:    s_and_saveexec_b64 s[2:3], vcc
; SI-NEXT:    s_cbranch_execz BB3_5
; SI-NEXT:  ; %bb.1: ; %outer_loop.preheader
; SI-NEXT:    s_load_dwordx2 s[4:5], s[0:1], 0x9
; SI-NEXT:    v_cmp_eq_u32_e32 vcc, 3, v0
; SI-NEXT:    v_cmp_ne_u32_e64 s[0:1], 3, v0
; SI-NEXT:    s_mov_b64 s[2:3], 0
; SI-NEXT:    s_mov_b32 s7, 0xf000
; SI-NEXT:    s_mov_b32 s6, -1
; SI-NEXT:  BB3_2: ; %outer_loop
; SI-NEXT:    ; =>This Loop Header: Depth=1
; SI-NEXT:    ; Child Loop BB3_3 Depth 2
; SI-NEXT:    s_and_b64 s[8:9], exec, vcc
; SI-NEXT:    s_or_b64 s[2:3], s[8:9], s[2:3]
; SI-NEXT:    s_mov_b64 s[8:9], 0
; SI-NEXT:  BB3_3: ; %inner_loop
; SI-NEXT:    ; Parent Loop BB3_2 Depth=1
; SI-NEXT:    ; => This Inner Loop Header: Depth=2
; SI-NEXT:    s_and_b64 s[10:11], exec, s[0:1]
; SI-NEXT:    s_or_b64 s[8:9], s[10:11], s[8:9]
; SI-NEXT:    s_waitcnt expcnt(0)
; SI-NEXT:    v_mov_b32_e32 v0, 0x3e7
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    buffer_store_dword v0, off, s[4:7], 0
; SI-NEXT:    s_andn2_b64 exec, exec, s[8:9]
; SI-NEXT:    s_cbranch_execnz BB3_3
; SI-NEXT:  ; %bb.4: ; %Flow
; SI-NEXT:    ; in Loop: Header=BB3_2 Depth=1
; SI-NEXT:    s_or_b64 exec, exec, s[8:9]
; SI-NEXT:    s_andn2_b64 exec, exec, s[2:3]
; SI-NEXT:    s_cbranch_execnz BB3_2
; SI-NEXT:  BB3_5: ; %UnifiedReturnBlock
; SI-NEXT:    s_endpgm
; IR-LABEL: @infinite_loop_nest_ret(
; IR-NEXT:  entry:
; IR-NEXT:    [[TMP:%.*]] = tail call i32 @llvm.amdgcn.workitem.id.x()
; IR-NEXT:    [[COND1:%.*]] = icmp eq i32 [[TMP]], 1
; IR-NEXT:    br i1 [[COND1]], label [[OUTER_LOOP:%.*]], label [[UNIFIEDRETURNBLOCK:%.*]]
; IR:       outer_loop:
; IR-NEXT:    br label [[INNER_LOOP:%.*]]
; IR:       inner_loop:
; IR-NEXT:    store volatile i32 999, i32 addrspace(1)* [[OUT:%.*]], align 4
; IR-NEXT:    [[COND3:%.*]] = icmp eq i32 [[TMP]], 3
; IR-NEXT:    br i1 true, label [[TRANSITIONBLOCK:%.*]], label [[UNIFIEDRETURNBLOCK]]
; IR:       TransitionBlock:
; IR-NEXT:    br i1 [[COND3]], label [[INNER_LOOP]], label [[OUTER_LOOP]]
; IR:       UnifiedReturnBlock:
; IR-NEXT:    ret void
;
entry:
  %tmp = tail call i32 @llvm.amdgcn.workitem.id.x()
  %cond1 = icmp eq i32 %tmp, 1
  br i1 %cond1, label %outer_loop, label %return

outer_loop:
  ; %cond2 = icmp eq i32 %tmp, 2
  ; br i1 %cond2, label %outer_loop, label %inner_loop
  br label %inner_loop

inner_loop:                                     ; preds = %LeafBlock, %LeafBlock1
  store volatile i32 999, i32 addrspace(1)* %out, align 4
  %cond3 = icmp eq i32 %tmp, 3
  br i1 %cond3, label %inner_loop, label %outer_loop

return:
  ret void
}

declare i32 @llvm.amdgcn.workitem.id.x()
