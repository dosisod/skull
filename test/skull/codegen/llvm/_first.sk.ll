; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

define i64 @.first() !dbg !5 {
entry:
  %0 = call i64 @f(), !dbg !7
  ret i64 0, !dbg !8
}

define private i64 @f() !dbg !9 {
entry:
  ret i64 1, !dbg !10
}

define private void @f2(i64 %0) !dbg !11 {
entry:
  ret void
}

define private void @f3(i64 %0, i64 %1) !dbg !15 {
entry:
  ret void
}

define private void @f4() !dbg !18 {
entry:
  %local_var = alloca i64
  store i64 1, i64* %local_var
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !19, metadata !DIExpression()), !dbg !20
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64
  store i64 1, i64* %if_test
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !21, metadata !DIExpression()), !dbg !23
  ret void
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

attributes #0 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".first", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!6 = !DISubroutineType(types: !2)
!7 = !DILocation(line: 5, column: 1, scope: !5)
!8 = !DILocation(line: 1, column: 1, scope: !5)
!9 = distinct !DISubprogram(name: "f", scope: !5, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DILocation(line: 2, column: 2, scope: !9)
!11 = distinct !DISubprogram(name: "f2", scope: !5, file: !1, line: 7, type: !12, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!12 = !DISubroutineType(types: !13)
!13 = !{!14}
!14 = !DIBasicType(name: "Int", size: 64)
!15 = distinct !DISubprogram(name: "f3", scope: !5, file: !1, line: 11, type: !16, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!16 = !DISubroutineType(types: !17)
!17 = !{!14, !14}
!18 = distinct !DISubprogram(name: "f4", scope: !5, file: !1, line: 15, type: !6, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!19 = !DILocalVariable(name: "local_var", scope: !18, file: !1, line: 19, type: !14, align: 8)
!20 = !DILocation(line: 19, column: 6, scope: !18)
!21 = !DILocalVariable(name: "if_test", scope: !22, file: !1, line: 22, type: !14, align: 8)
!22 = distinct !DILexicalBlock(scope: !18, file: !1, line: 21, column: 2)
!23 = !DILocation(line: 22, column: 7, scope: !22)
