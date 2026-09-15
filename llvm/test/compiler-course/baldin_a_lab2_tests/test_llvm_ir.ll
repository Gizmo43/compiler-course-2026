; RUN: opt -load-pass-plugin %llvmshlibdir/baldin_a_lab2_LLVM_IR%pluginext\
; RUN: -passes=IcmpInverse -S %s | FileCheck %s


; CHECK-LABEL: @_Z8test_sgtii
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp sle i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_sgtii(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_sgeii
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp slt i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_sgeii(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp sge i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_sltii
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp sge i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_sltii(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_sleii
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp sgt i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_sleii(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp sle i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_ugtjj
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp ule i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_ugtjj(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp ugt i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_ugejj
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp ult i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_ugejj(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_ultjj
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.inv = icmp uge i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:   ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_ultjj(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp ult i32 %a, %b
  ret i1 %cmp
}



; CHECK-LABEL: @_Z8test_ulejj
; CHECK-NEXT:  entry:
; CHECK-NEXT:  %cmp.inv = icmp ugt i32 %a, %b
; CHECK-NEXT:  %cmp.not = xor i1 %cmp.inv, true
; CHECK-NEXT:  ret i1 %cmp.not
; CHECK-NEXT: }

define dso_local noundef zeroext i1 @_Z8test_ulejj(i32 noundef %a, i32 noundef %b) {
entry:
  %cmp = icmp ule i32 %a, %b
  ret i1 %cmp
}