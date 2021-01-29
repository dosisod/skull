; ModuleID = './test/sh/declare/bool.sk'
source_filename = "./test/sh/declare/bool.sk"

@x = private global i1 false

define i64 @.bool() {
entry:
  store i1 true, i1* @x
  ret i64 0
}
