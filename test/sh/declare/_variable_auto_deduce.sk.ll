; ModuleID = './test/sh/declare/variable_auto_deduce.sk'
source_filename = "./test/sh/declare/variable_auto_deduce.sk"

@x = private global i64 0
@z = private global i64 0

define i64 @.variable_auto_deduce() {
entry:
  store i64 0, i64* @x
  %0 = load i64, i64* @x
  store i64 %0, i64* @z
  %1 = load i64, i64* @z
  ret i64 %1
}
