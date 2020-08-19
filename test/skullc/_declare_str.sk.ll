; ModuleID = 'main_module'
source_filename = "main_module"

define i64 @main() {
entry:
  %x = alloca [7 x i32]
  store [7 x i32] [i32 97, i32 98, i32 99, i32 100, i32 101, i32 102, i32 0], [7 x i32]* %x
}
