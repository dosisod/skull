; ModuleID = './test/sh/misc/const_inline.sk'
source_filename = "./test/sh/misc/const_inline.sk"

@x = private global i64 0
@y = private global i64 0
@z = private global i64 0

define i64 @main() {
entry:
  ret i64 3
}
