; ModuleID = './test/skull/codegen/llvm/ptr_types.sk'
source_filename = "./test/skull/codegen/llvm/ptr_types.sk"

@bool = private global i1 true, !dbg !0
@int = private global i64 1234, !dbg !6
@float = private global double 3.140000e+00, !dbg !9
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), !dbg !12
@rune = private global i32 120, !dbg !16
@bool_ptr = private global i1* @bool, !dbg !19
@int_ptr = private global i64* @int, !dbg !23
@float_ptr = private global double* @float, !dbg !26
@str_ptr = private global i8** @str, !dbg !29
@rune_ptr = private global i32* @rune, !dbg !32
@double_ptr = private global i64** @int_ptr, !dbg !35

define i64 @.ptr_types() !dbg !40 {
entry:
  ret i64 0, !dbg !42
}

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!38, !39}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "bool", scope: !2, file: !3, line: 1, type: !22, isLocal: true, isDefinition: true, align: 8)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "skull", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, splitDebugInlining: false)
!3 = !DIFile(filename: "./test/skull/codegen/llvm/ptr_types.sk", directory: ".")
!4 = !{}
!5 = !{!0, !6, !9, !12, !16, !19, !23, !26, !29, !32, !35}
!6 = !DIGlobalVariableExpression(var: !7, expr: !DIExpression())
!7 = distinct !DIGlobalVariable(name: "int", scope: !2, file: !3, line: 2, type: !8, isLocal: true, isDefinition: true, align: 8)
!8 = !DIBasicType(name: "Int", size: 64, encoding: DW_ATE_signed)
!9 = !DIGlobalVariableExpression(var: !10, expr: !DIExpression())
!10 = distinct !DIGlobalVariable(name: "float", scope: !2, file: !3, line: 3, type: !11, isLocal: true, isDefinition: true, align: 8)
!11 = !DIBasicType(name: "Float", size: 64, encoding: DW_ATE_float)
!12 = !DIGlobalVariableExpression(var: !13, expr: !DIExpression())
!13 = distinct !DIGlobalVariable(name: "str", scope: !2, file: !3, line: 4, type: !14, isLocal: true, isDefinition: true, align: 8)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "Str", baseType: !15, size: 64, dwarfAddressSpace: 0)
!15 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!16 = !DIGlobalVariableExpression(var: !17, expr: !DIExpression())
!17 = distinct !DIGlobalVariable(name: "rune", scope: !2, file: !3, line: 5, type: !18, isLocal: true, isDefinition: true, align: 8)
!18 = !DIBasicType(name: "Rune", size: 32, encoding: DW_ATE_UTF)
!19 = !DIGlobalVariableExpression(var: !20, expr: !DIExpression())
!20 = distinct !DIGlobalVariable(name: "bool_ptr", scope: !2, file: !3, line: 7, type: !21, isLocal: true, isDefinition: true, align: 8)
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Bool", baseType: !22, size: 64, dwarfAddressSpace: 0)
!22 = !DIBasicType(name: "Bool", size: 1, encoding: DW_ATE_boolean)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(name: "int_ptr", scope: !2, file: !3, line: 8, type: !25, isLocal: true, isDefinition: true, align: 8)
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Int", baseType: !8, size: 64, dwarfAddressSpace: 0)
!26 = !DIGlobalVariableExpression(var: !27, expr: !DIExpression())
!27 = distinct !DIGlobalVariable(name: "float_ptr", scope: !2, file: !3, line: 9, type: !28, isLocal: true, isDefinition: true, align: 8)
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Float", baseType: !11, size: 64, dwarfAddressSpace: 0)
!29 = !DIGlobalVariableExpression(var: !30, expr: !DIExpression())
!30 = distinct !DIGlobalVariable(name: "str_ptr", scope: !2, file: !3, line: 10, type: !31, isLocal: true, isDefinition: true, align: 8)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Str", baseType: !14, size: 64, dwarfAddressSpace: 0)
!32 = !DIGlobalVariableExpression(var: !33, expr: !DIExpression())
!33 = distinct !DIGlobalVariable(name: "rune_ptr", scope: !2, file: !3, line: 11, type: !34, isLocal: true, isDefinition: true, align: 8)
!34 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&Rune", baseType: !18, size: 64, dwarfAddressSpace: 0)
!35 = !DIGlobalVariableExpression(var: !36, expr: !DIExpression())
!36 = distinct !DIGlobalVariable(name: "double_ptr", scope: !2, file: !3, line: 12, type: !37, isLocal: true, isDefinition: true, align: 8)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&&Int", baseType: !25, size: 64, dwarfAddressSpace: 0)
!38 = !{i32 4, !"Dwarf Version", i32 4}
!39 = !{i32 4, !"Debug Info Version", i32 3}
!40 = distinct !DISubprogram(name: ".ptr_types", scope: !3, file: !3, line: 1, type: !41, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!41 = !DISubroutineType(types: !4)
!42 = !DILocation(line: 1, column: 1, scope: !40)
