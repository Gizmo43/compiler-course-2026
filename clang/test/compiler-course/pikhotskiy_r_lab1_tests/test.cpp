// RUN: %clang_cc1 -load %llvmshlibdir/pikhotskiy_r_lab1_ClangAST%pluginext -plugin var_counter_plugin -fsyntax-only %s 2>&1 | FileCheck %s

// Counted as global variables (6):
//   global1, global2, anon_global1, ns_global, extern_global (definition),
//   Test::static_field (out-of-class definition)
// Not counted as globals: declarations `extern int extern_global;` and
// `extern unsigned externValue;`.
//
// Counted as local variables (7):
//   local1, local2, block_local, local3, local4, local5, method_static
//
// Counted as static variables (7):
//   static_global1, static_global2, anon_static1, ns_static,
//   static_local1, block_static, main_static
//
// Counted as function parameters (5):
//   a, b, x, argc, argv
// Not counted as parameters: declaration-only `void foo(int, double);`
//
// CHECK: Global variables: 6
// CHECK-NEXT: Local variables: 7
// CHECK-NEXT: Static variables: 7
// CHECK-NEXT: Function parameters: 5
// CHECK-NEXT: Total: 25

int global1 = 10;
int global2;

static int static_global1 = 5;
static int static_global2;

namespace {
    int anon_global1;
    static int anon_static1;
}

namespace MyNS {
    int ns_global;
    static int ns_static;
}

extern int extern_global;
extern unsigned externValue;
int extern_global = 42;
void foo(int, double);

int func1(int a, int b) {
    int local1 = 0;
    static int static_local1 = 0;
    return local1;
}

double func2(double x) {
    int local2 = 42;
    {
        int block_local = 100;
        static int block_static = 200;
    }
    return x;
}

class Test {
    int field;
    static int static_field;
public:
    Test(int p) {
        int local3 = 1;
    }
    void method(int p1, float p2) {
        int local4 = 2;
        static int method_static = 0;
    }
};

int Test::static_field = 0;

int main(int argc, char* argv[]) {
    int local5 = 3;
    static int main_static = 4;
    return 0;
}
