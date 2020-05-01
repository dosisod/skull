from pathlib import Path
import re


def display_markdown(code):
	re_comment=re.compile(r"(\/\*[\s\S]*?\*\/)")

	ret=""

	for comment in re_comment.finditer(code):
		start=comment.start()
		end=comment.end()

		ret+="#### `" + code[end:].split("\n")[1][:-2] + "`\n"
		ret+=code[start+3:end-3] + "\n\n"

	return ret

src_path=Path("./src")

sources=[p for p in src_path.rglob("*") if p.suffix in [".c", ".h"]]
parts_len=len(src_path.resolve().parts)

for source in sources:
	doc_path=Path("docs") / "/".join(source.resolve().parts[parts_len:])

	with open(source, "r") as f:
		markdown=display_markdown(f.read())

		if not markdown:
			continue

		with open(str(doc_path)[:-2]+".md", "w+") as ff:
			ff.write(f"# {source}\n\n")
			ff.write(markdown)