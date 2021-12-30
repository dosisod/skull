; ModuleID = './test/sh/declare/many_vars.sk'
source_filename = "./test/sh/declare/many_vars.sk"

@x = private global i64 0
@y = private global i64 0

define i64 @.many_vars() {
entry:
  ret i64 0
}
