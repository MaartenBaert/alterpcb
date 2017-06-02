#!/usr/bin/env python3

import os

project_file = "alterpcb.pro"
marker = "\n########## Warning: Everything below this line is auto-generated and will be overwritten! ##########\n"

source_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(source_dir)

sourcetypes = {
	".h": "headers",
	".hpp": "headers",
	".c": "sources",
	".cpp": "sources",
}
configs = {
	"tests": "tests",
	"main": "notests",
}

files = {}

for (dirpath, dirnames, filenames) in os.walk("."):
	dirnames.sort()
	filenames.sort()
	for fn in filenames:
		sourcetype = sourcetypes.get(os.path.splitext(fn)[1])
		if sourcetype is None:
			continue
		config = configs.get(dirpath.split("/")[1], "default")
		if config not in files:
			files[config] = {}
		if sourcetype not in files[config]:
			files[config][sourcetype] = []
		files[config][sourcetype].append(os.path.join(dirpath, fn)[2:])

with open(project_file, "r") as f:
	text = f.read()

def writefiles(tabs, config):
	text = ""
	if config in files:
		if "headers" in files[config]:
			text += "\n" + tabs + "HEADERS += \\\n\t" + tabs + (" \\\n\t" + tabs).join(files[config]["headers"]) + "\n"
		if "sources" in files[config]:
			text += "\n" + tabs + "SOURCES += \\\n\t" + tabs + (" \\\n\t" + tabs).join(files[config]["sources"]) + "\n"
	return text

(before, _, after) = text.partition(marker)
text = before + marker
text += "\ntests {\n"
text += writefiles("\t", "tests")
text += "\n} else {\n"
text += writefiles("\t", "notests")
text += "\n}\n"
text += writefiles("", "default")

with open(project_file, "w") as f:
	f.write(text)

