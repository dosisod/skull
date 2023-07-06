; ModuleID = './test/sh/declare/variable_auto_deduce.sk'
source_filename = "./test/sh/declare/variable_auto_deduce.sk"

@x = private global i64 0
@z = private global i64 0

define i64 @.variable_auto_deduce() {
entry:
  %0 = load i64, ptr @x, align 4
  store i64 %0, ptr @z, align 4
  %1 = load i64, ptr @z, align 4
  ret i64 %1
}
