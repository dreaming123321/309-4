"""模拟跨日与保存中断，不改电脑时间或正式数据。"""
import pathlib
import subprocess
import sys
import tempfile
import time
exe=pathlib.Path(sys.argv[1]).resolve()
pending='未归还'
returned='已归还'
with tempfile.TemporaryDirectory(prefix='loan_clock_') as path:
    folder=pathlib.Path(path)
    (folder/'clock.txt').write_text('2024-04-03',encoding='utf-8')
    (folder/'book.txt').write_text('XL-B001 A X 10.00 5 4 1\n',encoding='utf-8')
    (folder/'borrow.txt').write_text(
        f'L000001 A S Alice 2024-03-12 2024-04-11 1 30 {pending} XL-B001\n'
        f'L000002 B S Alice 2024-03-01 2024-03-31 0 30 {returned} XL-B002 2024-04-01\n',encoding='utf-8')
    process=subprocess.Popen([str(exe)],cwd=folder,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        for _ in range(40):
            if (folder/'remind.txt').exists(): break
            time.sleep(0.1)
        assert (folder/'remind.txt').exists() and not (folder/'remind.txt').read_text(encoding='utf-8')
        # 程序停在借书输入界面时也应更新，不需要回到主菜单。
        process.stdin.write(b'6\n'); process.stdin.flush(); time.sleep(0.3)
        (folder/'clock.txt').write_text('2024-04-04',encoding='utf-8')
        for _ in range(60):
            if 'L000001' in (folder/'remind.txt').read_text(encoding='utf-8'): break
            time.sleep(0.1)
        assert 'L000001' in (folder/'remind.txt').read_text(encoding='utf-8')
        assert 'L000002' not in (folder/'borrow.txt').read_text(encoding='utf-8')
        process.stdin.write(b'unknown\n0\n'); process.stdin.flush()
        assert process.wait(timeout=10)==0
    finally:
        if process.poll() is None: process.kill(); process.wait()
with tempfile.TemporaryDirectory(prefix='loan_recovery_') as path:
    folder=pathlib.Path(path)
    (folder/'clock.txt').write_text('2024-04-03',encoding='utf-8')
    book='XL-B001 A X 10.00 5 4 1\n'
    borrow=f'L000001 A S Alice 2024-03-12 2024-04-11 1 30 {pending} XL-B001\n'
    (folder/'book.backup').write_text(book,encoding='utf-8')
    (folder/'borrow.backup').write_text(borrow,encoding='utf-8')
    (folder/'book.txt').write_text('wrong\n',encoding='utf-8')
    (folder/'borrow.txt').write_text('wrong\n',encoding='utf-8')
    (folder/'borrow.pending').write_text('saving\n',encoding='utf-8')
    p=subprocess.run([str(exe)],cwd=folder,input=b'0\n',stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=10)
    assert p.returncode==0
    assert (folder/'book.txt').read_text(encoding='utf-8')==book
    assert (folder/'borrow.txt').read_text(encoding='utf-8')==borrow
    assert not (folder/'borrow.pending').exists()
print('PASS: live date refresh while waiting for input, interrupted save recovery')
