; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  call void @x()
}

declare void @x()
