from pathlib import Path
import re

re_comment = re.compile(r"\/\*[\s\S]*?\*\/")

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
            struct_end = code[end:].index("}")

            value = code[(end + 1):(end + struct_end + 1)]

        else:
            value = lines[1][:-2]

        ret += f"```c\n{value}\n```\n\n"

        for line in doc.split("\n"):
            if not line:
                line = "\\"

            ret += f"> {line}\n"

        ret += "\n"

    return ret

units = {}

for source in [src for src in Path("./").rglob("*") if src.suffix in [".c", ".h"]]:
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