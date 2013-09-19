#!/usr/bin/python
# -*- coding:utf-8 -*-

from __future__ import print_function

import os
import subprocess
import unittest
import tempfile


NAME = 'jit'

MANIFEST_TMLPT = """
Channel = %(path)s/debug.%(name)s.log, /dev/debug, 0, 0, 0, 0, 99999999, 99999999
Channel = /dev/null, /dev/stdin, 0, 0, 999999, 999999, 0, 0
Channel = %(path)s/stdout.%(name)s, /dev/stdout, 0, 0, 0, 0, 999999, 999999
Channel = %(path)s/stderr.%(name)s, /dev/stderr, 0, 0, 0, 0, 9999999, 99999999999
Channel = %(path)s/nvram.%(name)s.conf, /dev/nvram, 0, 1, 999999, 999999, 0, 0
Channel = %(path)s/%(name)s.nexe, /dev/self, 3, 0, 9999999999, 9999999999, 0, 0

Channel = %(path)s/mount/include.tar, /dev/include.tar, 3, 1, 99999999, 99999999, 0, 0
Channel = %(path)s/mount/include_clang.tar, /dev/include_clang.tar, 3, 1, 99999999, 99999999, 0, 0
#Channel = %(path)s/mount/src.tar, /dev/src.tar, 3, 1, 99999999, 99999999, 0, 0

Channel = %(path)s/%(file)s, /dev/%(file)s, 0, 0, 999999999, 9999999999, 0, 0

Version = 20130611
Program = %(path)s/%(name)s.nexe

Memory = 0x100000000, 0
SyscallsMax = 13333
Timeout = 3600
"""
NVRAM_TMPLT = """
[fstab]
channel=/dev/include.tar, mountpoint=/, access=ro
channel=/dev/include_clang.tar, mountpoint=/, access=ro
[env]
name=Node, value=1
[debug]
verbosity=4
[args]
args=%(name)s.nexe /dev/%(file)s %(function)s
"""

class BaseJITTest(unittest.TestCase):
	src = """
	"""

	def setUp(self):
		data = {}
		data['path'] = os.path.dirname(os.path.abspath(__file__))
		data['name'] = NAME
		data['file'] = 'temp.c'
		#data['file'] = tempfile.mkstemp(suffix='.c', prefix='tmp', dir=os.path.dirname(os.path.abspath(__file__)), text=True)[1]
		data['function'] = 'main'
		self.data = data

		with open("%(name)s.manifest" % data, 'w') as mfile:
			print(MANIFEST_TMLPT % data, file=mfile)
		with open("nvram.%(name)s.conf" % data, 'w') as nfile:
			print(NVRAM_TMPLT % data, file=nfile)
		with open(data['file'], 'w') as src_file:
			print(self.src, file=src_file)
	
	def tearDown(self):
		#pass
		os.remove(self.data['file'])

	def runF(self, function):
		self.data['function'] = function
		zerovm = subprocess.Popen(['zerovm', '-PQs', "%(name)s.manifest" % self.data], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = zerovm.communicate()
		returncode = int(stdout.splitlines()[1].split()[-1])
		stdout = ""
		stderr = ""
		with open("%(path)s/stdout.%(name)s" % self.data) as outf:
			stdout = outf.read()
		with open("%(path)s/stderr.%(name)s" % self.data) as errf:
			stderr = errf.read()
		return (returncode, stdout, stderr)

class ReturnCodeTest(BaseJITTest):
	src = """
int main()
{
	return 5;
}
"""
	def test(self):
		returncode, out, err = self.runF('main')
		self.assertEqual(returncode, 5)

class PrintfCodeTest(BaseJITTest):
	src = """
#include <stdio.h>
int main()
{
	printf("Hello, world!");
	return 0;
}
"""
	def test(self):
		returncode, out, err = self.runF('main')
		self.assertEqual(returncode, 0)
		self.assertEqual(out, "Hello, world!");
		self.assertEqual(err, "")

class MathCodeTest(BaseJITTest):
	src = """
#include <stdio.h>
#include <math.h>
int main()
{
	int a = 43;
	int b = 32 + a;
	int c = pow(2, 5);
	printf("%d %d %d", a, b, c);
	return 2*b;
}
"""
	def test(self):
		returncode, out, err = self.runF('main')
		self.assertEqual(returncode, 150)
		self.assertEqual(out, "43 75 32");
		self.assertEqual(err, "")

class MultipleFunctionsTest(BaseJITTest):
	src = """
#include <stdio.h>
int five()
{
	return 5;
}

float seven_dot_rwo()
{
	return 7.2;
}

void printMe(const char* letter)
{
	printf("Letter is %s", letter);
}
int main()
{
	int a = five();
	float b = seven_dot_rwo();
	int c = five() + seven_dot_rwo();
	printf("%d %.2f %d", a, b, c);
	printMe("awesome!");
	return 0;
}
"""
	def test(self):
		returncode, out, err = self.runF('main')
		self.assertEqual(returncode, 0)
		self.assertEqual(out, "5 7.20 12Letter is awesome!");
		self.assertEqual(err, "")

if __name__ == '__main__':
    unittest.main()