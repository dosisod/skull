; ModuleID = './test/sh/oper/paren_rhs_expr.sk'
source_filename = "./test/sh/oper/paren_rhs_expr.sk"

@x = private global i64 0

define i64 @.paren_rhs_expr() {
entry:
  store i64 6, i64* @x
  ret i64 0
}
