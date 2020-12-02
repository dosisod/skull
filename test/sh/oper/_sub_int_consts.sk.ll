; ModuleID = './test/sh/oper/sub_int_consts.sk'
source_filename = "./test/sh/oper/sub_int_consts.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 -1, i64* @x
  ret i64 0
}
