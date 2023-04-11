import sys

BIN_HEADER = b'NWSF'
TXT_HEADER = 'NWS'

# This script detects the format (binary or textual) of the path given as its
# first and only argument and prints the state file in the other format.
#
# You can setup git to use it to show diff of nws files with :
#   echo "*.nws diff=nws" >> .git/info/attributes
#   git config diff.nws.textconv "python3 build/screenshots/nws2txt.py"

event_names = []

with open('ion/src/shared/events_names.inc') as f:
    for line in f:
        if line.startswith('//'): continue
        event_names.append(line[1:-3]) # "Left",\n

event_names += [""] * (256 - len(event_names))
event_ids = {event_names[i] : i for i in range(256) if event_names[i]}

def is_binary_nws(path):
    with open(path, 'rb') as f:
        header = f.read(4)
    if header == b'':
        exit(0)
    if header == BIN_HEADER:
        return True
    assert header == (TXT_HEADER + '\n').encode()
    return False

def print_as_text(nwspath):
    with open(nwspath, 'rb') as f:
        assert f.read(4) == BIN_HEADER
        version = f.read(8)
        formatVersion = f.read(1)
        language = f.read(2)
        events = f.read()

    print(TXT_HEADER)
    print(version.decode())
    print(formatVersion[0])
    print(language.decode())

    for c in events:
        print(event_names[c])

def print_as_nws(txtpath):
    with open(txtpath, encoding='ascii') as f:
        assert f.readline().strip() == TXT_HEADER
        version = f.readline().strip()
        formatVersion = int(f.readline())
        assert 0 < formatVersion < 256
        language = f.readline().strip()
        assert len(language) == 2
        events = [event_ids[line.strip()] for line in f]

    out = sys.stdout.buffer
    out.write(BIN_HEADER)
    out.write(version.encode())
    out.write(bytes([formatVersion]))
    out.write(language.encode())
    out.write(bytes(events))


if __name__ == "__main__":
    assert len(sys.argv) == 2
    path = sys.argv[1]
    if is_binary_nws(path):
        print_as_text(path)
    else:
        print_as_nws(path)
