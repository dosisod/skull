; ModuleID = './test/sh/declare/bool_expr/and.sk'
source_filename = "./test/sh/declare/bool_expr/and.sk"

@x = private global i1 false

define i64 @.and() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
