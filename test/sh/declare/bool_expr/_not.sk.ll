; ModuleID = './test/sh/declare/bool_expr/not.sk'
source_filename = "./test/sh/declare/bool_expr/not.sk"

@x = private global i1 false

define i64 @.not() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
