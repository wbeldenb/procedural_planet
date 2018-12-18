CC = cl
AS = ml
CONV1 = rem
CONV2 = rem

RM = if exist $@ del
ARADD = lib /OUT:
RANLIB = rem

C = .cpp
S = .asm
OBJ = .obj
OUT = .exe
EXE = .exe
BIN = binary
LIBPRE =
LIBEXT = .lib

OPTS = /O2 /GX /w /Zi
ASOPTS = /coff /Zi
OUTOPT = /Fe
DEFOPT = /D

# Comment do disable assembler optimizations
ASM = $(DEFOPT)ASM

COMPOPT = $(DEFOPT)BIN=$(BIN) $(ASM) /c

# Uncomment if your compiler doesn't have the truncate function
TRUNCATE = truncate$(OBJ)

LIBS =

WFTAOPTS = $(DEFOPT)WFTA

LIBOBJS = datastru$(OBJ) apfloat$(OBJ) apstruct$(OBJ) baseint$(OBJ) apfunc$(OBJ) apcplx$(OBJ) apcfunc$(OBJ) convolut$(OBJ) carrycrt$(OBJ) init$(OBJ) global$(OBJ) file$(OBJ) transpos$(OBJ) tranbloc$(OBJ) tablesit$(OBJ) tabletwt$(OBJ) tablefnt$(OBJ) scramble$(OBJ) move$(OBJ) bigint$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) tapstruc$(OBJ) tapfloat$(OBJ) tapfunc$(OBJ) apint$(OBJ) apifunc$(OBJ) aprat$(OBJ) aprfunc$(OBJ) $(TRUNCATE)
APTESTOBJS = aptest$(OBJ) datastru$(OBJ) apfloat$(OBJ) apstruct$(OBJ) baseint$(OBJ) apfunc$(OBJ) convolut$(OBJ) carrycrt$(OBJ) init$(OBJ) global$(OBJ) file$(OBJ) transpos$(OBJ) tranbloc$(OBJ) tablesit$(OBJ) tabletwt$(OBJ) tablefnt$(OBJ) scramble$(OBJ) move$(OBJ) bigint$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) $(TRUNCATE)
MODTESTOBJS = modtest$(OBJ) global$(OBJ) init$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) bigint$(OBJ)
DISKTESTOBJS = disktest$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) file$(OBJ) bigint$(OBJ)
FNTTESTOBJS = fnttest$(OBJ) fnt$(OBJ) tablefnt$(OBJ) fourstep$(OBJ) sixstep$(OBJ) tablefou$(OBJ) tablesix$(OBJ) tablesit$(OBJ) prime$(OBJ) primetab$(OBJ) modfunc$(OBJ) modfunca$(OBJ) scramble$(OBJ) transpos$(OBJ) tranbloc$(OBJ) move$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
FNTDISKOBJS = fntdisk$(OBJ) tabletwo$(OBJ) tabletwt$(OBJ) tablefnt$(OBJ) tablesit$(OBJ) prime$(OBJ) primetab$(OBJ) modfunc$(OBJ) modfunca$(OBJ) scramble$(OBJ) transpos$(OBJ) tranbloc$(OBJ) move$(OBJ) init$(OBJ) global$(OBJ) file$(OBJ) bigint$(OBJ)
NTTOBJS = ntt$(OBJ) prime$(OBJ) primetab$(OBJ) modfunc$(OBJ) modfunca$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
NTTWOBJS = nttw$(OBJ) prime$(OBJ) primetab$(OBJ) modfunc$(OBJ) modfunca$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
WFTATESTOBJS = wftatest$(OBJ) wfta$(OBJ) wfta_asm$(OBJ) sixwftat$(OBJ) wftatab1$(OBJ) wftatab2$(OBJ) wftatab3$(OBJ) wftatab4$(OBJ) wftatab5$(OBJ) wftatab6$(OBJ) wftatab7$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
WFTAVECTOBJS = wftatest$(OBJ) wftavect$(OBJ) sixwftat$(OBJ) wftatab1$(OBJ) wftatab2$(OBJ) wftatab3$(OBJ) wftatab4$(OBJ) wftatab5$(OBJ) wftatab6$(OBJ) wftatab7$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
MAKEWFTAOBJS = makewfta$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
CRTTESTOBJS = crttest$(OBJ) crt$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ) move$(OBJ)
BIGTESTOBJS = bigtest$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ)
BLOCKTESOBJS = blocktes$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) bigint$(OBJ) transpos$(OBJ) tranbloc$(OBJ) move$(OBJ)
TRANTESTOBJS = trantest$(OBJ) modfunc$(OBJ) modfunca$(OBJ) prime$(OBJ) primetab$(OBJ) init$(OBJ) global$(OBJ) transpos$(OBJ) tranbloc$(OBJ) move$(OBJ) bigint$(OBJ)

