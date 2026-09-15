; RUN: split-file %s %t
; RUN: opt -load-pass-plugin %llvmshlibdir/krykov_e_instrument_func_LLVM_IR%pluginext \
; RUN:   -passes=instrument-functions -S %t/declared.ll | FileCheck %s --check-prefix=DECLARED
; RUN: opt -load-pass-plugin %llvmshlibdir/krykov_e_instrument_func_LLVM_IR%pluginext \
; RUN:   -passes=instrument-functions -S %t/defined.ll | FileCheck %s --check-prefix=DEFINED

;--- declared.ll

declare void @instrument_start()
declare void @instrument_end()

define i32 @foo(i32 %a) {
  ret i32 %a
}

; DECLARED: declare void @instrument_start()
; DECLARED: declare void @instrument_end()

; DECLARED-LABEL: define i32 @foo(i32 %a) {
; DECLARED-NEXT: call void @instrument_start()
; DECLARED-NEXT: call void @instrument_end()
; DECLARED-NEXT: ret i32 %a
; DECLARED-NEXT: }
; DECLARED-NOT: declare void @instrument_start()
; DECLARED-NOT: declare void @instrument_end()

;--- defined.ll

define void @instrument_start() {
  ret void
}

define void @instrument_end() {
  ret void
}

define i32 @foo(i32 %a) {
  ret i32 %a
}

; DEFINED-LABEL: define void @instrument_start() {
; DEFINED-NEXT: ret void
; DEFINED-NEXT: }

; DEFINED-LABEL: define void @instrument_end() {
; DEFINED-NEXT: ret void
; DEFINED-NEXT: }

; DEFINED-LABEL: define i32 @foo(i32 %a) {
; DEFINED-NEXT: call void @instrument_start()
; DEFINED-NEXT: call void @instrument_end()
; DEFINED-NEXT: ret i32 %a
; DEFINED-NEXT: }
