// RUN: %clang_cc1 -load %llvmshlibdir/krykov_e_c_cast_ClangAST%pluginext -plugin replace_c_cast -fsyntax-only %s | FileCheck %s

// CHECK: double b = static_cast<double>(a);
// CHECK: int *p = reinterpret_cast<int *>(a);
// CHECK: int *q = const_cast<int *>(&c);



void primitive_tests() {
  int a = 5;

  // CHECK: double b = static_cast<double>(a);
  double b = (double)a;



  const int c = 10;

  // CHECK: int *q = const_cast<int *>(&c);
  int *q = (int *)&c;

  // CHECK: int *p = reinterpret_cast<int *>(a);
  int *p = (int *)a;

  
}



struct Vec2 {
  int x;
  int y;
};

struct RawBlock {
  float data[2];
};

class Parent {
public:
  virtual ~Parent() {}
};

class Child : public Parent {
public:
  int value;
};

void user_type_tests() {

  Vec2 v{1,2};

  // CHECK: RawBlock *block = reinterpret_cast<RawBlock *>(&v);
  RawBlock *block = (RawBlock *)&v;

  Child child;

  // CHECK: Parent *p = static_cast<Parent *>(&child);
  Parent *p = (Parent *)&child;

  Parent *base = new Child();

  // CHECK: Child *c = static_cast<Child *>(base);
  Child *c = (Child *)base;

  const Vec2 const_v{0,0};

  // CHECK: Vec2 *mutable_v = const_cast<Vec2 *>(&const_v);
  Vec2 *mutable_v = (Vec2 *)&const_v;
}
