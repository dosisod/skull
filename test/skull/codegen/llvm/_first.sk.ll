; ModuleID = './test/skull/codegen/llvm/first.sk'
source_filename = "./test/skull/codegen/llvm/first.sk"

@0 = private unnamed_addr constant [12 x i8] c"some string\00", align 1

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
  call void @llvm.dbg.declare(metadata i64 %0, metadata !16, metadata !DIExpression()), !dbg !17
  ret void
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define private void @f3(i64 %0, i64 %1) !dbg !18 {
entry:
  call void @llvm.dbg.declare(metadata i64 %0, metadata !22, metadata !DIExpression()), !dbg !24
  call void @llvm.dbg.declare(metadata i64 %1, metadata !23, metadata !DIExpression()), !dbg !24
  ret void
}

define private void @f4() !dbg !25 {
entry:
  %local_var = alloca i64
  store i64 1, i64* %local_var, !dbg !40
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !27, metadata !DIExpression()), !dbg !40
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64
  store i64 1, i64* %if_test, !dbg !41
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !28, metadata !DIExpression()), !dbg !41
  %int = alloca i64
  store i64 1234, i64* %int, !dbg !42
  call void @llvm.dbg.declare(metadata i64* %int, metadata !30, metadata !DIExpression()), !dbg !42
  %float = alloca double
  store double 3.141500e+00, double* %float, !dbg !43
  call void @llvm.dbg.declare(metadata double* %float, metadata !31, metadata !DIExpression()), !dbg !43
  %rune = alloca i32
  store i32 120, i32* %rune, !dbg !44
  call void @llvm.dbg.declare(metadata i32* %rune, metadata !33, metadata !DIExpression()), !dbg !44
  %bool = alloca i1
  store i1 false, i1* %bool, !dbg !45
  call void @llvm.dbg.declare(metadata i1* %bool, metadata !35, metadata !DIExpression()), !dbg !45
  %str = alloca i8*
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %str, !dbg !46
  call void @llvm.dbg.declare(metadata i8** %str, metadata !37, metadata !DIExpression()), !dbg !46
  ret void
}

attributes #0 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false)
!1 = !DIFile(filename: "./test/skull/codegen/llvm/first.sk", directory: ".")
!2 = !{}
!3 = !{i32 4, !"Dwarf Version", i32 4}
!4 = !{i32 4, !"Debug Info Version", i32 3}
!5 = distinct !DISubprogram(name: ".first", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!6 = !DISubroutineType(types: !2)
!7 = !DILocation(line: 5, column: 1, scope: !5)
!8 = !DILocation(line: 1, column: 1, scope: !5)
!9 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 1, type: !6, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DILocation(line: 2, column: 2, scope: !9)
!11 = distinct !DISubprogram(name: "f2", scope: !1, file: !1, line: 7, type: !12, scopeLine: 7, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !15)
!12 = !DISubroutineType(types: !13)
!13 = !{!14}
!14 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_signed)
!15 = !{!16}
!16 = !DILocalVariable(name: "x", arg: 1, scope: !11, file: !1, line: 7, type: !14)
!17 = !DILocation(line: 7, column: 1, scope: !11)
!18 = distinct !DISubprogram(name: "f3", scope: !1, file: !1, line: 11, type: !19, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !21)
!19 = !DISubroutineType(types: !20)
!20 = !{!14, !14}
!21 = !{!22, !23}
!22 = !DILocalVariable(name: "y", arg: 1, scope: !18, file: !1, line: 11, type: !14)
!23 = !DILocalVariable(name: "z", arg: 2, scope: !18, file: !1, line: 11, type: !14)
!24 = !DILocation(line: 11, column: 1, scope: !18)
!25 = distinct !DISubprogram(name: "f4", scope: !1, file: !1, line: 15, type: !6, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !26)
!26 = !{!27, !28, !30, !31, !33, !35, !37}
!27 = !DILocalVariable(name: "local_var", scope: !25, file: !1, line: 19, type: !14, align: 8)
!28 = !DILocalVariable(name: "if_test", scope: !29, file: !1, line: 22, type: !14, align: 8)
!29 = distinct !DILexicalBlock(scope: !25, file: !1, line: 21, column: 2)
!30 = !DILocalVariable(name: "int", scope: !25, file: !1, line: 25, type: !14, align: 8)
!31 = !DILocalVariable(name: "float", scope: !25, file: !1, line: 26, type: !32, align: 8)
!32 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!33 = !DILocalVariable(name: "rune", scope: !25, file: !1, line: 27, type: !34, align: 8)
!34 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!35 = !DILocalVariable(name: "bool", scope: !25, file: !1, line: 28, type: !36, align: 8)
!36 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!37 = !DILocalVariable(name: "str", scope: !25, file: !1, line: 29, type: !38, align: 8)
!38 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !39, size: 64, dwarfAddressSpace: 0)
!39 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!40 = !DILocation(line: 19, column: 6, scope: !25)
!41 = !DILocation(line: 22, column: 7, scope: !29)
!42 = !DILocation(line: 25, column: 6, scope: !25)
!43 = !DILocation(line: 26, column: 6, scope: !25)
!44 = !DILocation(line: 27, column: 6, scope: !25)
!45 = !DILocation(line: 28, column: 6, scope: !25)
!46 = !DILocation(line: 29, column: 6, scope: !25)
