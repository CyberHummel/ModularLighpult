from subprocess import Popen, PIPE, CalledProcessError
output = []

with Popen("", stdout=PIPE, bufsize=1, universal_newlines=True) as p:
    for line in p.stdout:
        output.append(line.removeprefix("'").removesuffix("\n").strip())

if p.returncode != 0:
    raise CalledProcessError(p.returncode, p.args)

print(output)

#246 1
#241 2
#235 3
#231 4