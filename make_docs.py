from pathlib import Path
import re

re_comment=re.compile(r"(\/\*[\s\S]*?\*\/)")

def display_markdown(code):
	ret=""
	for comment in re_comment.finditer(code):
		end=comment.end()

		ret+="#### `" + code[end:].split("\n")[1][:-2] + "`\n"
		ret+=code[comment.start() + 3:end - 3] + "\n\n"

	return ret

for source in [src for src in Path("./skull").rglob("*") if src.suffix in [".c", ".h"]]:
	with source.open() as f:
		markdown=display_markdown(f.read())
		if markdown:
			with open(str(Path("docs") / "/".join(source.parts[1:]))[:-2] + ".md", "w+") as ff:
				ff.write(f"# {source}\n\n{markdown}")