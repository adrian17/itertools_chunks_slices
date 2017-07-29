import unittest
import sys

from itertools_chunks_slices import chunks, slices

class DestructionTester:
    alive = 0
    def __init__(self):
        DestructionTester.alive += 1
    def __del__(self):
        DestructionTester.alive -= 1

class TestChunks(unittest.TestCase):
    def test_simple(self):
        lst = [1, 2, 3, 4, 5, 6, 7, 8]
        self.assertEqual(
            list(chunks(lst, 1)),
            [(1,), (2,), (3,), (4,), (5,), (6,), (7,), (8,)]
        )
        self.assertEqual(list(chunks(lst, 2)), [(1, 2), (3, 4), (5, 6), (7, 8)])
        self.assertEqual(list(chunks(lst, 3)), [(1, 2, 3), (4, 5, 6)])
        self.assertEqual(list(chunks(lst, 4)), [(1, 2, 3, 4), (5, 6, 7, 8)])
        self.assertEqual(list(chunks(lst, 5)), [(1, 2, 3, 4, 5)])
        self.assertEqual(list(chunks(lst, 6)), [(1, 2, 3, 4, 5, 6)])
        self.assertEqual(list(chunks(lst, 7)), [(1, 2, 3, 4, 5, 6, 7)])
        self.assertEqual(list(chunks(lst, 8)), [(1, 2, 3, 4, 5, 6, 7, 8)])

    def test_stop(self):
        it = chunks([1, 2, 3, 4, 5, 6], 3)
        next(it)
        next(it)
        with self.assertRaises(StopIteration):
            next(it)
        with self.assertRaises(StopIteration):
            next(it)

    def test_references(self):
        lst = [1, 2, 3, 4, 5, 6]
        self.assertEqual(sys.getrefcount(lst), 2)
        it = chunks(lst, 2)
        self.assertEqual(sys.getrefcount(lst), 3)
        self.assertEqual(next(it), (1, 2))
        del lst
        self.assertEqual(next(it), (3, 4))

    def test_item_refleak(self):
        def gen():
            yield DestructionTester()
            yield DestructionTester()
            yield DestructionTester()
        it = chunks(gen(), 2)
        self.assertEqual(DestructionTester.alive, 0)
        item = next(it)
        self.assertEqual(DestructionTester.alive, 2)
        del item
        self.assertEqual(DestructionTester.alive, 0)
        with self.assertRaises(StopIteration):
            next(it)
        self.assertEqual(DestructionTester.alive, 0)

    def test_args(self):
        functions = [
            lambda: chunks(),
            lambda: chunks(1),
            lambda: chunks(1, 2),
            lambda: chunks(1, 2, 3),
            lambda: chunks(1, 2, 3, 4),
            lambda: chunks([]),
            lambda: chunks(""),
            lambda: chunks([], []),
            lambda: chunks([], ""),
        ]
        for func in functions:
            with self.assertRaises(TypeError):
                func()
        functions = [
            lambda: chunks([], 0),
            lambda: chunks([], -1),
        ]
        for func in functions:
            with self.assertRaises(ValueError):
                func()

    def test_repr(self):
        self.assertRegex(
            repr(chunks([1, 2], 1)),
            r'chunks\(<list_iterator object at 0x.+>, 1\)'
        )

class TestSlices(unittest.TestCase):
    def test_simple(self):
        lst = [1, 2, 3, 4]
        self.assertEqual(list(slices(lst, 1)), [(1,), (2,), (3,), (4,)])
        self.assertEqual(list(slices(lst, 2)), [(1, 2), (2, 3), (3, 4)])
        self.assertEqual(list(slices(lst, 3)), [(1, 2, 3,), (2, 3, 4,)])
        self.assertEqual(list(slices(lst, 4)), [(1, 2, 3, 4)])
        self.assertEqual(list(slices(lst, 5)), [])

    def test_stop(self):
        it = slices([1, 2, 3, 4, 5, 6], 3)
        next(it)
        next(it)
        next(it)
        next(it)
        with self.assertRaises(StopIteration):
            next(it)
        with self.assertRaises(StopIteration):
            next(it)

    def test_references(self):
        lst = [1, 2, 3, 4, 5, 6]
        self.assertEqual(sys.getrefcount(lst), 2)
        it = slices(lst, 2)
        self.assertEqual(sys.getrefcount(lst), 3)
        self.assertEqual(next(it), (1, 2))
        del lst
        self.assertEqual(next(it), (2, 3))

    def test_item_refleak(self):
        def gen():
            yield DestructionTester()
            yield DestructionTester()
            yield DestructionTester()
        it = slices(gen(), 2)
        self.assertEqual(DestructionTester.alive, 0)
        item = next(it)
        self.assertEqual(DestructionTester.alive, 2)
        item = next(it)
        self.assertEqual(DestructionTester.alive, 2)
        del item
        self.assertEqual(DestructionTester.alive, 2)
        with self.assertRaises(StopIteration):
            next(it)
        self.assertEqual(DestructionTester.alive, 2)
        del it
        self.assertEqual(DestructionTester.alive, 0)

    def test_args(self):
        functions = [
            lambda: slices(),
            lambda: slices(1),
            lambda: slices(1, 2),
            lambda: slices(1, 2, 3),
            lambda: slices(1, 2, 3, 4),
            lambda: slices([]),
            lambda: slices(""),
            lambda: slices([], []),
            lambda: slices([], ""),
        ]
        for func in functions:
            with self.assertRaises(TypeError):
                func()
        functions = [
            lambda: slices([], 0),
            lambda: slices([], -1),
        ]
        for func in functions:
            with self.assertRaises(ValueError):
                func()

    def test_repr(self):
        self.assertRegex(
            repr(slices([1, 2], 1)),
            r'slices\(<list_iterator object at 0x.+>, 1\)'
        )
