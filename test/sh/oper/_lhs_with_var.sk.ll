; ModuleID = './test/sh/oper/lhs_with_var.sk'
source_filename = "./test/sh/oper/lhs_with_var.sk"

@x = private global i64 0
@z = private global i64 0

define i64 @.lhs_with_var() {
entry:
  store i64 1, i64* @x
  %0 = load i64, i64* @x
  %1 = add nsw i64 %0, 2
  store i64 %1, i64* @z
  ret i64 0
}
