; ModuleID = './test/sh/oper/sub_int_consts.sk'
source_filename = "./test/sh/oper/sub_int_consts.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 -1, i64* %x
}
