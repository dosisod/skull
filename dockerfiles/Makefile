all: alpine archlinux ubuntu

ubuntu: ubuntu-base
alpine: alpine-dev
archlinux: archlinux-dev
ubuntu: ubuntu-dev

IMAGES=\
	alpine \
	alpine-dev \
	archlinux \
	archlinux-dev \
	ubuntu \
	ubuntu-dev \
	ubuntu-base

$(IMAGES):
	$(SUDO) docker build -f Dockerfile.$@ .. -t skull:$@
