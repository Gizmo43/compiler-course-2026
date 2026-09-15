; RUN: opt -load-pass-plugin %llvmshlibdir/sakharov_a_lab2_LLVM_IR%pluginext \
; RUN:   -passes=replace-icmp-gt-ge -S %s | FileCheck %s

define i1 @replace_sgt(i32 %a, i32 %b) {
; CHECK-LABEL: @replace_sgt(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.opp = icmp sle i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.opp, true
; CHECK-NEXT:   ret i1 %cmp.not
entry:
  %cmp = icmp sgt i32 %a, %b
  ret i1 %cmp
}

define i1 @replace_sge_in_branch(i32 %a, i32 %b) {
; CHECK-LABEL: @replace_sge_in_branch(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.opp = icmp slt i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.opp, true
; CHECK-NEXT:   br i1 %cmp.not, label %ge, label %lt
; CHECK: ge:
; CHECK-NEXT:   ret i1 true
; CHECK: lt:
; CHECK-NEXT:   ret i1 false
entry:
  %cmp = icmp sge i32 %a, %b
  br i1 %cmp, label %ge, label %lt

ge:
  ret i1 true

lt:
  ret i1 false
}

define i32 @replace_ugt_in_select(i32 %a, i32 %b) {
; CHECK-LABEL: @replace_ugt_in_select(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.opp = icmp ule i32 %a, %b
; CHECK-NEXT:   %cmp.not = xor i1 %cmp.opp, true
; CHECK-NEXT:   %res = select i1 %cmp.not, i32 %a, i32 %b
; CHECK-NEXT:   ret i32 %res
entry:
  %cmp = icmp ugt i32 %a, %b
  %res = select i1 %cmp, i32 %a, i32 %b
  ret i32 %res
}

define <2 x i1> @replace_uge_vector(<2 x i32> %a, <2 x i32> %b) {
; CHECK-LABEL: @replace_uge_vector(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp.opp = icmp ult <2 x i32> %a, %b
; CHECK-NEXT:   %cmp.not = xor <2 x i1> %cmp.opp, splat (i1 true)
; CHECK-NEXT:   ret <2 x i1> %cmp.not
entry:
  %cmp = icmp uge <2 x i32> %a, %b
  ret <2 x i1> %cmp
}

define i1 @replace_two_cmps(i32 %a, i32 %b, i32 %c) {
; CHECK-LABEL: @replace_two_cmps(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %first.opp = icmp sle i32 %a, %b
; CHECK-NEXT:   %first.not = xor i1 %first.opp, true
; CHECK-NEXT:   %second.opp = icmp ult i32 %b, %c
; CHECK-NEXT:   %second.not = xor i1 %second.opp, true
; CHECK-NEXT:   %both = and i1 %first.not, %second.not
; CHECK-NEXT:   ret i1 %both
entry:
  %first = icmp sgt i32 %a, %b
  %second = icmp uge i32 %b, %c
  %both = and i1 %first, %second
  ret i1 %both
}

define i1 @keep_other_predicates(i32 %a, i32 %b) {
; CHECK-LABEL: @keep_other_predicates(
; CHECK-NEXT: entry:
; CHECK-NEXT:   %eq = icmp eq i32 %a, %b
; CHECK-NEXT:   %ne = icmp ne i32 %a, %b
; CHECK-NEXT:   %slt = icmp slt i32 %a, %b
; CHECK-NEXT:   %ule = icmp ule i32 %a, %b
; CHECK-NEXT:   %tmp0 = xor i1 %eq, %ne
; CHECK-NEXT:   %tmp1 = xor i1 %slt, %ule
; CHECK-NEXT:   %res = or i1 %tmp0, %tmp1
; CHECK-NEXT:   ret i1 %res
entry:
  %eq = icmp eq i32 %a, %b
  %ne = icmp ne i32 %a, %b
  %slt = icmp slt i32 %a, %b
  %ule = icmp ule i32 %a, %b
  %tmp0 = xor i1 %eq, %ne
  %tmp1 = xor i1 %slt, %ule
  %res = or i1 %tmp0, %tmp1
  ret i1 %res
}
