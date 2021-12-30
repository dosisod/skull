; ModuleID = './test/sh/declare/export_var.sk'
source_filename = "./test/sh/declare/export_var.sk"

@x = global i64 1

define i64 @.export_var() {
entry:
  ret i64 0
}
