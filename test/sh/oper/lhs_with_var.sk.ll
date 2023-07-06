; ModuleID = './test/sh/oper/lhs_with_var.sk'
source_filename = "./test/sh/oper/lhs_with_var.sk"

@x = private global i64 1
@y = private global i64 0
@z = private global i1 false

define i64 @.lhs_with_var() {
entry:
  %0 = load i64, ptr @x, align 4
  %1 = add nsw i64 %0, 2
  store i64 %1, ptr @y, align 4
  %2 = load i64, ptr @x, align 4
  %3 = icmp slt i64 %2, 1
  store i1 %3, ptr @z, align 1
  ret i64 0
}
