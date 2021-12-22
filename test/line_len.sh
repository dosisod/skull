#!/bin/sh

MAX_LEN=80
pass=/bin/true

echo "Files with lines over $MAX_LEN columns:"
echo

cmd='
gsub("\t", "    ") {}
length > '$MAX_LEN' {
	print FILENAME ":" NR
	exit_code=1
}
END { exit exit_code }'

for file in $(find skull -type f) ; do
	awk "$cmd" "$file" || pass=/bin/false
done

echo
$pass || exit 1

echo "None!"
