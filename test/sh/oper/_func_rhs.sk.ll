; ModuleID = './test/sh/oper/func_rhs.sk'
source_filename = "./test/sh/oper/func_rhs.sk"

@x = private global i64 0

define i64 @.func_rhs() {
entry:
  %0 = call i64 @f()
  %1 = add nsw i64 1, %0
  store i64 %1, i64* @x
  ret i64 0
}

declare i64 @f()
