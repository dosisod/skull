; ModuleID = './test/sh/declare/bool_expr/less_than.sk'
source_filename = "./test/sh/declare/bool_expr/less_than.sk"

@x = private global i1 false
@y = private global i1 false

define i64 @main() {
entry:
  store i1 true, i1* @x
  store i1 true, i1* @y
  ret i64 0
}
