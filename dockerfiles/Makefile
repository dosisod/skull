all: alpine archlinux ubuntu

ubuntu: ubuntu-base

alpine archlinux ubuntu ubuntu-base:
	$(SUDO) docker build -f Dockerfile.$@ .. -t skull:$@
