; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

define i64 @.first() !dbg !5 {
entry:
  ret i64 0, !dbg !7
}

define private i64 @f() !dbg !8 {
entry:
  ret i64 1, !dbg !9
}

define private void @f2(i64 %0) !dbg !10 {
entry:
  ret void
}

define private void @f3(i64 %0, i64 %1) !dbg !14 {
entry:
  ret void
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".first", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!6 = !DISubroutineType(types: !2)
!7 = !DILocation(line: 1, column: 1, scope: !5)
!8 = distinct !DISubprogram(name: "f", scope: !5, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DILocation(line: 2, column: 2, scope: !8)
!10 = distinct !DISubprogram(name: "f2", scope: !5, file: !1, line: 5, type: !11, scopeLine: 5, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !0, retainedNodes: !2)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "Int", size: 64)
!14 = distinct !DISubprogram(name: "f3", scope: !5, file: !1, line: 9, type: !15, scopeLine: 9, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !0, retainedNodes: !2)
!15 = !DISubroutineType(types: !16)
!16 = !{!13, !13}
