#!/bin/bash

#ref: http://www.tex.ac.uk/cgi-bin/texfaq2html?label=usebibtex

pdflatex --shell-escape cal2013.tex

bibtex cal2013

pdflatex --shell-escape cal2013.tex

pdflatex --shell-escape cal2013.tex