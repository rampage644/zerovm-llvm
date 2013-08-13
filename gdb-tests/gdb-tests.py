import os
import unittest
import subprocess
import re

CWD = os.path.dirname(os.path.realpath(__file__))


class GDB:
    def __init__(self):
        self.gdb = '/usr/local/bin/gdb'
        self.nexe = '%s/gdb-tests.nexe' % CWD
        self.manifest = '%s/gdb-tests.manifest' % CWD
        self.args = ['-q', '--return-child-result', '--args', '/opt/zerovm/bin/zerovm', '-sPQt', self.manifest]

    def start(self, input):
        return self._do_start(input)

    def _do_start(self, input):
        raise "Not implemented"


class BatchGDB(GDB):
    def _do_start(self, input):
        COMMAND_FILE = "cmd.txt"
        with open(COMMAND_FILE, 'w') as fcmd:
            fcmd.write(self.input)
            ret = subprocess.Popen([self.gdb, '--batch', '-x', COMMAND_FILE] + self.args,
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
            out, err = ret.communicate()
            return out


class InteractiveGDB(GDB):
    def _do_start(self, input):
        ret = subprocess.Popen([self.gdb] + self.args,
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)

        # could  write whole input without splitting in lines
        for line in input.splitlines():
            ret.stdin.write("%s\n" % line)

        out, err = ret.communicate()
        # remove first and last junk lines
        if err:
            print "stderr output: '%s'" % err
        return out


class GDBTestCase(unittest.TestCase):
    def setUp(self):
        self.gdb = InteractiveGDB()
        self.prologue = """
set confirm off
b CreateSession
r
add-symbol-file %s 0x440a00020000
b main.cpp:100
c
""" % self.gdb.nexe

    def remove_junk(self, out):
        return out.splitlines()[18:-5]

    def testIntInStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.a\np t.b\nc\n""")
        self.assertEqual("(gdb) $1 = 2", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 3", self.remove_junk(out)[1])

    def testDoubleInStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.c\np t.d\nc\n""")
        self.assertEqual("(gdb) $1 = 4.5", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 7.80000019", self.remove_junk(out)[1])

    def testTextInStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.text\nc\n""")
        self.assertEqual(True, "Hello, GDB!" in self.remove_junk(out)[0],
                         msg="'Hello, GDB!' is not present in %s" % self.remove_junk(out)[0])

    def testStructInStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.e.a\np t.e.b\np t.e.c\nc\n""")
        self.assertEqual("(gdb) $1 = 20", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 22.300000000000001", self.remove_junk(out)[1])
        self.assertEqual("(gdb) $3 = false", self.remove_junk(out)[2])

    def testArrayInStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.e.e\nc\n""")
        self.assertEqual("(gdb) $1 = {50, 52, 54, 56, 58, 60, 62, 64, 66, 68}", self.remove_junk(out)[0])

    def testWholeStructPrinting(self):
        out = self.gdb.start(self.prologue + """p t.e\nc\n""")
        self.assertEqual("(gdb) $1 = {a = 20, b = 22.300000000000001, c = false, e = {50, 52, 54, 56, 58, 60, ",
                         self.remove_junk(out)[0])
        self.assertEqual("    62, 64, 66, 68}}", self.remove_junk(out)[1])

    def testMethodInvocationPrinting(self):
        out = self.gdb.start(self.prologue + """p t.printMe(1)\nc\n""")
        self.assertIn("void ATest::printMe(int)", self.remove_junk(out)[0],
                      msg=out)

    # ---------------------------------------------------------------------------------------------------------------------
    def testIntInStructExamining(self):
        out = self.gdb.start(self.prologue + """x/wd &t.a\nx/wd &t.b\nc\n""")
        # discard address, split by ':', strip tabulation
        self.assertEqual("2", self.remove_junk(out)[0].split(':')[1].strip())
        self.assertEqual("3", self.remove_junk(out)[1].split(':')[1].strip())

    def testDoubleInStructExamining(self):
        out = self.gdb.start(self.prologue + """x/8bx &t.c\nx/8bx &t.d\nc\n""")
        self.assertEqual("0x00	0x00	0x00	0x00	0x00	0x00	0x12	0x40",
                         '\t'.join(self.remove_junk(out)[0].split(':')[1].split()))
        self.assertEqual("0x9a	0x99	0xf9	0x40	0x48	0x65	0x6c	0x6c",
                         '\t'.join(self.remove_junk(out)[1].split(':')[1].split()))

    def testTextInStructExamining(self):
        out = self.gdb.start(self.prologue + """x/s t.text\nc\n""")
        self.assertEqual("\"Hello, GDB!\\n\"", self.remove_junk(out)[0].split(':')[1].strip())

    def testStructInStructExamining(self):
        out = self.gdb.start(self.prologue + """x/wd &t.e.a\nx/8bx &t.e.b\nx/wd &t.e.c\nc\n""")
        self.assertEqual("20", self.remove_junk(out)[0].split(':')[1].strip())
        self.assertEqual("0", self.remove_junk(out)[2].split(':')[1].strip())
        self.assertEqual("0xcd	0xcc	0xcc	0xcc	0xcc	0x4c	0x36	0x40",
                         '\t'.join(self.remove_junk(out)[1].split(':')[1].split()))

    def testArrayInStructExamining(self):
        # examine 10 words (4 byte) in memory in decimal format
        out = self.gdb.start(self.prologue + """x/10wd t.e.e\nc\n""")
        # discard address, split numbers by tabulation sym, create list
        numbers = ""
        for line in self.remove_junk(out):
            numbers = numbers + ' '.join(line.split(':')[1].split('\t'))
        self.assertEqual("50 52 54 56 58 60 62 64 66 68", numbers.strip())

    # -----------------------------------------------------------------------------------------------------------------
    def testIntInStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->a\np ptr->b\nc\n""")
        self.assertEqual("(gdb) $1 = 2", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 3", self.remove_junk(out)[1])

    def testDoubleInStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->c\np ptr->d\nc\n""")
        self.assertEqual("(gdb) $1 = 4.5", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 7.80000019", self.remove_junk(out)[1])

    def testTextInStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->text\nc\n""")
        self.assertEqual(True, "Hello, GDB!" in self.remove_junk(out)[0],
                         msg="'Hello, GDB!' is not present in %s" % self.remove_junk(out)[0])

    def testStructInStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->e.a\np ptr->e.b\np ptr->e.c\nc\n""")
        self.assertEqual("(gdb) $1 = 20", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 22.300000000000001", self.remove_junk(out)[1])
        self.assertEqual("(gdb) $3 = false", self.remove_junk(out)[2])

    def testArrayInStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->e.e\nc\n""")
        self.assertEqual("(gdb) $1 = {50, 52, 54, 56, 58, 60, 62, 64, 66, 68}", self.remove_junk(out)[0])

    def testWholeStructPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->e\nc\n""")
        self.assertEqual("(gdb) $1 = {a = 20, b = 22.300000000000001, c = false, e = {50, 52, 54, 56, 58, 60, ",
                         self.remove_junk(out)[0])
        self.assertEqual("    62, 64, 66, 68}}", self.remove_junk(out)[1])

    def testMethodInvocationPrintingHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->printMe(1)\nc\n""")
        self.assertIn("void ATest::printMe(int)", self.remove_junk(out)[0],
                      msg=out)

    #------------------------------------------------------------------------------------------------------------------
    def testIntInStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.a\np g.b\nc\n""")
        self.assertEqual("(gdb) $1 = 2", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 3", self.remove_junk(out)[1])

    def testDoubleInStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.c\np g.d\nc\n""")
        self.assertEqual("(gdb) $1 = 4.5", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 7.80000019", self.remove_junk(out)[1])

    def testTextInStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.text\nc\n""")
        self.assertEqual(True, "Hello, GDB!" in self.remove_junk(out)[0],
                         msg="'Hello, GDB!' is not present in %s" % self.remove_junk(out)[0])

    def testStructInStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.e.a\np g.e.b\np g.e.c\nc\n""")
        self.assertEqual("(gdb) $1 = 20", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 22.300000000000001", self.remove_junk(out)[1])
        self.assertEqual("(gdb) $3 = false", self.remove_junk(out)[2])

    def testArrayInStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.e.e\nc\n""")
        self.assertEqual("(gdb) $1 = {50, 52, 54, 56, 58, 60, 62, 64, 66, 68}", self.remove_junk(out)[0])

    def testWholeStructPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.e\nc\n""")
        self.assertEqual("(gdb) $1 = {a = 20, b = 22.300000000000001, c = false, e = {50, 52, 54, 56, 58, 60, ",
                         self.remove_junk(out)[0])
        self.assertEqual("    62, 64, 66, 68}}", self.remove_junk(out)[1])

    def testMethodInvocationPrintingGlobal(self):
        out = self.gdb.start(self.prologue + """p g.printMe(1)\nc\n""")
        self.assertIn("void ATest::printMe(int)", self.remove_junk(out)[0],
                      msg=out)

    # -----------------------------------------------------------------------------------------------------------------
    def testVirtualFunctionInvocationGlobal(self):
        out = self.gdb.start(self.prologue + """p g.value()\np g2.value()\nc\n""")
        self.assertEqual("(gdb) $1 = 1", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 2", self.remove_junk(out)[1])

    def testVirtualFunctionInvocation(self):
        out = self.gdb.start(self.prologue + """p t.value()\np t2.value()\nc\n""")
        self.assertEqual("(gdb) $1 = 1", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 2", self.remove_junk(out)[1])

    def testVirtualFunctionInvocationHeap(self):
        out = self.gdb.start(self.prologue + """p ptr->value()\np ptr2->value()\nc\n""")
        self.assertEqual("(gdb) $1 = 1", self.remove_junk(out)[0])
        self.assertEqual("(gdb) $2 = 2", self.remove_junk(out)[1])

    def testGlobalFunctionInvocation(self):
        out = self.gdb.start(self.prologue + """p value(1)\np print()\nc\n""")
        self.assertEqual("(gdb) $1 = 11", self.remove_junk(out)[0])
        self.assertIn("Hello, world!", self.remove_junk(out)[1])
    # -----------------------------------------------------------------------------------------------------------------
class GDBTrustedTestCase(unittest.TestCase):
    def setUp(self):
        self.gdb = InteractiveGDB()
        self.prologue = """
set confirm off
b CreateSession
r
add-symbol-file %s 0x440a00020000
b main.cpp:100
c
b trap.c:207
""" % self.gdb.nexe

    def remove_junk(self, out):
        return out.splitlines()[25:-5]

    def testHitBreakpointInTrusted(self):
        out = self.gdb.start(self.prologue + """c\nbt\nc\n""")
        frame1 = "".join(self.remove_junk(out)[:3])
        frame2 = "".join(self.remove_junk(out)[3:5])
        # assert we've hit br
        self.assertIn('trap.c:207', frame1)
        # assert previous frame addr is out of sandboxed region
        self.assertEqual('0x0000555555563e62', frame2.split()[1])
        # assert no args aren't resolved
        self.assertNotIn('out of bounds', frame1)

    def testPrintingInTrusted(self):
        out = self.gdb.start(self.prologue + """c\np ch\np buffer\np retcode\nc\n""")
        # assert we could print smth
        self.assertEqual("(gdb) $1 = 1", self.remove_junk(out)[0])
        self.assertNotIn('out of bounds', self.remove_junk(out)[1])
        self.assertEqual("(gdb) $3 = 17", self.remove_junk(out)[2])

    def testReturnFromTrusted(self):
        out = self.gdb.start(self.prologue + """c\nb main.cpp:102\nc\np ptr->a\np t.b\n p g.c\nc""")
        self.assertEqual("(gdb) $1 = 2", self.remove_junk(out)[0])
        self.assertEqual('(gdb) $2 = 3', self.remove_junk(out)[1])
        self.assertEqual("(gdb) $3 = 4.5", self.remove_junk(out)[2])

    def testStepFromUntrustedToTrusted(self):
        # assert we could reach trusted with 's' command
        out = self.gdb.start(self.prologue + """s 24\nc\n""")
        print out
        self.assertTrue(False)

    def testStepFromTrustedToUntrusted(self):
        # assert we could reach trusted with 'si' command
        self.assertTrue(False)

    def testBackTraceInTrustedFromUntrusted(self):
        out = self.gdb.start(self.prologue + """c\nbt\nc\n""")
        regexp = re.compile(r'#\d\s+0x(\w+)')
        bt = []
        for line in self.remove_junk(out):
            match = regexp.search(line)
            if match:
                bt.append(match.groups()[0])
        # TrapHandler
        self.assertIn("0000555555563e62", bt)
        # SyscallHook
        self.assertIn("000055555555ddda", bt)
        # trampoline
        self.assertIn("0000440a0001000c", bt)
        # somewhere in .text
        self.assertIn("0000440a00020680", bt)


if __name__ == '__main__':
    unittest.main()
