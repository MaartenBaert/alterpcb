#!/usr/bin/env python3

from pylab import *

def printlut(name, columns, format, data):
	print("constexpr %s[%d] = {\n\t" % (name, len(data)) + ",\n\t".join(", ".join(format % (a) for a in data[i:i+columns]) for i in range(0, len(data), columns)) + ",\n};")
	print("")

def generate_todecimal():
	
	# base 2: exponent bias 1023, mantissa shift 52
	# base 10: exponent bias 308, mantissa shift 17
	
	# expo10 = (expo2 - 1023) * log10(2) + 308
	#        = expo2 * log10(2) - 1023 * log10(2) + 308
	#        = expo2 * 0.3010299956639812 + 0.046314435747206062
	#        ~= (expo2 * 315652) >> 20
	# mant10 = mant2 * 2**(expo2 - 1023 - 52) / 10**(expo10 - 308 - 17)
	#        = mant2 * (2**expo2 * 10**(308 + 17)) / (10**expo10 * 2**(1023 + 52))
	
	expo2 = arange(2047)
	expo10 = (expo2 * 315652) >> 20
	
	expo10r = arange(max(expo10) + 1)
	expo2r = searchsorted(expo10, expo10r, side="right")
	
	extrascale = 8
	
	# this part needs more than 64 bits
	scalefac = extrascale * 2**(64 - 12)
	num = [2**int(expo2r[i]) * 10**(308 + 17) * scalefac for i in range(len(expo10r))]
	den = [10**int(expo10r[i]) * 2**(1023 + 52)  for i in range(len(expo10r))]
	mult = [(num[i] + den[i] // 2) // den[i] for i in range(len(expo10r))]
	
	x = arange(18, dtype=uint64)
	div = extrascale * 10**(17 - x)
	
	printlut("uint32_t LUT_TODECIMAL_EXPO", 16, "0x%03x", expo2r)
	printlut("uint64_t LUT_TODECIMAL_MULT", 4, "UINT64_C(0x%016x)", mult)
	printlut("uint64_t LUT_TODECIMAL_DIV", 4, "UINT64_C(0x%016x)", div)
	
	print((min(expo2r), max(expo2r)))
	print((min(mult) / 2**64, max(mult) / 2**64))
	assert(max(mult) <= 2**64 - 1) # required to avoid overflow

def generate_fromdecimal():
	
	# base 2: exponent bias 1023, mantissa shift 63
	# base 10: exponent bias 308, mantissa shift 18
	
	# expo2 = (expo10 - 308) * log2(10) + 1023 + log2(2**64 / 10**18) - log2(2**53 / 2**52)
	#       = (expo10 - 308 - 18) * log2(10) + 1023 + 63
	# mant2 = mant10 * 10**(expo10 - 308 - 18) / 2**(expo2 - 1023 - 63)
	#        = mant2 * (10**expo10 * 2**(1023 + 63)) / (2**expo2 * 10**(308 + 18))
	
	expo10 = arange(308 * 2 + 1)
	expo2 = maximum(ceil((expo10 - 308 - 18) * log2(10) + 1023 + 63 + 1e-10).astype(int), 5)
	
	# this part needs more than 64 bits
	scalefac = 2**64
	num = [10**int(expo10[i]) * 2**(1023 + 63) * scalefac for i in range(len(expo10))]
	den = [2**int(expo2[i]) * 10**(308 + 18) for i in range(len(expo10))]
	mult = [(num[i] + den[i] // 2) // den[i] for i in range(len(expo10))]
	
	printlut("uint32_t LUT_FROMDECIMAL_EXPO", 16, "0x%03x", expo2)
	printlut("uint64_t LUT_FROMDECIMAL_MULT", 4, "UINT64_C(0x%016x)", mult)
	
	print((min(expo2), max(expo2)))
	print((min(mult) / 2**64, max(mult) / 2**64))
	assert(max(mult) <= 2**64 - 1 - 2**10) # required to avoid overflow in mantissa rounding during normalization

def generate_pow10():
	
	x = arange(20, dtype=uint64)
	y = 10**x
	
	printlut("uint64_t LUT_POW10", 4, "UINT64_C(0x%016x)", y)

generate_todecimal()
generate_fromdecimal()
generate_pow10()
