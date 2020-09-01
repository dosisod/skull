; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 1, i64* %x
  ret i64 1
}
