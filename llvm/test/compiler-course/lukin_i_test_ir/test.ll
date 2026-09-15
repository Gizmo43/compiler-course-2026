; RUN: opt -load-pass-plugin %llvmshlibdir/lukin_i_lab2_LLVM_IR%pluginext\
; RUN: -passes=powi -S %s | FileCheck %s

; CHECK-LABEL: define dso_local noundef double @_Z12test_pow_negd
; CHECK: %1 = call double @llvm.powi.f64.i32(double %0, i32 -1)
define dso_local noundef double @_Z12test_pow_negd(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 -1)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow0d
; CHECK-NOT: call double @llvm.powi
; CHECK: ret double 1.000000e+00
define dso_local noundef double @_Z9test_pow0d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 0)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow1d
; CHECK-NOT: call double @llvm.powi
; CHECK: ret double %0
define dso_local noundef double @_Z9test_pow1d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 1)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow2d
; CHECK-NOT: call double @llvm.powi
; CHECK: [[MUL:%[0-9a-zA-Z_]+]] = fmul double %0, %0
; CHECK-NEXT: ret double [[MUL]]
define dso_local noundef double @_Z9test_pow2d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 2)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow3d
; CHECK-NOT: call double @llvm.powi
; CHECK: [[MUL2:%[0-9a-zA-Z_]+]] = fmul double %0, %0
; CHECK-NEXT: [[MUL3:%[0-9a-zA-Z_]+]] = fmul double [[MUL2]], %0
; CHECK-NEXT: ret double [[MUL3]]
define dso_local noundef double @_Z9test_pow3d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 3)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow4d
; CHECK-NOT: call double @llvm.powi
; CHECK: [[MUL2:%[0-9a-zA-Z_]+]] = fmul double %0, %0
; CHECK-NEXT: [[MUL4:%[0-9a-zA-Z_]+]] = fmul double [[MUL2]], [[MUL2]]
; CHECK-NEXT: ret double [[MUL4]]
define dso_local noundef double @_Z9test_pow4d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 4)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z9test_pow5d
; CHECK: %1 = call double @llvm.powi.f64.i32(double %0, i32 5)
define dso_local noundef double @_Z9test_pow5d(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 5)
  ret double %1
}

; CHECK-LABEL: define dso_local noundef double @_Z17test_pow_multipowd
; CHECK: [[VAL0:%[0-9a-zA-Z_]+]] = load double, ptr %a.addr, align 8
; CHECK-NEXT: [[MUL1:%[0-9a-zA-Z_]+]] = fmul double [[VAL0]], [[VAL0]]
; CHECK-NEXT: store double [[MUL1]], ptr %tmp, align 8
; CHECK-NEXT: [[VAL2:%[0-9a-zA-Z_]+]] = load double, ptr %tmp, align 8
; CHECK-NEXT: [[MUL2:%[0-9a-zA-Z_]+]] = fmul double [[VAL2]], [[VAL2]]
; CHECK-NEXT: [[MUL3:%[0-9a-zA-Z_]+]] = fmul double [[MUL2]], [[VAL2]]
; CHECK-NEXT: ret double [[MUL3]]
define dso_local noundef double @_Z17test_pow_multipowd(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  %tmp = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 2)
  store double %1, ptr %tmp, align 8
  %2 = load double, ptr %tmp, align 8
  %3 = call double @llvm.powi.f64.i32(double %2, i32 3)
  ret double %3
}

; CHECK-LABEL: define dso_local noundef double @_Z21test_pow_loopmultipowd
; CHECK: [[VAL0:%[0-9a-zA-Z_]+]] = load double, ptr %a.addr, align 8
; CHECK-NEXT: [[MUL1:%[0-9a-zA-Z_]+]] = fmul double [[VAL0]], [[VAL0]]
; CHECK-NEXT: store double [[MUL1]], ptr %tmp, align 8
; CHECK-NEXT: br label %while.body
; CHECK: while.body:
; CHECK-NEXT: [[VAL2:%[0-9a-zA-Z_]+]] = load double, ptr %tmp, align 8
; CHECK-NEXT: [[MUL2:%[0-9a-zA-Z_]+]] = fmul double [[VAL2]], [[VAL2]]
; CHECK-NEXT: store double [[MUL2]], ptr %tmp, align 8
; CHECK-NEXT: br label %while.end
; CHECK: while.end:
; CHECK-NEXT: [[VAL4:%[0-9a-zA-Z_]+]] = load double, ptr %tmp, align 8
; CHECK-NEXT: [[MUL4:%[0-9a-zA-Z_]+]] = fmul double [[VAL4]], [[VAL4]]
; CHECK-NEXT: ret double [[MUL4]]
define dso_local noundef double @_Z21test_pow_loopmultipowd(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  %tmp = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %1 = call double @llvm.powi.f64.i32(double %0, i32 2)
  store double %1, ptr %tmp, align 8
  br label %while.body

while.body:                                       
  %2 = load double, ptr %tmp, align 8
  %3 = call double @llvm.powi.f64.i32(double %2, i32 2)
  store double %3, ptr %tmp, align 8
  br label %while.end

while.end:                                        
  %4 = load double, ptr %tmp, align 8
  %5 = call double @llvm.powi.f64.i32(double %4, i32 2)
  ret double %5
}

; CHECK-LABEL: define dso_local noundef double @_Z17test_pow_variabledi
; CHECK: %2 = call double @llvm.powi.f64.i32(double %0, i32 %1)
define dso_local noundef double @_Z17test_pow_variabledi(double noundef %a, i32 noundef %n) #0 {
entry:
  %a.addr = alloca double, align 8
  %n.addr = alloca i32, align 4
  store double %a, ptr %a.addr, align 8
  store i32 %n, ptr %n.addr, align 4
  %0 = load double, ptr %a.addr, align 8
  %1 = load i32, ptr %n.addr, align 4
  %2 = call double @llvm.powi.f64.i32(double %0, i32 %1)
  ret double %2
}
