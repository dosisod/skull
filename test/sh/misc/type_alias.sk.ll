; ModuleID = './test/sh/misc/type_alias.sk'
source_filename = "./test/sh/misc/type_alias.sk"

@x = private global i64 1234

define i64 @.type_alias() {
entry:
  ret i64 0
}

define private i64 @f(i64 %0) {
entry:
  ret i64 %0
}
