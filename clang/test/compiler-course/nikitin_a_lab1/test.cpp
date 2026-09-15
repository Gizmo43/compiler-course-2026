// RUN: %clang_cc1 -load %llvmshlibdir/nikitin_a_lab_1_ClangAST%pluginext -plugin nikitin_a_lab_1_plugin -fsyntax-only %s 2>&1 | FileCheck %s

 
// Тест 1: Полностью неизменяемый указатель
 
// CHECK-LABEL: test_fully_unmodified_ptr
// CHECK: const int* const ptr {{=}}
void test_fully_unmodified_ptr() {
    int val = 100;
    int* ptr = &val;
    int x = *ptr;
    (void)x;
}



 
// Тест 2: Модифицируется только объект (*ptr = ...)
 
// CHECK-LABEL: test_pointee_modified_ptr
// CHECK: int* const ptr {{=}}
void test_pointee_modified_ptr() {
    int val = 300;
    int* ptr = &val;
    *ptr = 150;
}

 
// Тест 3: Модифицируются и указатель, и объект
 
// CHECK-LABEL: test_both_modified_ptr
// CHECK: int* ptr {{=}}
void test_both_modified_ptr() {
    int val = 400;
    int* ptr = &val;
    *ptr = 250;
    ptr = nullptr;
}

 
// Тест 4: Неизменяемая ссылка
 
// CHECK-LABEL: test_unmodified_ref
// CHECK: const int& ref {{=}}
void test_unmodified_ref() {
    int val = 500;
    int& ref = val;
    int x = ref;
    (void)x;
}

 
// Тест 5: Изменяемая ссылка
 
// CHECK-LABEL: test_modified_ref
// CHECK: int& ref {{=}}
// CHECK-NOT: const int& ref
void test_modified_ref() {
    int val = 600;
    int& ref = val;
    ref = 350;
}

 
// Тест 6: Параметр-указатель только для чтения
 
// CHECK-LABEL: test_param_ptr_readonly
// CHECK: void test_param_ptr_readonly(const int* const ptr)
void test_param_ptr_readonly(int* ptr) {
    int x = *ptr;
    (void)x;
}

 
// Тест 7: Параметр-указатель, модифицируется объект
 
// CHECK-LABEL: test_param_ptr_pointee_modified
// CHECK: void test_param_ptr_pointee_modified(int* const ptr)
void test_param_ptr_pointee_modified(int* ptr) {
    *ptr = 42;
}

 
// Тест 8: Параметр-указатель, переназначается
 
// CHECK-LABEL: test_param_ptr_reassigned
// CHECK: void test_param_ptr_reassigned(int* ptr)
void test_param_ptr_reassigned(int* ptr) {
    int local = 700;
    ptr = &local;
}

 
// Тест 9: Параметр-ссылка только для чтения
 
// CHECK-LABEL: test_param_ref_readonly
// CHECK: void test_param_ref_readonly(const int& ref)
void test_param_ref_readonly(int& ref) {
    int x = ref;
    (void)x;
}

 
// Тест 10: Параметр-ссылка изменяемая
 
// CHECK-LABEL: test_param_ref_modified
// CHECK: void test_param_ref_modified(int& ref)
void test_param_ref_modified(int& ref) {
    ref = 55;
}

 
// Тест 11: Локальная переменная-указатель
 
// CHECK-LABEL: test_local_ptr_candidate
// CHECK: const int* const local_ptr {{=}}
void test_local_ptr_candidate() {
    int value = 800;
    int* local_ptr = &value;
    int x = *local_ptr;
    (void)x;
}

 
// Тест 12: Локальная переменная-ссылка
 
// CHECK-LABEL: test_local_ref_candidate
// CHECK: const int& local_ref {{=}}
void test_local_ref_candidate() {
    int value = 900;
    int& local_ref = value;
    int x = local_ref;
    (void)x;
}

 
// Тест 13: Доступ к массиву через указатель (модификация)
 
// CHECK-LABEL: test_array_modify
// CHECK: int* const arr {{=}}
void test_array_modify() {
    int data[] = {10, 20, 30};
    int* arr = data;
    arr[1] = 42;
}

 
// Тест 14: Доступ к массиву через указатель (только чтение)
 
// CHECK-LABEL: test_array_readonly
// CHECK: const int* const arr {{=}}
void test_array_readonly() {
    int data[] = {11, 22, 33};
    int* arr = data;
    int x = arr[1];
    (void)x;
}

 
// Тест 15: Структура с указателем
 
struct TestStruct {
    int a;
    int b;
};

// CHECK-LABEL: test_struct_pointer_modify_field
// CHECK: TestStruct* const s {{=}}
void test_struct_pointer_modify_field() {
    TestStruct obj{44, 55};
    TestStruct* s = &obj;
    s->a = 66;
}

