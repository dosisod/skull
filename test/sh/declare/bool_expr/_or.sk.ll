; ModuleID = './test/sh/declare/bool_expr/or.sk'
source_filename = "./test/sh/declare/bool_expr/or.sk"

@x = private global i1 false

define i64 @.or() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
