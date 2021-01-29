; ModuleID = './test/sh/function/nodes_after_call_not_ignored.sk'
source_filename = "./test/sh/function/nodes_after_call_not_ignored.sk"

define i64 @.nodes_after_call_not_ignored() {
entry:
  call void @x()
  ret i64 1
}

declare void @x()
