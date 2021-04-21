; ModuleID = './test/sh/misc/escape_sequences.sk'
source_filename = "./test/sh/misc/escape_sequences.sk"

@0 = private unnamed_addr constant [33 x i8] c"\0D \0A \09 A \C3\BF \09 \0Dhello \F0\9F\92\80 \1B \22 ' \\\00", align 1

define i64 @.escape_sequences() {
entry:
  ret i64 0
}
