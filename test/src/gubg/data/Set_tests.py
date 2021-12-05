from gubg.data import Set
import os, math
import numpy as np
from pytest import approx

def localpath(name):
	here_dir = os.path.dirname(__file__)
	return f"{here_dir}/{name}"

def test_read_save():
	with open(localpath("data.tsv"), "w") as fo:
		print("input\ttarget", file=fo)
		print("0\t0", file=fo)
		print("1\t1", file=fo)
		print("2\t4", file=fo)
		print("3\t9", file=fo)

	s = Set.read_csv(localpath("data.tsv"), "\t", True)
	print(s)
	s.save_csv(localpath("data.psv"), "|")

	s2 = Set.read_csv(localpath("data.psv"), "|", True)

	assert(s.item_count() == s2.item_count())

def test_create():
	s = Set.create(["input", "output"], ((x,math.sin(x)) for x in np.arange(-5.0, 5.01, 0.1)))
	assert(s.item_count() == 101)
	s.save_csv(localpath("sine.tsv"), "\t")

def test_iteration():
	s = Set.create(["input", "output"], ((x,math.sin(x)) for x in np.arange(-5.0, 5.01, 0.1)))
	mysum = sum(row[1] for row in s.items)
	assert(mysum == approx(0.0))