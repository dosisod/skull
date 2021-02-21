; ModuleID = './test/sh/oper/is_cmp_float.sk'
source_filename = "./test/sh/oper/is_cmp_float.sk"

@x = private global i1 false

define i64 @.is_cmp_float() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
