; ModuleID = './test/sh/declare/int_hex.sk'
source_filename = "./test/sh/declare/int_hex.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 255, i64* @x
  ret i64 0
}
