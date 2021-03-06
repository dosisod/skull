from pathlib import Path
import re

re_comment = re.compile(r"\/\*[\s\S]*?\*\/")
re_struct = re.compile(r"typedef\ struct\ .*\{(.*)\}")

def iter_stack(code: str) -> int:
    # from: https://stackoverflow.com/a/38212061
    index = 0
    stack = 0

    for i, c in enumerate(code):
        index += 1
        if c == '{':
            stack += 1

        elif c == '}':
            stack -= 1

            if stack == 0:
                return index

    return 0

def display_markdown(code: str) -> str:
    ret = ""
    for comment in re_comment.finditer(code):
        end = comment.end()

        doc = code[(comment.start() + 3):(end - 3)]
        if "NOLINT" in doc:
            continue

        value = ""
        lines = code[end:].split("\n")

        if lines[1].startswith("typedef struct"):
            struct_end = iter_stack(code[end + 1:])

            value = code[(end + 1):(end + struct_end + 1)]

        elif lines[1].startswith("typedef"):
            typedef_end = code[end:].index(";")

            value = code[(end + 1):(end + typedef_end)]

        elif lines[1].startswith("#define"):
            define_end = code[end:].index("\n\n")

            value = code[(end + 1):(end + define_end)]

        else:
            func_start = code[end:].index("{")
            value = code[
                (end + 1):(end + func_start - 1)
            ].replace('\n', '').replace('\t', ' ').replace("( ", "(")

        ret += f"```c\n{value}\n```\n\n"

        for line in doc.split("\n"):
            if not line:
                line = "\\"

            ret += f"> {line}\n"

        ret += "\n"

    return ret

units = {}

sources = [
    src for src in Path("./skull").rglob("*") if src.suffix in [".c", ".h"]
]
for source in sources:
    filename = str(source)[:-2]

    if filename in units:
        units[filename].append(source)
    else:
        units[filename] = [source]

for unit, files in units.items():
    markdown = ""
    for source in files:
        with source.open() as f:
            markdown += display_markdown(f.read())

    if markdown:
        with open(str(Path("docs") / source)[:-2] + ".md", "w+") as f:
            f.write(f"# {unit}\n\n{markdown}")
