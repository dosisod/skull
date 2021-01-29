; ModuleID = './test/sh/declare/int_octal.sk'
source_filename = "./test/sh/declare/int_octal.sk"

@x = private global i64 0

define i64 @.int_octal() {
entry:
  store i64 511, i64* @x
  ret i64 0
}
