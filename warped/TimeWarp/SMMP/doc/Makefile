# -*-Makefile-*-
#
# Copyright (C) 1994 Ohio Board of Regents and the University of
# Cincinnati.  All rights reserved.
#
# author: philip a. wilsey
# phil.wilsey@uc.edu
#
#
# this makefile is designed to help format the warped.texi
# documentation.  this makefile supports the construction of the
# following formats: into dvi, postscript, and html.  the following
# are relevant invocations for this makefile:
#
#    make all:  creating all formats described above.
#
#    make dvi:  creating the dvi format
#               (resulting filename "warped.dvi"). 
#
#    make ps:   creating the postscript format
#               (resulting filename "warped.ps"). 
#
#    make clean: clears all intermediate files used to create above
#                described formats.  leaves final formatted files in
#                place. 
#
#    make scrub: clears everything, leaving only the original source
#                files. 


all:	ps 

dvi:	smmp.dvi

ps:	smmp.dvi
	dvips smmp.dvi -o -r0

smmp.dvi: smmp.tex
	latex smmp.tex
	latex smmp.tex

clean:
	rm -f *.aux *.cp *.cps *.log *.pg *.toc *.tp *.vr

scrub:	clean
	rm -f *.dvi *.ps  
