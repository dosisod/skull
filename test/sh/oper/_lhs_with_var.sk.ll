; ModuleID = './test/sh/oper/lhs_with_var.sk'
source_filename = "./test/sh/oper/lhs_with_var.sk"

@x = private global i64 0
@y = private global i64 0
@z = private global i1 false

define i64 @.lhs_with_var() {
entry:
  store i64 1, i64* @x
  %0 = load i64, i64* @x
  %1 = add nsw i64 %0, 2
  store i64 %1, i64* @y
  %2 = load i64, i64* @x
  %3 = icmp slt i64 %2, 1
  store i1 %3, i1* @z
  ret i64 0
}
