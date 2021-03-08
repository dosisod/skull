; ModuleID = './test/sh/declare/many_vars.sk'
source_filename = "./test/sh/declare/many_vars.sk"

@x = private global i64 0
@y = private global i64 0

define i64 @.many_vars() {
entry:
  store i64 0, i64* @x
  store i64 0, i64* @y
  ret i64 0
}
