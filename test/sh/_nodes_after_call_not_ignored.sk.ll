; ModuleID = './test/sh/nodes_after_call_not_ignored.sk'
source_filename = "./test/sh/nodes_after_call_not_ignored.sk"

define i64 @main() {
entry:
  call void @x()
  ret i64 1
}

declare void @x()
