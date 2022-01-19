; ModuleID = './test/sh/oper/deref.sk'
source_filename = "./test/sh/oper/deref.sk"

@a = private global i64 123
@ptr = private global i64* @a
@b = private global i64 0

define i64 @.deref() {
entry:
  %0 = load i64*, i64** @ptr, align 8
  %1 = load i64, i64* %0, align 4
  store i64 %1, i64* @b, align 4
  ret i64 0
}
