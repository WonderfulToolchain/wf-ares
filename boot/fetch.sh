#!/bin/bash

fetch_http_tar () {
	if [ ! -f "$1" ] || [ ! tar tf "$1" >/dev/null 2>/dev/null ]; then
		echo "=== Downloading "$1"... ==="
		if [ -d "$2" ]; then
			rm -r "$2"
		fi
		rm -f "$1"
		if ! wget -O "$1" "$3"; then
			echo "Error!"
			exit 1
		fi
	fi
	if [ ! -d "$2" ]; then
		echo "=== Extracting "$1"... ==="
		if ! tar xf "$1"; then
			echo "Error!"
			exit 1
		fi
	fi
}

fetch_git () {
	if [ ! -d "$1" ]; then
		echo "=== Cloning "$1"... ==="
		if git clone "$2"; then
			return 0
		else
			echo "Error!"
			exit 1
		fi
	else
		echo "=== Updating "$1"... ==="
		cd "$1"
		if git pull; then
			cd ".."
			return 0
		else
			echo "Error!"
			exit 1
		fi
	fi
}

fetch_git_shallow () {
	if [ ! -d "$1" ]; then
		echo "=== Cloning "$1" (shallow)... ==="
		if git clone --depth=1 "$2"; then
			return 0
		else
			echo "Error!"
			exit 1
		fi
	else
		echo "=== Updating "$1" (shallow)... ==="
		cd "$1"
		if git pull --depth=1; then
			cd ".."
			return 0
		else
			echo "Error!"
			exit 1
		fi
	fi
}

fetch_git_shallow SameBoy https://github.com/LIJI32/SameBoy
