; RUN: opt -load-pass-plugin %llvmshlibdir/rozenberg_a_lab2_LLVM_IR%pluginext\
; RUN: -passes=icmp-replace-opposite -S %s | FileCheck %s

; CHECK-LABEL: @_Z7test_eqii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp ne i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z7test_eqii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp eq i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z7test_neii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp eq i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z7test_neii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp ne i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_ugtii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp ule i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_ugtii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp ugt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_ugeii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp ult i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_ugeii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_ultii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp uge i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_ultii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp ult i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_uleii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp ugt i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_uleii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp ule i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_sgtii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp sle i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_sgtii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_sgeii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp slt i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_sgeii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp sge i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_sltii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp sge i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_sltii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp slt i32 %a, %b
  ret i1 %cmp
}

; CHECK-LABEL: @_Z8test_sleii
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp_inv = icmp sgt i32 %a, %b
; CHECK-NEXT: %cmp_new = xor i1 %cmp_inv, true
; CHECK-NEXT: ret i1 %cmp_new
define dso_local noundef zeroext i1 @_Z8test_sleii(i32 noundef %a, i32 noundef %b) local_unnamed_addr #0 {
entry:
  %cmp = icmp sle i32 %a, %b
  ret i1 %cmp
}