all: lib aptest$(EXE) modtest$(EXE) disktest$(EXE) fnttest$(EXE) fntdisk$(EXE) ntt$(EXE) crttest$(EXE) bigtest$(EXE) blocktes$(EXE) trantest$(EXE)
allwfta: all nttw$(EXE) wftatest$(EXE) wftavect$(EXE)
lib: $(LIBPRE)apfloat$(LIBEXT)

$(LIBPRE)apfloat$(LIBEXT): $(LIBOBJS)
	$(RM) $(LIBPRE)apfloat$(LIBEXT)
	$(ARADD)$(LIBPRE)apfloat$(LIBEXT) @lib.lnk
	$(RANLIB) $(LIBPRE)apfloat$(LIBEXT)

aptest$(EXE): $(APTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)aptest$(OUT) @aptest.lnk $(LIBS)
	$(CONV1) aptest
	$(CONV2) aptest

modtest$(EXE): $(MODTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)modtest$(OUT) $(MODTESTOBJS) $(LIBS)
	$(CONV1) modtest
	$(CONV2) modtest

disktest$(EXE): $(DISKTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)disktest$(OUT) @disktest.lnk $(LIBS)
	$(CONV1) disktest
	$(CONV2) disktest

fnttest$(EXE): $(FNTTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)fnttest$(OUT) @fnttest.lnk $(LIBS)
	$(CONV1) fnttest
	$(CONV2) fnttest

fntdisk$(EXE): $(FNTDISKOBJS)
	$(CC) $(OPTS) $(OUTOPT)fntdisk$(OUT) @fntdisk.lnk $(LIBS)
	$(CONV1) fntdisk
	$(CONV2) fntdisk

ntt$(EXE): $(NTTOBJS)
	$(CC) $(OPTS) $(OUTOPT)ntt$(OUT) @ntt.lnk $(LIBS)
	$(CONV1) ntt
	$(CONV2) ntt

nttw$(EXE): $(NTTWOBJS)
	$(CC) $(OPTS) $(OUTOPT)nttw$(OUT) @nttw.lnk $(LIBS)
	$(CONV1) nttw
	$(CONV2) nttw

wftatest$(EXE): $(WFTATESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)wftatest$(OUT) @wftatest.lnk $(LIBS)
	$(CONV1) wftatest
	$(CONV2) wftatest

wftavect$(EXE): $(WFTAVECTOBJS)
	$(CC) $(OPTS) $(OUTOPT)wftavect$(OUT) @wftavect.lnk $(LIBS)
	$(CONV1) wftavect
	$(CONV2) wftavect

crttest$(EXE): $(CRTTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)crttest$(OUT) $(CRTTESTOBJS) $(LIBS)
	$(CONV1) crttest
	$(CONV2) crttest

bigtest$(EXE): $(BIGTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)bigtest$(OUT) $(BIGTESTOBJS) $(LIBS)
	$(CONV1) bigtest
	$(CONV2) bigtest

blocktes$(EXE): $(BLOCKTESOBJS)
	$(CC) $(OPTS) $(OUTOPT)blocktes$(OUT) @blocktes.lnk $(LIBS)
	$(CONV1) blocktes
	$(CONV2) blocktes

trantest$(EXE): $(TRANTESTOBJS)
	$(CC) $(OPTS) $(OUTOPT)trantest$(OUT) $(TRANTESTOBJS) $(LIBS)
	$(CONV1) trantest
	$(CONV2) trantest

makewfta$(EXE): $(MAKEWFTAOBJS)
	$(CC) $(OPTS) $(OUTOPT)makewfta$(OUT) @makewfta.lnk $(LIBS)
	$(CONV1) makewfta
	$(CONV2) makewfta

makeprim$(EXE): makeprim$(C)
	$(CC) $(OPTS) $(OUTOPT)makeprim$(OUT) $(DEFOPT)CEXT=\"$(C)\" makeprim$(C) $(LIBS)
	$(CONV1) makeprim
	$(CONV2) makeprim

primetab$(C): makeprim$(EXE)
	makeprim 65537

wftatab1$(C): makewfta$(EXE)
	makewfta
wftatab2$(C): makewfta$(EXE)
	makewfta
wftatab3$(C): makewfta$(EXE)
	makewfta
wftatab4$(C): makewfta$(EXE)
	makewfta
wftatab5$(C): makewfta$(EXE)
	makewfta
wftatab6$(C): makewfta$(EXE)
	makewfta
wftatab7$(C): makewfta$(EXE)
	makewfta

primetab$(OBJ): raw.h primetab$(C)
	$(CC) $(OPTS) $(COMPOPT) primetab$(C)

