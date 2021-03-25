; ModuleID = './test/sh/declare/int_underscore.sk'
source_filename = "./test/sh/declare/int_underscore.sk"

@d = private global i64 0
@x = private global i64 0
@b = private global i64 0
@o = private global i64 0

define i64 @.int_underscore() {
entry:
  store i64 123456789, i64* @d
  store i64 11259375, i64* @x
  store i64 172, i64* @b
  store i64 262143, i64* @o
  ret i64 0
}
