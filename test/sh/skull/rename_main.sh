[ "$2" = "0" ]

expected="; ModuleID = './test/sh/skull/dummy.sk'
source_filename = \"./test/sh/skull/dummy.sk\"

define i64 @alt_main() {
entry:
  ret i64 0
}"

[ "$expected" = "$1" ]
