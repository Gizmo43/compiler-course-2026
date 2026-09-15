; RUN: opt -load-pass-plugin %llvmshlibdir/dolov_v_lab2_LLVM_IR%pluginext -passes=load-store-elim -S %s | FileCheck %s

define void @redundant_store_elimination(ptr %p) {
; CHECK-LABEL: @redundant_store_elimination(
; CHECK-NEXT:    %val = load i32, ptr %p
; CHECK-NEXT:    %res = add i32 %val, 1
; CHECK-NEXT:    ret void
  %val = load i32, ptr %p
  store i32 %val, ptr %p
  %res = add i32 %val, 1
  ret void
}

define void @same_const_store_elim(ptr %p) {
; CHECK-LABEL: @same_const_store_elim(
; CHECK-NEXT:    store i32 42, ptr %p
; CHECK-NEXT:    ret void
  store i32 42, ptr %p
  store i32 42, ptr %p
  ret void
}

define float @forward_float_val(ptr %p, float %val) {
; CHECK-LABEL: @forward_float_val(
; CHECK-NEXT:    store float %val, ptr %p
; CHECK-NEXT:    %res = fadd float %val, 1.0
; CHECK-NEXT:    ret float %res
  store float %val, ptr %p
  %loaded = load float, ptr %p
  %res = fadd float %loaded, 1.0
  ret float %res
}

define i32 @redundant_load_sequence(ptr %p) {
; CHECK-LABEL: @redundant_load_sequence(
; CHECK-NEXT:    %val1 = load i32, ptr %p
; CHECK-NEXT:    %extra = add i32 %val1, 10
; CHECK-NEXT:    %res = mul i32 %val1, %val1
; CHECK-NEXT:    ret i32 %res
  %val1 = load i32, ptr %p
  %extra = add i32 %val1, 10
  %val2 = load i32, ptr %p
  %res = mul i32 %val1, %val2
  ret i32 %res
}

define void @triple_store_elimination(ptr %p) {
; CHECK-LABEL: @triple_store_elimination(
; CHECK-NOT:     store i64 1
; CHECK-NOT:     store i64 2
; CHECK:         store i64 3, ptr %p
  store i64 1, ptr %p
  store i64 2, ptr %p
  store i64 3, ptr %p
  ret void
}

define i32 @aliasing_independence(ptr %p1, ptr %p2) {
; CHECK-LABEL: @aliasing_independence(
; CHECK:         %v1 = load i32, ptr %p1
; CHECK-NEXT:    store i32 99, ptr %p2
; CHECK-NEXT:    %res = add i32 %v1, %v1
  %v1 = load i32, ptr %p1
  store i32 99, ptr %p2
  %v2 = load i32, ptr %p1
  %res = add i32 %v1, %v2
  ret i32 %res
}

declare void @external_side_effect()
define i32 @call_clobber_test(ptr %p) {
; CHECK-LABEL: @call_clobber_test(
; CHECK:         store i32 7, ptr %p
; CHECK-NEXT:    call void @external_side_effect()
; CHECK-NEXT:    %reload = load i32, ptr %p
  store i32 7, ptr %p
  call void @external_side_effect()
  %reload = load i32, ptr %p
  ret i32 %reload
}

define i32 @barrier_safety(ptr %p) {
; CHECK-LABEL: @barrier_safety(
; CHECK:         store volatile i32 10, ptr %p
; CHECK:         %v1 = load volatile i32, ptr %p
; CHECK:         store atomic i32 20, ptr %p release, align 4
; CHECK:         %v2 = load atomic i32, ptr %p acquire, align 4
  store volatile i32 10, ptr %p
  %v1 = load volatile i32, ptr %p
  store atomic i32 20, ptr %p release, align 4
  %v2 = load atomic i32, ptr %p acquire, align 4
  %sum = add i32 %v1, %v2
  ret i32 %sum
}