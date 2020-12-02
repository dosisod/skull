; ModuleID = './test/sh/misc/const_inline.sk'
source_filename = "./test/sh/misc/const_inline.sk"

@y = private global i64 0
@z = private global i64 0

define i64 @main() {
entry:
  %0 = load i64, i64 1
  %1 = add i64 %0, 1
  store i64 %1, i64* @y
  %2 = load i64, i64 1
  %3 = load i64, i64* @y
  %4 = add i64 %3, %2
  store i64 %4, i64* @z
  %5 = load i64, i64* @z
  ret i64 %5
}
