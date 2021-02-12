; ModuleID = './test/sh/oper/lshift.sk'
source_filename = "./test/sh/oper/lshift.sk"

@x = private global i64 0

define i64 @.lshift() {
entry:
  store i64 4, i64* @x
  ret i64 0
}
