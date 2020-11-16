; ModuleID = './test/sh/oper/div_int_consts.sk'
source_filename = "./test/sh/oper/div_int_consts.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 2, i64* %x
  ret i64 0
}
