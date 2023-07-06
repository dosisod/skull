; ModuleID = './test/sh/declare/bool_expr/var_rhs.sk'
source_filename = "./test/sh/declare/bool_expr/var_rhs.sk"

@x = private global i64 1
@y = private global i1 false

define i64 @.var_rhs() {
entry:
  %0 = load i64, ptr @x, align 4
  %1 = icmp eq i64 1, %0
  store i1 %1, ptr @y, align 1
  ret i64 0
}
