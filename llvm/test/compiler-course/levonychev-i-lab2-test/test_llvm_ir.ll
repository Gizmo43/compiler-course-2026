; RUN: opt -load-pass-plugin %llvmshlibdir/levonychev-i-lab2_LLVM_IR%pluginext\
; RUN: -passes=Icmp -S %s | FileCheck %s



; CHECK-LABEL: @test1
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp = icmp sle i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp, true
; CHECK-NEXT: %res = select i1 %cmp.not, i32 %a, i32 %b
; CHECK-NEXT: store i32 %res, ptr %out
; CHECK-NEXT: ret i1 %cmp.not
define i1 @test1(i32 %a, i32 %b, ptr %out) {
entry:
  %cmp = icmp sgt i32 %a, %b
  %res = select i1 %cmp, i32 %a, i32 %b
  store i32 %res, ptr %out
  ret i1 %cmp
}




; CHECK-LABEL: @test2
; CHECK-NEXT: entry:
; CHECK-NEXT: %c1 = icmp sle i32 %a, 10
; CHECK-NEXT: %c1.not = xor i1 %c1, true
; CHECK-NEXT: %c2 = icmp ult i32 %b, 20
; CHECK-NEXT: %res = and i1 %c1.not, %c2
define i1 @test2(i32 %a, i32 %b) {
entry:
  %c1 = icmp sgt i32 %a, 10
  %c2 = icmp ult i32 %b, 20
  %res = and i1 %c1, %c2
  ret i1 %res
}

; CHECK-LABEL: @test3
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp = icmp eq i32 %a, %b
; CHECK-NOT: xor i1 %cmp, true
define i1 @test3(i32 %a, i32 %b) {
entry:
  %cmp = icmp eq i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test4
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp = icmp ult i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @test4(i32 %a, i32 %b) {
entry:
  %cmp = icmp uge i32 %a, %b
  ret i1 %cmp
}


; CHECK-LABEL: @test5
; CHECK-NEXT: entry:
; CHECK-NEXT: %cmp = icmp ule i32 %a, %b
; CHECK-NEXT: %cmp.not = xor i1 %cmp, true
; CHECK-NEXT: ret i1 %cmp.not
define i1 @test5(i32 %a, i32 %b) {
entry:
  %cmp = icmp ugt i32 %a, %b
  ret i1 %cmp
}