wftatab1$(OBJ): ap.h raw.h modint.h wftatab1$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab1$(C)

wftatab2$(OBJ): ap.h raw.h modint.h wftatab2$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab2$(C)

wftatab3$(OBJ): ap.h raw.h modint.h wftatab3$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab3$(C)

wftatab4$(OBJ): ap.h raw.h modint.h wftatab4$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab4$(C)

wftatab5$(OBJ): ap.h raw.h modint.h wftatab5$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab5$(C)

wftatab6$(OBJ): ap.h raw.h modint.h wftatab6$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab6$(C)

wftatab7$(OBJ): ap.h raw.h modint.h wftatab7$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatab7$(C)

nttw$(OBJ): ap.h raw.h modint.h nttw$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) nttw$(C)

wftavect$(OBJ): ap.h raw.h modint.h wftavect$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftavect$(C)

fnttest$(OBJ): ap.h raw.h modint.h fnttest$(C)
	$(CC) $(OPTS) $(COMPOPT) fnttest$(C)

tablefou$(OBJ): ap.h raw.h modint.h tablefou$(C)
	$(CC) $(OPTS) $(COMPOPT) tablefou$(C)

sixwftat$(OBJ): ap.h raw.h modint.h sixwftat$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) sixwftat$(C)

makewfta$(OBJ): ap.h raw.h modint.h makewfta$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(DEFOPT)CEXT=\"$(C)\" $(COMPOPT) makewfta$(C)

ntt$(OBJ): ap.h raw.h modint.h ntt$(C)
	$(CC) $(OPTS) $(COMPOPT) ntt$(C)

wfta_asm$(OBJ): ap.h raw.h modint.h wfta_asm$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wfta_asm$(C)

disktest$(OBJ): ap.h raw.h modint.h disktest$(C)
	$(CC) $(OPTS) $(COMPOPT) disktest$(C)

crt$(OBJ): ap.h raw.h modint.h bigint.h crt$(C)
	$(CC) $(OPTS) $(COMPOPT) crt$(C)

tablesix$(OBJ): ap.h raw.h modint.h tablesix$(C)
	$(CC) $(OPTS) $(COMPOPT) tablesix$(C)

transpos$(OBJ): ap.h raw.h modint.h transpos$(C)
	$(CC) $(OPTS) $(COMPOPT) transpos$(C)

tranbloc$(OBJ): ap.h raw.h modint.h tranbloc$(S)
	$(AS) $(ASOPTS) $(COMPOPT) tranbloc$(S)

init$(OBJ): ap.h raw.h modint.h init$(C)
	$(CC) $(OPTS) $(COMPOPT) init$(C)

wfta$(OBJ): ap.h raw.h modint.h wfta$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wfta$(C)

fnt$(OBJ): ap.h raw.h modint.h fnt$(C)
	$(CC) $(OPTS) $(COMPOPT) fnt$(C)

tabletwt$(OBJ): ap.h raw.h modint.h tabletwt$(C)
	$(CC) $(OPTS) $(COMPOPT) tabletwt$(C)

wftatest$(OBJ): ap.h raw.h modint.h wftatest$(C)
	$(CC) $(OPTS) $(WFTAOPTS) $(COMPOPT) wftatest$(C)

tabletwo$(OBJ): ap.h raw.h modint.h tabletwo$(C)
	$(CC) $(OPTS) $(COMPOPT) tabletwo$(C)

file$(OBJ): ap.h raw.h modint.h file$(C)
	$(CC) $(OPTS) $(COMPOPT) file$(C)

global$(OBJ): ap.h raw.h modint.h global$(C)
	$(CC) $(OPTS) $(COMPOPT) global$(C)

fntdisk$(OBJ): ap.h raw.h modint.h fntdisk$(C)
	$(CC) $(OPTS) $(COMPOPT) fntdisk$(C)

modtest$(OBJ): ap.h raw.h modint.h modtest$(C)
	$(CC) $(OPTS) $(COMPOPT) modtest$(C)

bigtest$(OBJ): ap.h raw.h modint.h bigint.h bigtest$(C)
	$(CC) $(OPTS) $(COMPOPT) bigtest$(C)

tablefnt$(OBJ): ap.h raw.h modint.h tablefnt$(C)
	$(CC) $(OPTS) $(COMPOPT) tablefnt$(C)

move$(OBJ): ap.h raw.h modint.h move$(C)
	$(CC) $(OPTS) $(COMPOPT) move$(C)

blocktes$(OBJ): ap.h raw.h modint.h blocktes$(C)
	$(CC) $(OPTS) $(COMPOPT) blocktes$(C)

