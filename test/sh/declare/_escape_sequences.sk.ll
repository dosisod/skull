; ModuleID = './test/sh/declare/escape_sequences.sk'
source_filename = "./test/sh/declare/escape_sequences.sk"

@0 = private unnamed_addr constant [16 x i8] c"\\ \0D \0A \09 A \1B \22 '\00", align 1

define i64 @.escape_sequences() {
entry:
  ret i64 0
}
