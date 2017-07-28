from more_itertools import chunked, sliced
from itertools_chunks_slices import chunks

def f():
	data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] * 10000

	func = chunks

	num = 0

	for _ in range(100):
		for x in func(data, 2):
			num += len(x)

	print(num)


f()