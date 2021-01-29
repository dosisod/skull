; ModuleID = './test/sh/declare/variable_auto.sk'
source_filename = "./test/sh/declare/variable_auto.sk"

@x = private global i64 0
@z = private global i64 0

define i64 @.variable_auto() {
entry:
  store i64 0, i64* @x
  %0 = load i64, i64* @x
  store i64 %0, i64* @z
  %1 = load i64, i64* @z
  ret i64 %1
}
