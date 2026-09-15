; RUN: opt -load-pass-plugin %llvmshlibdir/krykov_e_instrument_func_LLVM_IR%pluginext \
; RUN:   -passes=instrument-functions -S %s | FileCheck %s

define i32 @simple(i32 %a, i32 %b) {
  %res = add i32 %a, %b
  ret i32 %res
}

; CHECK-LABEL: define i32 @simple(i32 %a, i32 %b) {
; CHECK-NEXT: call void @instrument_start()
; CHECK-NEXT: %res = add i32 %a, %b
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret i32 %res
; CHECK-NEXT: }

define i32 @branchy(i1 %cond) {
entry:
  br i1 %cond, label %then, label %else
then:
  ret i32 1
else:
  ret i32 0
}

; CHECK-LABEL: define i32 @branchy(i1 %cond) {
; CHECK-NEXT: entry:
; CHECK-NEXT: call void @instrument_start()
; CHECK-NEXT: br i1 %cond, label %then, label %else
; CHECK: then:
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret i32 1
; CHECK: else:
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret i32 0

define i32 @looped(i32 %n) {
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

; CHECK-LABEL: define i32 @looped(i32 %n) {
; CHECK-NEXT: entry:
; CHECK-NEXT: call void @instrument_start()
; CHECK-NEXT: br label %loop
; CHECK: loop:
; CHECK-NEXT: %i = phi i32 [ 0, %entry ], [ %next, %loop ]
; CHECK-NEXT: %next = add i32 %i, 1
; CHECK-NEXT: %cond = icmp ult i32 %next, %n
; CHECK-NEXT: br i1 %cond, label %loop, label %exit
; CHECK: exit:
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret i32 %i

define void @returns_void() {
  ret void
}

; CHECK-LABEL: define void @returns_void() {
; CHECK-NEXT: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret void
; CHECK-NEXT: }