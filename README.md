# Chunks & slices for itertools

    from itertools_chunks_slices import chunks, slices
	data = [1, 2, 3, 4, 5, 6]
    for chunk in chunks(data, 3):
        print(chunk)

    # (1, 2, 3)
    # (4, 5, 6)

    for slice in slices(data, 3):
        print(slice)

    # (1, 2, 3)
    # (2, 3, 4)
    # (3, 4, 5)
    # (4, 5, 6)

## Todo

- behavior on non-exact chunk size