tablesit$(OBJ): ap.h raw.h modint.h tablesit$(C)
	$(CC) $(OPTS) $(COMPOPT) tablesit$(C)

bigint$(OBJ): ap.h raw.h modint.h bigint.h bigint$(S)
	$(AS) $(ASOPTS) $(COMPOPT) bigint$(S)

baseint$(OBJ): ap.h raw.h modint.h bigint.h baseint.h baseint$(S)
	$(AS) $(ASOPTS) $(COMPOPT) baseint$(S)

sixstep$(OBJ): ap.h raw.h modint.h sixstep$(C)
	$(CC) $(OPTS) $(COMPOPT) sixstep$(C)

fourstep$(OBJ): ap.h raw.h modint.h fourstep$(C)
	$(CC) $(OPTS) $(COMPOPT) fourstep$(C)

trantest$(OBJ): ap.h raw.h modint.h trantest$(C)
	$(CC) $(OPTS) $(COMPOPT) trantest$(C)

prime$(OBJ): ap.h raw.h modint.h prime$(C)
	$(CC) $(OPTS) $(COMPOPT) prime$(C)

crttest$(OBJ): ap.h raw.h modint.h crttest$(C)
	$(CC) $(OPTS) $(COMPOPT) crttest$(C)

scramble$(OBJ): ap.h raw.h modint.h scramble$(C)
	$(CC) $(OPTS) $(COMPOPT) scramble$(C)

modfunc$(OBJ): ap.h raw.h modint.h modfunc$(C)
	$(CC) $(OPTS) $(COMPOPT) modfunc$(C)

modfunca$(OBJ): ap.h raw.h modint.h modfunca$(S)
	$(AS) $(ASOPTS) $(COMPOPT) modfunca$(S)

aptest$(OBJ): ap.h apfloat.h raw.h modint.h aptest$(C)
	$(CC) $(OPTS) $(COMPOPT) aptest$(C)

datastru$(OBJ): ap.h apfloat.h raw.h modint.h datastru$(C)
	$(CC) $(OPTS) $(COMPOPT) datastru$(C)

apfloat$(OBJ): ap.h apfloat.h raw.h modint.h apfloat$(C)
	$(CC) $(OPTS) $(COMPOPT) apfloat$(C)

apstruct$(OBJ): ap.h apfloat.h raw.h modint.h bigint.h baseint.h apstruct$(C)
	$(CC) $(OPTS) $(COMPOPT) apstruct$(C)

apfunc$(OBJ): ap.h apfloat.h raw.h modint.h apfunc$(C)
	$(CC) $(OPTS) $(COMPOPT) apfunc$(C)

carrycrt$(OBJ): ap.h apfloat.h raw.h modint.h carrycrt$(C)
	$(CC) $(OPTS) $(COMPOPT) carrycrt$(C)

convolut$(OBJ): ap.h apfloat.h raw.h modint.h convolut$(C)
	$(CC) $(OPTS) $(COMPOPT) convolut$(C)

apcplx$(OBJ): ap.h apfloat.h apcplx.h raw.h modint.h apcplx$(C)
	$(CC) $(OPTS) $(COMPOPT) apcplx$(C)

apcfunc$(OBJ): ap.h apfloat.h apcplx.h raw.h modint.h apcfunc$(C)
	$(CC) $(OPTS) $(COMPOPT) apcfunc$(C)

tapstruc$(OBJ): ap.h apfloat.h tapfloat.h raw.h modint.h tapstruc$(C)
	$(CC) $(OPTS) $(COMPOPT) tapstruc$(C)

tapfloat$(OBJ): ap.h apfloat.h tapfloat.h raw.h modint.h tapfloat$(C)
	$(CC) $(OPTS) $(COMPOPT) tapfloat$(C)

tapfunc$(OBJ): ap.h apfloat.h tapfloat.h raw.h modint.h tapfunc$(C)
	$(CC) $(OPTS) $(COMPOPT) tapfunc$(C)

apint$(OBJ): ap.h apfloat.h apint.h raw.h modint.h apint$(C)
	$(CC) $(OPTS) $(COMPOPT) apint$(C)

apifunc$(OBJ): ap.h apfloat.h apint.h raw.h modint.h apifunc$(C)
	$(CC) $(OPTS) $(COMPOPT) apifunc$(C)

aprat$(OBJ): ap.h apfloat.h apint.h aprat.h raw.h modint.h aprat$(C)
	$(CC) $(OPTS) $(COMPOPT) aprat$(C)

aprfunc$(OBJ): ap.h apfloat.h apint.h aprat.h raw.h modint.h aprfunc$(C)
	$(CC) $(OPTS) $(COMPOPT) aprfunc$(C)

truncate$(OBJ): truncate.c
	$(CC) $(OPTS) $(COMPOPT) truncate.c
