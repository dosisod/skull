; ModuleID = './test/sh/add_int_consts.sk'
source_filename = "./test/sh/add_int_consts.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  store i64 3, i64* %x
}
