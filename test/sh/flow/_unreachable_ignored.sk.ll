; ModuleID = './test/sh/flow/unreachable_ignored.sk'
source_filename = "./test/sh/flow/unreachable_ignored.sk"

define i64 @.unreachable_ignored() {
entry:
  ret i64 0
  unreachable
}
