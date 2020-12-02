; ModuleID = './test/sh/declare/many.sk'
source_filename = "./test/sh/declare/many.sk"

@x = private global i64 0
@y = private global i64 0

define i64 @main() {
entry:
  store i64 0, i64* @x
  store i64 0, i64* @y
  ret i64 0
}
