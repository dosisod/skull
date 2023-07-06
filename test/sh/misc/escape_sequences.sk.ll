; ModuleID = './test/sh/misc/escape_sequences.sk'
source_filename = "./test/sh/misc/escape_sequences.sk"

@0 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@x = private global ptr @0
@y = private global i32 10

define i64 @.escape_sequences() {
entry:
  ret i64 0
}
