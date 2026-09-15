; RUN: opt -load-pass-plugin %llvmshlibdir/telnov_a_lab2_LLVM_IR%pluginext -passes=invert-relational-icmp -S %s | FileCheck %s

; CHECK-LABEL: define i1 @sgt_i32
; CHECK: %cmp.inv = icmp sle i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @sgt_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @sge_i32
; CHECK: %cmp.inv = icmp slt i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @sge_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp sge i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @ugt_i32
; CHECK: %cmp.inv = icmp ule i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @ugt_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp ugt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @uge_i32
; CHECK: %cmp.inv = icmp ult i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @uge_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @eq_i32
; CHECK: %cmp = icmp eq i32 %a, %b
; CHECK-NEXT: ret i1 %cmp
define i1 @eq_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp eq i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @slt_i32
; CHECK: %cmp = icmp slt i32 %a, %b
; CHECK-NEXT: ret i1 %cmp
define i1 @slt_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: define i1 @sle_i32
; CHECK: %cmp = icmp sle i32 %a, %b
; CHECK-NEXT: ret i1 %cmp
define i1 @sle_i32(i32 %a, i32 %b) {
entry:
  %cmp = icmp sle i32 %a, %b
  ret i1 %cmp
}