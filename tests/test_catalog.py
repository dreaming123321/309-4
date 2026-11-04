"""Run against a compiled console program; all data stays in temporary folders."""
import pathlib
import subprocess
import sys
import tempfile

exe = pathlib.Path(sys.argv[1]).resolve()
pending = "\u672a\u5f52\u8fd8"
returned = "\u5df2\u5f52\u8fd8"

def run(books, command, records="", missing_records=False):
    with tempfile.TemporaryDirectory(prefix="catalog_test_") as directory:
        folder = pathlib.Path(directory)
        (folder / "book.txt").write_text(books, encoding="utf-8")
        if not missing_records:
            (folder / "borrow.txt").write_text(records, encoding="utf-8")
        result = subprocess.run([str(exe)], cwd=folder, input=command.encode("utf-8"),
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=10)
        assert result.returncode == 0, (result.returncode, result.stderr)
        return (folder / "book.txt").read_text(encoding="utf-8"), result.stdout

books = "XL-B001 Same X 10.00 5 5 0\nPL-B001 Same X 10.00 4 4 0\n"
updated, _ = run(books, "1\n1\nSame\n0\n")
assert updated.splitlines()[0].split()[4:6] == ["6", "6"]
assert updated.splitlines()[1].split()[4:6] == ["4", "4"]
full = "XL-B001 Same X 10.00 200 200 0\n"
assert run(full, "1\n1\nSame\n0\n")[0] == full

holes = "XL-B001 A X 10.00 1 1 0\nXL-B003 C X 10.00 1 1 0\n"
added, _ = run(holes, "1\n1\nNew\nX\n10\n2\n0\n")
assert added.splitlines()[-1].split()[0] == "XL-B004"
history = f"L000001 Old S Alice 2026-10-01 2026-10-31 0 30 {returned} XL-B009\n"
added, _ = run(holes, "1\n1\nNew\nX\n10\n2\n0\n", history)
assert added.splitlines()[-1].split()[0] == "XL-B010"
for command in ["1\n3\n0\n", "1\n1\nBad Name\n0\n", "1\n1\nNew\nX\n-1\n0\n", "1\n1\nNew\nX\n501\n0\n", "1\n1\nNew\nX\n10\n201\n0\n"]:
    assert run(holes, command)[0] == holes
assert run(holes + "bad\n", "1\n1\nA\n0\n")[0] == holes + "bad\n"
capacity = "".join(f"XL-B{i:03} Book{i} X 10.00 2 2 0\n" for i in range(1, 301))
assert run(capacity, "1\n1\nNew\n0\n")[0] == capacity
assert run(capacity, "1\n1\nBook1\n0\n")[0].splitlines()[0].split()[4:6] == ["3", "3"]

assert run(books, "2\nSame\n0\n")[0] == books
assert run(books, "2\nXL-B001\nn\n0\n")[0] == books
assert run(books, "2\nXL-B001\ny\n0\n")[0] == books.splitlines(keepends=True)[1]
assert run(books, "2\nNO-ID\n0\n")[0] == books
loan = f"L000001 Same S Alice 2026-11-01 2026-12-01 1 30 {pending} XL-B001\n"
assert run(books, "2\nXL-B001\n0\n", loan)[0] == books
legacy = f"L000001 Same S Alice 2026-11-01 2026-12-01 1 30 {pending}\n"
assert run(books, "2\nPL-B001\n0\n", legacy)[0] == books
assert run(books, "2\nPL-B001\ny\n0\n", loan)[0] == books.splitlines(keepends=True)[0]
stock = "XL-B001 Same X 10.00 5 4 0\n"
assert run(stock, "2\nXL-B001\n0\n")[0] == stock
assert run(books, "2\nXL-B001\n0\n", missing_records=True)[0] == books
assert run(books, "2\nXL-B001\n0\n", "bad record\n")[0] == books

_, output = run(books, "3\nSame\n0\n")
assert b"ID: XL-B001" in output and b"ID: PL-B001" in output
_, output = run(books, "3\nPL-B001\n0\n")
assert b"ID: PL-B001" in output and b"ID: XL-B001" not in output
_, output = run(books, "3\nX\n0\n")
assert b"ID: XL-B001" in output and b"ID: PL-B001" in output
print("PASS: same-name +1, campus separation, unique IDs, input/capacity limits, deletion guards, all-match search")
