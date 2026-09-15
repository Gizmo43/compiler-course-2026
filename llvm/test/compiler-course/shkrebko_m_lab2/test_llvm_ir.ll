; RUN: opt -load-pass-plugin %llvmshlibdir/shkrebko_m_lab2_LLVM_IR%pluginext\
; RUN: -passes=invert-relational-icmp -S %s | FileCheck %s

; CHECK-LABEL: @test_sge_const
; CHECK-NEXT:    %cmp.rev = icmp slt i32 %x, 42
; CHECK-NEXT:    %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:    ret i1 %cmp.not
define i1 @test_sge_const(i32 %x) {
  %cmp = icmp sge i32 %x, 42
  ret i1 %cmp
}

; CHECK-LABEL: @test_ugt_or
; CHECK-NEXT:    %c1.rev = icmp ule i32 %a, %b
; CHECK-NEXT:    %c1.not = xor i1 %c1.rev, true
; CHECK-NEXT:    %c2.rev = icmp ule i32 %c, %d
; CHECK-NEXT:    %c2.not = xor i1 %c2.rev, true
; CHECK-NEXT:    %res = or i1 %c1.not, %c2.not
; CHECK-NEXT:    ret i1 %res
define i1 @test_ugt_or(i32 %a, i32 %b, i32 %c, i32 %d) {
  %c1 = icmp ugt i32 %a, %b
  %c2 = icmp ugt i32 %c, %d
  %res = or i1 %c1, %c2
  ret i1 %res
}

; CHECK-LABEL: @test_mixed_xor
; CHECK-NEXT:    %cmp1.rev = icmp sle i16 %x, %y
; CHECK-NEXT:    %cmp1.not = xor i1 %cmp1.rev, true
; CHECK-NEXT:    %cmp2 = icmp ult i16 %z, %w
; CHECK-NEXT:    %res = xor i1 %cmp1.not, %cmp2
; CHECK-NEXT:    ret i1 %res
define i1 @test_mixed_xor(i16 %x, i16 %y, i16 %z, i16 %w) {
  %cmp1 = icmp sgt i16 %x, %y
  %cmp2 = icmp ult i16 %z, %w
  %res = xor i1 %cmp1, %cmp2
  ret i1 %res
}

; CHECK-LABEL: @test_ptr_uge
; CHECK-NEXT:    %cmp.rev = icmp ult ptr %p, %q
; CHECK-NEXT:    %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:    ret i1 %cmp.not
define i1 @test_ptr_uge(ptr %p, ptr %q) {
  %cmp = icmp uge ptr %p, %q
  ret i1 %cmp
}

; CHECK-LABEL: @test_sge_and_sle
; CHECK-NEXT:    %c1.rev = icmp slt i64 %a, %b
; CHECK-NEXT:    %c1.not = xor i1 %c1.rev, true
; CHECK-NEXT:    %c2 = icmp sle i64 %c, %d
; CHECK-NEXT:    %res = and i1 %c1.not, %c2
; CHECK-NEXT:    ret i1 %res
define i1 @test_sge_and_sle(i64 %a, i64 %b, i64 %c, i64 %d) {
  %c1 = icmp sge i64 %a, %b
  %c2 = icmp sle i64 %c, %d
  %res = and i1 %c1, %c2
  ret i1 %res
}

; CHECK-LABEL: @test_ugt_zero
; CHECK-NEXT:    %cmp.rev = icmp ule i32 %val, 0
; CHECK-NEXT:    %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:    ret i1 %cmp.not
define i1 @test_ugt_zero(i32 %val) {
  %cmp = icmp ugt i32 %val, 0
  ret i1 %cmp
}