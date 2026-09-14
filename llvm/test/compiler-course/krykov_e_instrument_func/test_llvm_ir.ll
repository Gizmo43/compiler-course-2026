; RUN: opt -load-pass-plugin %llvmshlibdir/krykov_e_instrument_func_LLVM_IR%pluginext \
; RUN:   -passes=instrument-functions -S %s | FileCheck %s

; CHECK-LABEL: @simple
; CHECK: call void @instrument_start()
; CHECK: %res = add i32 %a, %b
; CHECK: call void @instrument_end()
; CHECK: ret i32 %res

define i32 @simple(i32 %a, i32 %b) {
  %res = add i32 %a, %b
  ret i32 %res
}

; CHECK-LABEL: @branch
; CHECK: call void @instrument_start()
; CHECK: br i1 %cond, label %then, label %else
; CHECK: then:
; CHECK: call void @instrument_end()
; CHECK: ret i32 1
; CHECK: else:
; CHECK: call void @instrument_end()
; CHECK: ret i32 0

define i32 @branch(i1 %cond) {
entry:
  br i1 %cond, label %then, label %else
then:
  ret i32 1
else:
  ret i32 0
}

; CHECK-LABEL: @loop
; CHECK: call void @instrument_start()
; CHECK: loop:
; CHECK: %cond = icmp ult i32 %next, %n
; CHECK: br i1 %cond, label %loop, label %exit
; CHECK: exit:
; CHECK: call void @instrument_end()
; CHECK: ret i32 %i

define i32 @loop(i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next, %loop ]
  %next = add i32 %i, 1
  %cond = icmp ult i32 %next, %n
  br i1 %cond, label %loop, label %exit
exit:
  ret i32 %i
}

; CHECK-LABEL: @void
; CHECK: call void @instrument_start()
; CHECK: call void @instrument_end()
; CHECK: ret void

define void @void() {
  ret void
}

declare void @instrument_start()
declare void @instrument_end()
