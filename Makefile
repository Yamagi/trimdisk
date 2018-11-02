PROG=	trimdisk
SRCS=	trimdisk.c
MAN=	trimdisk.8

WARNS?=	2

BINDIR?=	/usr/local/sbin
BINMODE=	755
BINOWN=		root

MANDIR?=	/usr/local/share/man/man

.include <bsd.prog.mk>
