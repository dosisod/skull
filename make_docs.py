from pathlib import Path
import re

re_comment = re.compile(r"\/\*[\s\S]*?\*\/")

def display_markdown(code: str):
    ret = ""
    for comment in re_comment.finditer(code):
        end = comment.end()

        doc = code[comment.start() + 3:end - 3]
        if "NOLINT" in doc:
            continue

        ret += "```c\n" + code[end:].split("\n")[1][:-2] + "\n```\n\n"

        for line in doc.split("\n"):
            ret += "> " + line if line else "> \\"
            ret += "\n"

        ret += "\n"

    return ret

for source in [src for src in Path("./").rglob("*") if src.suffix in [".c", ".h"]]:
    with source.open() as f:
        markdown = display_markdown(f.read())
        if markdown:
            with open(str(Path("docs") / source)[:-2] + ".md", "w+") as ff:
                ff.write(f"# {source}\n\n{markdown}")