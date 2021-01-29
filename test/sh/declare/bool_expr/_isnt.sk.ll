; ModuleID = './test/sh/declare/bool_expr/isnt.sk'
source_filename = "./test/sh/declare/bool_expr/isnt.sk"

@x = private global i1 false

define i64 @.isnt() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
