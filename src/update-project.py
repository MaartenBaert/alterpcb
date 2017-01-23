#!/usr/bin/env python3

import os

project_file = "alterpcb.pro"
marker = "########## Warning: Everything below this line is auto-generated and will be overwritten! ##########"

source_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(source_dir)

files_headers = []
files_sources = []

for (dirpath, dirnames, filenames) in os.walk("."):
	dirnames.sort()
	filenames.sort()
	for fn in filenames:
		ext = os.path.splitext(fn)[1]
		if ext in (".h", ".hpp"):
			files_headers.append(os.path.join(dirpath, fn)[2:])
		if ext in (".c", ".cpp"):
			files_sources.append(os.path.join(dirpath, fn)[2:])

with open(project_file, "r") as f:
	text = f.read()

(before, _, after) = text.partition(marker)
text = before + marker
text += "\n\nHEADERS += \\\n\t" + " \\\n\t".join(files_headers)
text += "\n\nSOURCES += \\\n\t" + " \\\n\t".join(files_sources)
text += "\n"

with open(project_file, "w") as f:
	f.write(text)

