; ModuleID = './test/sh/declare/bool_expr_is.sk'
source_filename = "./test/sh/declare/bool_expr_is.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i1 true, i64* @x
  ret i64 0
}