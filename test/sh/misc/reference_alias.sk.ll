; ModuleID = './test/sh/misc/reference_alias.sk'
source_filename = "./test/sh/misc/reference_alias.sk"

@i = private global i64 1
@ptr = private global i64* @i

define i64 @.reference_alias() {
entry:
  ret i64 0
}
