#!/usr/bin/env python3

from pylab import *

def printlut(name, columns, format, data):
	print("constexpr %s[%d] = {\n\t" % (name, len(data)) + ",\n\t".join(", ".join(format % (a) for a in data[i:i+columns]) for i in range(0, len(data), columns)) + ",\n};")
	print("")

def generate_todecimal():
	
	expo2 = arange(2047)
	expo10 = (expo2 * 315652) >> 20
	
	expo10r = arange(max(expo10) + 1)
	expo2r = expo2[searchsorted(expo10, expo10r, side="right") - 1] + 1
	
	# this part needs more than 64 bits
	scalefac = 8
	num = [scalefac * 2**int(expo2r[i]) * 10**(308 + 17) for i in range(len(expo10r))]
	den = [2**(1023) * 10**int(expo10r[i]) for i in range(len(expo10r))]
	mult = [(num[i] + den[i] // 2) // den[i] for i in range(len(expo10r))]
	
	printlut("uint32_t LUT_TODECIMAL_EXPO", 16, "0x%03x", expo2r)
	printlut("uint64_t LUT_TODECIMAL_MULT", 4, "UINT64_C(0x%016x)", mult)
	
	x = arange(18, dtype=uint64)
	div = scalefac * 10**(17 - x)
	#lim = 10**(1 + x)
	
	printlut("uint64_t LUT_TODECIMAL_DIV", 4, "UINT64_C(0x%016x)", div)
	#printlut("uint64_t LUT_TODECIMAL_LIM", 4, "UINT64_C(0x%016x)", lim)

def generate_fromdecimal():
	
	expo10 = arange(308 * 2 + 1)
	expo2 = clip(floor((expo10 - 308) * log2(10) + 1023 + 0.206).astype(int), 1, 2046) # any approximation will work here as long as we avoid overflows
	
	# this part needs more than 64 bits
	scalefac = 2**(64 + 7)
	num = [scalefac * 2**(1023 + 52) * 10**int(expo10[i]) for i in range(len(expo10))]
	den = [2**int(expo2[i]) * 10**(308 + 18) for i in range(len(expo10))]
	mult = [(num[i] + den[i] // 2) // den[i] for i in range(len(expo10))]
	
	printlut("uint32_t LUT_FROMDECIMAL_EXPO", 16, "0x%03x", expo2)
	printlut("uint64_t LUT_FROMDECIMAL_MULT", 4, "UINT64_C(0x%016x)", mult)
	
	print((min(expo2), max(expo2)))
	print((min(mult) / 2**64, max(mult) / 2**64))

def generate_pow10():
	
	x = arange(20, dtype=uint64)
	y = 10**x
	
	printlut("uint64_t LUT_POW10", 4, "UINT64_C(0x%016x)", y)

generate_todecimal()
generate_fromdecimal()
generate_pow10()
