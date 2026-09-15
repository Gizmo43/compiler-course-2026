; RUN: opt -load-pass-plugin=%llvmshlibdir/load_store_elimination_LLVM_IR%pluginext -passes=load-store-elimination -S %s | FileCheck %s

define i32 @basic_load_elimination(ptr %p) {
; CHECK-LABEL: @basic_load_elimination(
; CHECK: %v1 = load i32, ptr %p
; CHECK-NEXT: %sum = add i32 %v1, %v1
; CHECK-NEXT: ret i32 %sum

  %v1 = load i32, ptr %p
  %v2 = load i32, ptr %p
  %sum = add i32 %v1, %v2
  ret i32 %sum
}

define i32 @store_to_load_forwarding(ptr %p) {
; CHECK-LABEL: @store_to_load_forwarding(
; CHECK: store i32 42, ptr %p
; CHECK-NEXT: ret i32 42

  store i32 42, ptr %p
  %v = load i32, ptr %p
  ret i32 %v
}

define void @dead_store_elimination(ptr %p) {
; CHECK-LABEL: @dead_store_elimination(
; CHECK: store i32 2, ptr %p
; CHECK-NEXT: ret void

  store i32 1, ptr %p
  store i32 2, ptr %p
  ret void
}

define void @store_after_visible_load(ptr %p) {
; CHECK-LABEL: @store_after_visible_load(
; CHECK: store i32 20, ptr %p
; CHECK-NEXT: ret void

  store i32 10, ptr %p
  %v = load i32, ptr %p
  store i32 20, ptr %p
  ret void
}

define i32 @load_preserved_across_other_load(ptr %p, ptr %q) {
; CHECK-LABEL: @load_preserved_across_other_load(
; CHECK: %a = load i32, ptr %p
; CHECK-NEXT: %b = load i32, ptr %q
; CHECK-NEXT: ret i32 %a

  %a = load i32, ptr %p
  %b = load i32, ptr %q
  %c = load i32, ptr %p
  ret i32 %c
}

define i32 @load_invalidated_by_store(ptr %p, ptr %q) {
; CHECK-LABEL: @load_invalidated_by_store(
; CHECK: %a = load i32, ptr %p
; CHECK-NEXT: store i32 7, ptr %q
; CHECK-NEXT: %c = load i32, ptr %p
; CHECK-NEXT: ret i32 %c

  %a = load i32, ptr %p
  store i32 7, ptr %q
  %c = load i32, ptr %p
  ret i32 %c
}

declare void @unknown_func()

define i32 @call_clobbers_memory(ptr %p) {
; CHECK-LABEL: @call_clobbers_memory(
; CHECK: store i32 5, ptr %p
; CHECK-NEXT: call void @unknown_func()
; CHECK-NEXT: %v = load i32, ptr %p
; CHECK-NEXT: ret i32 %v

  store i32 5, ptr %p
  call void @unknown_func()
  %v = load i32, ptr %p
  ret i32 %v
}

define i32 @volatile_operations_are_preserved(ptr %p) {
; CHECK-LABEL: @volatile_operations_are_preserved(
; CHECK: store volatile i32 1, ptr %p
; CHECK-NEXT: %v = load volatile i32, ptr %p
; CHECK-NEXT: ret i32 %v

  store volatile i32 1, ptr %p
  %v = load volatile i32, ptr %p
  ret i32 %v
}

define i32 @atomic_operations_are_preserved(ptr %p) {
; CHECK-LABEL: @atomic_operations_are_preserved(
; CHECK: store atomic i32 3, ptr %p seq_cst, align 4
; CHECK-NEXT: %v = load atomic i32, ptr %p seq_cst, align 4
; CHECK-NEXT: ret i32 %v

  store atomic i32 3, ptr %p seq_cst, align 4
  %v = load atomic i32, ptr %p seq_cst, align 4
  ret i32 %v
}

define i32 @atomic_store_invalidates_known_value(ptr %p) {
; CHECK-LABEL: @atomic_store_invalidates_known_value(
; CHECK: store i32 1, ptr %p
; CHECK-NEXT: store atomic i32 2, ptr %p seq_cst, align 4
; CHECK-NEXT: %v = load i32, ptr %p
; CHECK-NEXT: ret i32 %v

  store i32 1, ptr %p
  store atomic i32 2, ptr %p seq_cst, align 4
  %v = load i32, ptr %p
  ret i32 %v
}