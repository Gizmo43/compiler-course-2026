; RUN: opt -load-pass-plugin=%llvmshlibdir/kiselev_i_second_lab_v_6_LLVM_IR%pluginext -passes=kiselev-load-store-elimination -S %s | FileCheck %s

define i32 @lse_basic_load(ptr %p) {
; CHECK-LABEL: @lse_basic_load(
; CHECK: %v1 = load i32, ptr %p
; CHECK-NEXT: %r = add i32 %v1, %v1
; CHECK-NEXT: ret i32 %r

  %v1 = load i32, ptr %p
  %v2 = load i32, ptr %p
  %r = add i32 %v1, %v2
  ret i32 %r
}

define i32 @lse_store_forward(ptr %p) {
; CHECK-LABEL: @lse_store_forward(
; CHECK: store i32 123, ptr %p
; CHECK-NEXT: ret i32 123

  store i32 123, ptr %p
  %v = load i32, ptr %p
  ret i32 %v
}

define void @lse_dead_store(ptr %p) {
; CHECK-LABEL: @lse_dead_store(
; CHECK: store i32 2, ptr %p
; CHECK-NEXT: ret void

  store i32 1, ptr %p
  store i32 2, ptr %p
  ret void
}

define void @lse_store_after_load(ptr %p) {
; CHECK-LABEL: @lse_store_after_load(
; CHECK: store i32 20, ptr %p
; CHECK-NEXT: ret void

  store i32 10, ptr %p
  %v = load i32, ptr %p
  store i32 20, ptr %p
  ret void
}

define i32 @lse_read_barrier(ptr %p, ptr %q) {
; CHECK-LABEL: @lse_read_barrier(
; CHECK: %a = load i32, ptr %p
; CHECK-NEXT: %b = load i32, ptr %q
; CHECK-NEXT: ret i32 %a

  %a = load i32, ptr %p
  %b = load i32, ptr %q
  %c = load i32, ptr %p
  ret i32 %c
}

define i32 @lse_write_barrier(ptr %p, ptr %q) {
; CHECK-LABEL: @lse_write_barrier(
; CHECK: %a = load i32, ptr %p
; CHECK-NEXT: store i32 7, ptr %q
; CHECK-NEXT: %c = load i32, ptr %p
; CHECK-NEXT: ret i32 %c

  %a = load i32, ptr %p
  store i32 7, ptr %q
  %c = load i32, ptr %p
  ret i32 %c
}

declare void @unknown_func()
define i32 @lse_call_clobber(ptr %p) {
; CHECK-LABEL: @lse_call_clobber(
; CHECK: store i32 5, ptr %p
; CHECK-NEXT: call void @unknown_func()
; CHECK-NEXT: %v = load i32, ptr %p
; CHECK-NEXT: ret i32 %v

  store i32 5, ptr %p
  call void @unknown_func()
  %v = load i32, ptr %p
  ret i32 %v
}

define i32 @lse_volatile(ptr %p) {
; CHECK-LABEL: @lse_volatile(
; CHECK: store volatile i32 1, ptr %p
; CHECK-NEXT: %v = load volatile i32, ptr %p
; CHECK-NEXT: ret i32 %v

  store volatile i32 1, ptr %p
  %v = load volatile i32, ptr %p
  ret i32 %v
}

define i32 @lse_atomic(ptr %p) {
; CHECK-LABEL: @lse_atomic(
; CHECK: store atomic i32 3, ptr %p seq_cst, align 4
; CHECK-NEXT: %v = load atomic i32, ptr %p seq_cst, align 4
; CHECK-NEXT: ret i32 %v

  store atomic i32 3, ptr %p seq_cst, align 4
  %v = load atomic i32, ptr %p seq_cst, align 4
  ret i32 %v
}

define i32 @lse_mixed(ptr %p) {
; CHECK-LABEL: @lse_mixed(
; CHECK: store i32 20, ptr %p
; CHECK-NEXT: ret i32 20

  store i32 10, ptr %p
  %v1 = load i32, ptr %p
  store i32 20, ptr %p
  %v2 = load i32, ptr %p
  ret i32 %v2
}