; ModuleID = './test/sh/declare/int_binary.sk'
source_filename = "./test/sh/declare/int_binary.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 21, i64* @x
  ret i64 0
}
