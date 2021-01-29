; ModuleID = './test/sh/declare/int.sk'
source_filename = "./test/sh/declare/int.sk"

@x = private global i64 0

define i64 @.int() {
entry:
  store i64 1234, i64* @x
  ret i64 0
}
