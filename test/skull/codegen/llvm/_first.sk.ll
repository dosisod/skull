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
  %x = alloca i64
  store i64 %0, i64* %x
  call void @llvm.dbg.declare(metadata i64* %x, metadata !16, metadata !DIExpression()), !dbg !17
  %noop = alloca i64
  store i64 0, i64* %noop, !dbg !18
  ret void
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define private void @f3(i64 %0, i64 %1) !dbg !19 {
entry:
  %y = alloca i64
  store i64 %0, i64* %y
  call void @llvm.dbg.declare(metadata i64* %y, metadata !23, metadata !DIExpression()), !dbg !25
  %z = alloca i64
  store i64 %1, i64* %z
  call void @llvm.dbg.declare(metadata i64* %z, metadata !24, metadata !DIExpression()), !dbg !25
  %noop = alloca i64
  store i64 0, i64* %noop, !dbg !26
  ret void
}

define private void @f4() !dbg !27 {
entry:
  %local_var = alloca i64
  store i64 1, i64* %local_var, !dbg !42
  call void @llvm.dbg.declare(metadata i64* %local_var, metadata !29, metadata !DIExpression()), !dbg !42
  br i1 true, label %if_true, label %end

if_true:                                          ; preds = %entry
  %if_test = alloca i64
  store i64 1, i64* %if_test, !dbg !43
  br label %end

end:                                              ; preds = %entry, %if_true
  call void @llvm.dbg.declare(metadata i64* %if_test, metadata !30, metadata !DIExpression()), !dbg !43
  %int = alloca i64
  store i64 1234, i64* %int, !dbg !44
  call void @llvm.dbg.declare(metadata i64* %int, metadata !32, metadata !DIExpression()), !dbg !44
  %float = alloca double
  store double 3.141500e+00, double* %float, !dbg !45
  call void @llvm.dbg.declare(metadata double* %float, metadata !33, metadata !DIExpression()), !dbg !45
  %rune = alloca i32
  store i32 120, i32* %rune, !dbg !46
  call void @llvm.dbg.declare(metadata i32* %rune, metadata !35, metadata !DIExpression()), !dbg !46
  %bool = alloca i1
  store i1 false, i1* %bool, !dbg !47
  call void @llvm.dbg.declare(metadata i1* %bool, metadata !37, metadata !DIExpression()), !dbg !47
  %str = alloca i8*
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** %str, !dbg !48
  call void @llvm.dbg.declare(metadata i8** %str, metadata !39, metadata !DIExpression()), !dbg !48
  %0 = load i1, i1* %bool, !dbg !49
  br i1 %0, label %if_true2, label %end1

if_true2:                                         ; preds = %end
  %noop = alloca i64
  store i64 0, i64* %noop, !dbg !50
  br label %end1

end1:                                             ; preds = %end, %if_true2
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
!18 = !DILocation(line: 8, column: 2, scope: !11)
!19 = distinct !DISubprogram(name: "f3", scope: !1, file: !1, line: 11, type: !20, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !22)
!20 = !DISubroutineType(types: !21)
!21 = !{!14, !14}
!22 = !{!23, !24}
!23 = !DILocalVariable(name: "y", arg: 1, scope: !19, file: !1, line: 11, type: !14)
!24 = !DILocalVariable(name: "z", arg: 2, scope: !19, file: !1, line: 11, type: !14)
!25 = !DILocation(line: 11, column: 1, scope: !19)
!26 = !DILocation(line: 12, column: 2, scope: !19)
!27 = distinct !DISubprogram(name: "f4", scope: !1, file: !1, line: 15, type: !6, scopeLine: 15, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !28)
!28 = !{!29, !30, !32, !33, !35, !37, !39}
!29 = !DILocalVariable(name: "local_var", scope: !27, file: !1, line: 19, type: !14, align: 8)
!30 = !DILocalVariable(name: "if_test", scope: !31, file: !1, line: 22, type: !14, align: 8)
!31 = distinct !DILexicalBlock(scope: !27, file: !1, line: 21, column: 2)
!32 = !DILocalVariable(name: "int", scope: !27, file: !1, line: 25, type: !14, align: 8)
!33 = !DILocalVariable(name: "float", scope: !27, file: !1, line: 26, type: !34, align: 8)
!34 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!35 = !DILocalVariable(name: "rune", scope: !27, file: !1, line: 27, type: !36, align: 8)
!36 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!37 = !DILocalVariable(name: "bool", scope: !27, file: !1, line: 28, type: !38, align: 8)
!38 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!39 = !DILocalVariable(name: "str", scope: !27, file: !1, line: 29, type: !40, align: 8)
!40 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !41, size: 64, dwarfAddressSpace: 0)
!41 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!42 = !DILocation(line: 19, column: 6, scope: !27)
!43 = !DILocation(line: 22, column: 7, scope: !31)
!44 = !DILocation(line: 25, column: 6, scope: !27)
!45 = !DILocation(line: 26, column: 6, scope: !27)
!46 = !DILocation(line: 27, column: 6, scope: !27)
!47 = !DILocation(line: 28, column: 6, scope: !27)
!48 = !DILocation(line: 29, column: 6, scope: !27)
!49 = !DILocation(line: 31, column: 5, scope: !27)
!50 = !DILocation(line: 32, column: 3, scope: !51)
!51 = distinct !DILexicalBlock(scope: !27, file: !1, line: 31, column: 2)
