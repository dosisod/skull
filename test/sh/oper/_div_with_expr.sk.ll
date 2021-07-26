; ModuleID = './test/sh/oper/div_with_expr.sk'
source_filename = "./test/sh/oper/div_with_expr.sk"

@x = private global i64 0

define i64 @.div_with_expr() {
entry:
  %0 = call i64 @f()
  %1 = call i64 @f()
  %2 = sdiv exact i64 %0, %1
  store i64 %2, i64* @x
  ret i64 0
}

declare i64 @f()
