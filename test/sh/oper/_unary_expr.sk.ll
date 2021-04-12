; ModuleID = './test/sh/oper/unary_expr.sk'
source_filename = "./test/sh/oper/unary_expr.sk"

@x = private global i1 false

define i64 @.unary_expr() {
entry:
  store i1 false, i1* @x
  ret i64 0
}
