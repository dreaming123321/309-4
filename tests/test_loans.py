"""借还和日期边界测试。所有数据写在临时目录。"""
import datetime as dt
import pathlib
import subprocess
import sys
import tempfile

exe = pathlib.Path(sys.argv[1]).resolve()
driver = pathlib.Path(sys.argv[2]).resolve()
today = dt.date.today()
old = (today - dt.timedelta(days=40)).isoformat()
yesterday = (today - dt.timedelta(days=1)).isoformat()
today = today.isoformat()
pending = "未归还"
returned = "已归还"

def run(folder, cmd):
    p = subprocess.run([str(exe)], cwd=folder, input=cmd.encode(), stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=15)
    assert p.returncode == 0, (p.returncode, p.stdout, p.stderr)
    return p.stdout.decode('utf-8', errors='replace')

def contents(folder, name):
    return (folder / name).read_text(encoding='utf-8')

with tempfile.TemporaryDirectory(prefix='loan_test_') as path:
    folder = pathlib.Path(path)
    original = 'XL-B001 Book X 10.00 5 5 0\nPL-B001 Book X 10.00 4 4 0\n'
    (folder/'book.txt').write_text(original,encoding='utf-8')
    (folder/'borrow.txt').write_text('',encoding='utf-8')
    run(folder, f'6\nXL-B001\ny\n3\nAlice\nS1\n{old}\nn\n0\n')
    books=contents(folder,'book.txt').splitlines()
    assert books[0].split()[4:6]==['5','2'] and books[1].split()[4:6]==['4','4']
    record=contents(folder,'borrow.txt').split()
    assert record[6]=='3' and record[9]=='XL-B001' and len(record)==10
    assert (dt.date.fromisoformat(record[5])-dt.date.fromisoformat(record[4])).days==30
    assert 'L000001' in contents(folder,'remind.txt') # 已经逾期
    before=contents(folder,'book.txt'),contents(folder,'borrow.txt')
    run(folder, f'7\nXL-B001\ny\n1\nWrong\nS1\n{today}\n0\n')
    assert (contents(folder,'book.txt'),contents(folder,'borrow.txt'))==before
    run(folder, f'7\nXL-B001\ny\n1\nAlice\nS1\n2025-02-30\n0\n')
    assert (contents(folder,'book.txt'),contents(folder,'borrow.txt'))==before
    run(folder, f'7\nXL-B001\ny\n1\nAlice\nS1\n{yesterday}\nn\n0\n')
    record=contents(folder,'borrow.txt').split()
    assert record[6]=='2' and record[8]==pending and len(record)==10
    assert contents(folder,'book.txt').splitlines()[0].split()[5]=='3'
    assert contents(folder,'remind.txt').split()[6]=='2'
    run(folder, f'7\nXL-B001\ny\n2\nAlice\nS1\n{today}\nn\n0\n')
    record=contents(folder,'borrow.txt').split()
    assert record[6]=='0' and record[8]==returned and record[10]==today
    assert contents(folder,'book.txt').splitlines()[0].split()[5]=='5'
    assert not contents(folder,'remind.txt')
    before=contents(folder,'book.txt'),contents(folder,'borrow.txt')
    run(folder, '6\nXL-B001\ny\n2junk\n0\n')
    assert (contents(folder,'book.txt'),contents(folder,'borrow.txt'))==before
    run(folder, '6\nXL-B001\ny\n1\nAlice Smith\n0\n')
    assert (contents(folder,'book.txt'),contents(folder,'borrow.txt'))==before

# 同一人多次借同一本书时，归还指定的那一条。
with tempfile.TemporaryDirectory(prefix='loan_multiple_') as path:
    folder=pathlib.Path(path)
    book='XL-B001 Book X 10.00 5 2 2\n'
    rows=(f'L000001 Book S1 Alice {old} {today} 1 30 {pending} XL-B001\n'
          f'L000002 Book S1 Alice {old} {today} 2 30 {pending} XL-B001\n')
    (folder/'book.txt').write_text(book,encoding='utf-8')
    (folder/'borrow.txt').write_text(rows,encoding='utf-8')
    run(folder, f'7\nXL-B001\ny\n1\nAlice\nS1\n{today}\n2\nn\n0\n')
    records=contents(folder,'borrow.txt').splitlines()
    assert records[0].split()[6]=='1' and records[1].split()[6]=='1'
    assert contents(folder,'book.txt').split()[5]=='3'

# 记录达到上限或文件损坏时，库存和记录都不能被修改。
with tempfile.TemporaryDirectory(prefix='loan_capacity_') as path:
    folder=pathlib.Path(path)
    book='XL-B001 Book X 10.00 5 5 0\n'
    (folder/'book.txt').write_text(book,encoding='utf-8')
    records=''.join(f'L{i:06} Book S1 Alice 2024-01-01 2024-01-31 0 30 {returned}\n' for i in range(1,5001))
    (folder/'borrow.txt').write_text(records,encoding='utf-8')
    run(folder, f'6\nXL-B001\ny\n1\nAlice\nS1\n{today}\n0\n')
    assert contents(folder,'book.txt')==book and contents(folder,'borrow.txt')==records
    for records in ['bad record\n',
                    f'L000001 Book S1 Alice 2024-01-01 2024-01-31 99999999999999999 30 {pending} XL-B001\n']:
        (folder/'borrow.txt').write_text(records,encoding='utf-8')
        run(folder, f'6\nXL-B001\ny\n1\nAlice\nS1\n{today}\n0\n')
        assert contents(folder,'book.txt')==book and contents(folder,'borrow.txt')==records

assert subprocess.run([str(driver)]).returncode==0
with tempfile.TemporaryDirectory(prefix='loan_date_') as path:
    folder=pathlib.Path(path)
    rows=[
      f'L000001 A S A 2024-02-01 2024-03-08 1 30 {pending} XL-B001', # 7天
      f'L000002 B S A 2024-02-01 2024-03-09 1 30 {pending} XL-B002', # 8天
      f'L000003 C S A 2024-01-01 2024-02-01 1 30 {pending} XL-B003', # 逾期
      f'L000004 D S A 2024-01-01 2024-02-01 0 30 {returned} XL-B004 2024-02-27', # 3天
      f'L000005 E S A 2024-01-01 2024-02-01 0 30 {returned} XL-B005 2024-02-28', # 2天
      f'L000006 F S A 2024-01-01 2024-02-01 0 30 {returned}', # 老数据未知日期
    ]
    (folder/'borrow.txt').write_text('\n'.join(rows)+'\n',encoding='utf-8')
    assert subprocess.run([str(driver),'2024-03-01'],cwd=folder).returncode==0
    reminder=contents(folder,'remind.txt'); records=contents(folder,'borrow.txt')
    assert 'L000001' in reminder and 'L000003' in reminder and 'L000002' not in reminder
    assert 'L000004' not in records and 'L000005' in records and 'L000006' in records and 'L000003' in records
    assert subprocess.run([str(driver),'2024-03-02'],cwd=folder).returncode==0
    assert 'L000005' not in contents(folder,'borrow.txt') and 'L000003' in contents(folder,'remind.txt')

print('PASS: borrow and return, stock, campus, invalid inputs, leap dates, 7/8 days, overdue, 3-day retention')
