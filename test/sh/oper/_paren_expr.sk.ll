; ModuleID = './test/sh/oper/paren_expr.sk'
source_filename = "./test/sh/oper/paren_expr.sk"

@x = private global i64 0
@y = private global i64 0

define i64 @.paren_expr() {
entry:
  store i64 3, i64* @x
  store i64 3, i64* @y
  ret i64 0
}
