; ModuleID = './test/sh/oper/mult_int_consts.sk'
source_filename = "./test/sh/oper/mult_int_consts.sk"

@x = private global i64 0

define i64 @.mult_int_consts() {
entry:
  store i64 6, i64* @x
  ret i64 0
}
