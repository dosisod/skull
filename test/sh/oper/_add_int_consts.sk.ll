; ModuleID = './test/sh/oper/add_int_consts.sk'
source_filename = "./test/sh/oper/add_int_consts.sk"

@x = private global i64 0

define i64 @.add_int_consts() {
entry:
  store i64 3, i64* @x
  ret i64 0
}
