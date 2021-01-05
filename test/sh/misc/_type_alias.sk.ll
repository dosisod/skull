; ModuleID = './test/sh/misc/type_alias.sk'
source_filename = "./test/sh/misc/type_alias.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 1234, i64* @x
  ret i64 0
}

define private i64 @f(i64 %0) {
entry:
  ret i64 %0
}
