// RUN: split-file %s %t && %clang_cc1 -load %llvmshlibdir/CstyleCastPlugin_Fedoseev_Sergey_FIIT0_ClangAST%pluginext -plugin cstyle_cast_plugin -fsyntax-only %t/input.cpp 2>&1 | FileCheck %t/input.cpp

//--- input.cpp
// Арифметические приведения
void arithmetic() {
    int i = 10;
    double d = (double)i; // CHECK: double d = static_cast<double>(i);
    float f = (float)i; // CHECK: float f = static_cast<float>(i);
    long l = (long)i; // CHECK: long l = static_cast<long>(i);
    short s = (short)i; // CHECK: short s = static_cast<short>(i);
    unsigned u = (unsigned)i; // CHECK: unsigned u = static_cast<unsigned>(i);
}

// const_cast
void const_qualifier() {
    const int ci = 5;
    int* ptr = (int*)&ci; // CHECK: int* ptr = const_cast<int*>(&ci);
    const int* cptr = (const int*)ptr; // CHECK: const int* cptr = const_cast<const int*>(ptr);
    volatile int vi = 10;
    int* vptr = (int*)&vi; // CHECK: int* vptr = const_cast<int*>(&vi);
    const volatile int cvi = 20;
    int* cvptr = (int*)&cvi; // CHECK: int* cvptr = const_cast<int*>(&cvi);
}

// reinterpret_cast (указатели на разные типы)
void pointer_cast() {
    int* p = nullptr;
    char* c = (char*)p; // CHECK: char* c = reinterpret_cast<char*>(p);
    void* v = (void*)p; // CHECK: void* v = reinterpret_cast<void*>(p);
    long long* ll = (long long*)p; // CHECK: long long* ll = reinterpret_cast<long long*>(p);
    int* p2 = (int*)c; // CHECK: int* p2 = reinterpret_cast<int*>(c);
}

// dynamic_cast (полиморфные классы)
struct Base { virtual ~Base() = default; };
struct Derived : Base {};
struct Another { virtual ~Another() = default; };
void polymorphic_cast() {
    Base* b = new Derived;
    Derived* d = (Derived*)b; // CHECK: Derived* d = dynamic_cast<Derived*>(b);
    Base& br = *b;
    Derived& dr = (Derived&)br; // CHECK: Derived& dr = dynamic_cast<Derived&>(br);
    Another* a = (Another*)b; // CHECK: Another* a = reinterpret_cast<Another*>(b);
}

// static_cast (неполиморфные указатели)
struct NonPolyBase {};
struct NonPolyDerived : NonPolyBase {};
void non_polymorphic_cast() {
    NonPolyBase* b = new NonPolyDerived;
    NonPolyDerived* d = (NonPolyDerived*)b; // CHECK: NonPolyDerived* d = reinterpret_cast<NonPolyDerived*>(b);
}

// Приведение к ссылке
void reference_cast() {
    int i = 42;
    double& d = (double&)i; // CHECK: double& d = static_cast<double&>(i);
    const int& cr = (const int&)i; // CHECK: const int& cr = static_cast<const int&>(i);
}

// Приведение указателя на функцию
void func() {}
void (*fp)() = &func;
void (*fp2)() = (void(*)())fp; // CHECK: void (*fp2)() = reinterpret_cast<void(*)()>(fp);

// Касты в выражениях
void expressions() {
    int a = 10, b = 20;
    long c = (long)a + b; // CHECK: long c = static_cast<long>(a) + b;
    double d = (double)(a + b); // CHECK: double d = static_cast<double>(a + b);
    int* p = &a;
    long long* q = (long long*)(p + 1); // CHECK: long long* q = reinterpret_cast<long long*>(p + 1);
}

// Вложенные касты
void nested() {
    int i = 10;
    double d = (double)((float)i); // CHECK: double d = static_cast<double>(static_cast<float>(i));
    int* p = &i;
    void* v = (void*)p;
    char* c = (char*)v; // CHECK: char* c = reinterpret_cast<char*>(v);
}

// Касты в шаблонном контексте
template<typename T>
void templ(T t) {
    int x = (int)t; // CHECK: int x = static_cast<int>(t);
}
void instantiate() {
    templ(3.14);
}

// Касты с классами, имеющими собственные операторы приведения
struct Convertible {
    operator int() const { return 42; }
    explicit operator double() const { return 3.14; }
};
void user_defined() {
    Convertible c;
    int i = (int)c; // CHECK: int i = static_cast<int>(c);
    double d = (double)c; // CHECK: double d = static_cast<double>(c);
}

// Касты в инициализации
struct S {
    int x;
    S(int y) : x((int)y) {} // CHECK: x(static_cast<int>(y))
};
void init() {
    S s((long)5); // CHECK: S s(static_cast<long>(5))
}

// Касты с массивами
void arrays() {
    int arr[10];
    int* p = (int*)arr; // CHECK: int* p = reinterpret_cast<int*>(arr);
    char* c = (char*)arr; // CHECK: char* c = reinterpret_cast<char*>(arr);
}