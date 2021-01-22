; ModuleID = './test/sh/declare/bool_expr/is_not.sk'
source_filename = "./test/sh/declare/bool_expr/is_not.sk"

@x = private global i1 false

define i64 @main() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
