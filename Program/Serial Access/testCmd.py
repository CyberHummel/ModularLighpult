from subprocess import Popen, PIPE, CalledProcessError

output = []

with Popen("test.exe", stdout=PIPE, bufsize=1, universal_newlines=True) as p:
    for line in p.stdout:
        output.append(line.removeprefix("'").removesuffix("\n").strip())

if p.returncode != 0:
    raise CalledProcessError(p.returncode, p.args)
