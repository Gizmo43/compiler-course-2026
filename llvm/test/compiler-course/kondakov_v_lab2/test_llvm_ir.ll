; RUN: opt -load-pass-plugin %llvmshlibdir/kondakov_v_lab_2_LLVM_IR%pluginext \
; RUN: -passes=replace-icmp -S %s | FileCheck %s

define i1 @signed_gt(i32 %a, i32 %b) {
; CHECK-LABEL: @signed_gt(
; CHECK: [[CMP:%.*]] = icmp sle i32 %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}

define i1 @signed_ge(i32 %a, i32 %b) {
; CHECK-LABEL: @signed_ge(
; CHECK: [[CMP:%.*]] = icmp slt i32 %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp sge i32 %a, %b
  ret i1 %cmp
}

define i1 @unsigned_gt(i32 %a, i32 %b) {
; CHECK-LABEL: @unsigned_gt(
; CHECK: [[CMP:%.*]] = icmp ule i32 %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp ugt i32 %a, %b
  ret i1 %cmp
}

define i1 @unsigned_ge(i32 %a, i32 %b) {
; CHECK-LABEL: @unsigned_ge(
; CHECK: [[CMP:%.*]] = icmp ult i32 %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}

define i1 @pointer_uge(ptr %lhs, ptr %rhs) {
; CHECK-LABEL: @pointer_uge(
; CHECK: [[CMP:%.*]] = icmp ult ptr %lhs, %rhs
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp uge ptr %lhs, %rhs
  ret i1 %cmp
}

define <4 x i1> @vector_ugt(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: @vector_ugt(
; CHECK: [[CMP:%.*]] = icmp ule <4 x i32> %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor <4 x i1> [[CMP]], {{(<i1 true, i1 true, i1 true, i1 true>|splat \(i1 true\))}}
; CHECK-NEXT: ret <4 x i1> [[NEG]]
  %cmp = icmp ugt <4 x i32> %a, %b
  ret <4 x i1> %cmp
}

define i1 @samesign_sge(i32 %a, i32 %b) {
; CHECK-LABEL: @samesign_sge(
; CHECK: [[CMP:%.*]] = icmp samesign slt i32 %a, %b
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp samesign sge i32 %a, %b
  ret i1 %cmp
}

define i1 @edge_min_value(i32 %x) {
; CHECK-LABEL: @edge_min_value(
; CHECK: [[CMP:%.*]] = icmp slt i32 %x, -2147483648
; CHECK-NEXT: [[NEG:%.*]] = xor i1 [[CMP]], true
; CHECK-NEXT: ret i1 [[NEG]]
  %cmp = icmp sge i32 %x, -2147483648
  ret i1 %cmp
}

define i1 @eq_is_unchanged(i32 %a, i32 %b) {
; CHECK-LABEL: @eq_is_unchanged(
; CHECK: %cmp = icmp eq i32 %a, %b
; CHECK-NEXT: ret i1 %cmp
  %cmp = icmp eq i32 %a, %b
  ret i1 %cmp
}
