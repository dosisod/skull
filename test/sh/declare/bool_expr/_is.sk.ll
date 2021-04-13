; ModuleID = './test/sh/declare/bool_expr/is.sk'
source_filename = "./test/sh/declare/bool_expr/is.sk"

@x = private global i1 false
@y = private global i1 false
@z = private global i1 false
@q = private global i1 false

define i64 @.is() {
entry:
  store i1 true, i1* @x
  store i1 true, i1* @y
  store i1 true, i1* @z
  store i1 true, i1* @q
  ret i64 0
}
