; ModuleID = './test/sh/declare/int_negative.sk'
source_filename = "./test/sh/declare/int_negative.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 -1234, i64* @x
  ret i64 0
}
