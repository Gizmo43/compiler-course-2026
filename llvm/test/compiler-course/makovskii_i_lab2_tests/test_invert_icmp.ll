; RUN: opt -load-pass-plugin %llvmshlibdir/makovskii_i_lab2_LLVM_IR%pluginext\
; RUN: -passes=invert-relational-icmp -S %s | FileCheck %s


; CHECK-LABEL: @test_slt
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.rev = icmp sge i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:   ret i1 %cmp.not
define i1 @test_slt(i32 %a, i32 %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test_sgt
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.rev = icmp sle i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:   ret i1 %cmp.not
define i1 @test_sgt(i32 %a, i32 %b) {
entry:
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test_ule
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.rev = icmp ugt i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:   ret i1 %cmp.not
define i1 @test_ule(i32 %a, i32 %b) {
entry:
  %cmp = icmp ule i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test_uge
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.rev = icmp ult i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.rev, true
; CHECK-NEXT:   ret i1 %cmp.not
define i1 @test_uge(i32 %a, i32 %b) {
entry:
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test_eq
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp = icmp eq i32 %a, %b
; CHECK-NEXT:   ret i1 %cmp
define i1 @test_eq(i32 %a, i32 %b) {
entry:
  %cmp = icmp eq i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test_ne
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp = icmp ne i32 %a, %b
; CHECK-NEXT:   ret i1 %cmp
define i1 @test_ne(i32 %a, i32 %b) {
entry:
  %cmp = icmp ne i32 %a, %b
  ret i1 %cmp
}