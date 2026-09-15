// RUN: %clang_cc1 -load %llvmshlibdir/shvetsova_k_noexcept_func_ClangAST%pluginext -plugin noexcept_plugin -fsyntax-only -fcxx-exceptions %s 2>&1 | FileCheck %s

// CHECK: FunctionDecl {{.*}} simpleSafeFunction 'int () noexcept'
int simpleSafeFunction() {
    return 42;
}

// CHECK: FunctionDecl {{.*}} complexSafeFunction 'int () noexcept'
int complexSafeFunction() {
    int a = 10;
    int b = 20;
    return a + b;
} 

// CHECK: FunctionDecl {{.*}} safeFunctionWithLoops 'int () noexcept'
int safeFunctionWithLoops() {
    int sum = 0;
    for(int i = 0; i < 10; ++i) {
        sum += i;
    }
    return sum;
}

// CHECK: FunctionDecl {{.*}} functionWithConditions 'int (int) noexcept'
int functionWithConditions(int x) {
    if (x > 0) return x;
    else return -x;
}

// CHECK: FunctionDecl {{.*}} functionWithSwitch 'int (int) noexcept'
int functionWithSwitch(int x) {
    switch(x) {
        case 1: return 10;
        default: return 0;
    }
}

namespace Outer {
    namespace Inner {
        // CHECK: FunctionDecl {{.*}} namespacedFunction 'int () noexcept'
        int namespacedFunction() { return 100; }
    }
    // CHECK: FunctionDecl {{.*}} outerFunction 'int () noexcept'
    int outerFunction() { return Inner::namespacedFunction(); }
}

// CHECK: FunctionDecl {{.*}} functionA 'int () noexcept'
int functionA() { return 1; }

// CHECK: FunctionDecl {{.*}} functionB 'int () noexcept'
int functionB() { return functionA() + 1; }

// CHECK: FunctionDecl {{.*}} main 'int () noexcept'
int main() {
    return 0;
}

// CHECK-NOT: FunctionDecl {{.*}} throwingFunction 'int () noexcept'
int throwingFunction() {
    throw 1;
    return 0;
}

// CHECK-NOT: FunctionDecl {{.*}} functionWithNestedThrow 'int () noexcept'
void nestedThrowHelper() { throw 42; }
int functionWithNestedThrow() {
    nestedThrowHelper();
    return 0;
}

// CHECK-NOT: FunctionDecl {{.*}} tryCatchFunction 'int () noexcept'
int tryCatchFunction() {
    try { return 1; } catch (...) { return 0; }
}

class TestClass {
public:
    // CHECK: CXXMethodDecl {{.*}} staticFunction 'int () noexcept' static
    static int staticFunction() { return 100; }
};

