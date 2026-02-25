// RUN: cat %s | clang-repl | FileCheck %s
extern "C" int printf(const char *, ...);
int x1 = 0;
int x2 = 42;
%undo
int x2 = 24;
auto r1 = printf("x1 = %d\n", x1);
// CHECK: x1 = 0
auto r2 = printf("x2 = %d\n", x2);
// CHECK-NEXT: x2 = 24

int foo() { return 1; }
%undo
int foo() { return 2; }
auto r3 = printf("foo() = %d\n", foo());
// CHECK-NEXT: foo() = 2

inline int bar() { return 42;}
auto r4 = bar();
%undo
auto r5 = bar();

//--- Test file re-inclusion after undo with in-repl modification ---
// RUN: rm -rf %T && mkdir -p %T
// RUN: cp %S/Inputs/dynamic-header.h %T/dynamic-header-test.h
// RUN: cat %s | clang-repl -I%T | FileCheck %s
#include <cstdio>

#include "dynamic-header-test.h"
auto val1 = getDynamicValue();
%undo
%undo
// CHECK: val1 = 100
{
    FILE *f;
    fopen_s(&f, "%T/dynamic-header-test.h", "w");
    fprintf(f, "#ifndef DYNAMIC_HEADER_H\n");
    fprintf(f, "#define DYNAMIC_HEADER_H\n");
    fprintf(f, "inline int getDynamicValue() { return 200; }\n");
    fprintf(f, "#endif\n");
    fclose(f);
}
#include "dynamic-header-test.h"
auto val2 = getDynamicValue();
// CHECK-NEXT: val2 = 200

%quit
