; ModuleID = './test/sh/declare/bool_expr/less_than_eq.sk'
source_filename = "./test/sh/declare/bool_expr/less_than_eq.sk"

@x = private global i1 false
@y = private global i1 false

define i64 @.less_than_eq() {
entry:
  store i1 true, i1* @x
  store i1 true, i1* @y
  ret i64 0
}
