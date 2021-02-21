; ModuleID = './test/sh/oper/is_cmp_rune.sk'
source_filename = "./test/sh/oper/is_cmp_rune.sk"

@x = private global i1 false

define i64 @.is_cmp_rune() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