// CHECK-LABEL: test_struct_pointer_readonly
// CHECK: const TestStruct* const s {{=}}
void test_struct_pointer_readonly() {
    TestStruct obj{77, 88};
    TestStruct* s = &obj;
    int x = s->a + s->b;
    (void)x;
}

 
// Тест 16: Смешанные параметры
 
// CHECK-LABEL: test_mixed_parameters
// CHECK: void test_mixed_parameters(const int* const p, const int& r, int* const q)
void test_mixed_parameters(int* p, int& r, int* q) {
    int x = *p + r;
    *q = 99;
}

 
// Тест 17: Уже const переменные (не должны меняться)
 
// CHECK-LABEL: test_already_const_ref
// CHECK: const int& value
void test_already_const_ref(const int& value) {
    int x = value;
    (void)x;
}

// CHECK-LABEL: test_already_const_ptr
// CHECK: const int* const ptr
void test_already_const_ptr(const int* const ptr) {
    int x = *ptr;
    (void)x;
}

 
// Тест 18: Передача в функции (анализ вызовов)
 
void takes_const_ptr(const int*);
void takes_mut_ptr(int*);
void takes_const_ref(const int&);
void takes_mut_ref(int&);

// CHECK-LABEL: test_call_with_const_accepting
// CHECK: void test_call_with_const_accepting(const int* const ptr, const int& ref)
void test_call_with_const_accepting(int* ptr, int& ref) {
    takes_const_ptr(ptr);
    takes_const_ref(ref);
}

// CHECK-LABEL: test_call_with_mut_accepting
// CHECK: void test_call_with_mut_accepting(int* ptr, int& ref)
void test_call_with_mut_accepting(int* ptr, int& ref) {
    takes_mut_ptr(ptr);
    takes_mut_ref(ref);
}

 
// Тест 19: Цикл for с инкрементом указателя
 
// CHECK-LABEL: test_for_loop_ptr_modified
// CHECK: void test_for_loop_ptr_modified(const int* ptr)
void test_for_loop_ptr_modified(int* ptr) {
    for (int i = 0; i < 10; ++i) {
        *ptr = i;
        ptr++;
    }
}

// CHECK-LABEL: test_for_loop_ptr_readonly
// CHECK: void test_for_loop_ptr_readonly(const int* const ptr)
void test_for_loop_ptr_readonly(int* ptr) {
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        sum += ptr[i];
    }
    (void)sum;
}

 
// Тест 20: Условный оператор
 
// CHECK-LABEL: test_conditional_ptr
// CHECK: void test_conditional_ptr(const int* const ptr)
void test_conditional_ptr(int* ptr) {
    int x = (ptr != nullptr) ? *ptr : 0;
    (void)x;
}

 
// Тест 21: Возврат указателя из функции
 
int* test_return_ptr(int* ptr) {
    // ptr не модифицируется внутри, но возвращается
    // Это должно остаться без const, так как может быть изменён вне
    return ptr;
}

 
// Тест 22: Оператор стрелка (->) и точка (.)
 
struct Node {
    int value;
    Node* next;
};

// CHECK-LABEL: test_arrow_operator
// CHECK: Node* const node {{=}}
void test_arrow_operator() {
    Node n{10, nullptr};
    Node* node = &n;
    node->value = 20;
}

// CHECK-LABEL: test_arrow_operator_readonly
// CHECK: const Node* const node {{=}}
void test_arrow_operator_readonly() {
    Node n{10, nullptr};
    Node* node = &n;
    int x = node->value;
    (void)x;
}

 
// Тест 23: Ссылка на указатель
 
// CHECK-LABEL: test_ref_to_ptr
// CHECK: void test_ref_to_ptr(int* const& rptr)
void test_ref_to_ptr(int*& rptr) {
    int x = *rptr;
    (void)x;
}

 
// Тест 24: Указатель на константу (уже const)
 
// CHECK-LABEL: test_ptr_to_const
// CHECK: const int* ptr
void test_ptr_to_const(const int* ptr) {
    int x = *ptr;
    (void)x;
}

 
// Тест 25: Модификация через разыменование в составном выражении
 
// CHECK-LABEL: test_complex_modification
// CHECK: int* const ptr {{=}}
void test_complex_modification() {
    int val = 100;
    int* ptr = &val;
    int x = (*ptr)++ + 5;
    (void)x;
}

 
// Тест 26: Лямбда-выражение (если поддерживается C++11)
 
// CHECK-LABEL: test_lambda_capture
// CHECK: const int* const captured_ptr {{=}}
void test_lambda_capture() {
    int val = 100;
    int* captured_ptr = &val;
    auto lambda = [captured_ptr]() {
        int x = *captured_ptr;
        (void)x;
    };
    lambda();
}

 
// Тест 27: Несколько переменных в одном объявлении
 
// CHECK-LABEL: test_multiple_decl
// CHECK: const int* const a, const int* const b
void test_multiple_decl() {
    int x = 1, y = 2;
    int* a = &x, *b = &y;
    int s = *a + *b;
    (void)s;
}
