#!/bin/sh

MAX_LEN=80
pass=/bin/true

check_len() {
	len=$(awk '{print length}' "$1" | sort -nr | head -n 1)

	over=$(expr "$len" \> $MAX_LEN)

	[ "$over" = "1" ] && {
		echo "$1"
		pass=/bin/false
	}
}

echo "Files with lines over $MAX_LEN columns:"
echo

for file in $(find skull -type f) ; do
	check_len "$file"
done

echo
$pass || exit 1

echo "None!"