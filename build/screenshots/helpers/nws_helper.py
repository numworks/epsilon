import sys, os

BIN_HEADER = b"NWSF"
TXT_HEADER = "NWS"

events_names = []
events_names_extended = []


def import_events_names(path, array):
    with open(path) as f:
        for line in f:
            if line.startswith("//"):
                continue
            array.append(line[1:-3])  # "Left",\n


import_events_names("ion/src/shared/events_names.inc", events_names)
import_events_names("ion/src/shared/events_names_extended.inc", events_names_extended)

events_names += [""] * (256 - len(events_names))
events_ids = {events_names[i]: i for i in range(256) if events_names[i]}


def convert_nws_to_txt(nwspath, txtpath=None):
    if not os.path.isfile(nwspath) or os.path.splitext(nwspath)[1] != ".nws":
        raise argparse.ArgumentTypeError(nwspath + " is not a .nws")

    with open(nwspath, "rb") as f:
        if f.read(4) != BIN_HEADER:
            print("Error:", nwspath, "is ill formatted")
            sys.exit(1)
        version = f.read(8)
        formatVersion = f.read(1)
        language = f.read(2)
        events = f.read()

    if txtpath is None:
        f = sys.stdout
    else:
        f = open(txtpath, "w", encoding="ascii")

    f.write(TXT_HEADER)
    f.write("\n")
    f.write(version.decode())
    f.write("\n")
    f.write(str(formatVersion[0]))
    f.write("\n")
    f.write(language.decode())
    for c in events:
        f.write("\n")
        f.write(events_names_extended[c])
    f.close()


def convert_txt_to_nws(txtpath, nwspath, filter=[]):
    if not os.path.isfile(txtpath) or os.path.splitext(txtpath)[1] != ".txt":
        raise argparse.ArgumentTypeError(txtpath + " is not a .txt")

    with open(txtpath, encoding="ascii") as f:
        if f.readline().strip() != TXT_HEADER:
            print("Error:", txtpath, "is ill formatted")
            sys.exit(1)
        version = f.readline().strip()
        formatVersion = int(f.readline())
        if formatVersion <= 0 or 256 <= formatVersion:
            print("Error:", formatVersion, "is not a valid format version")
            sys.exit(1)
        language = f.readline().strip()
        if len(language) != 2:
            print("Error:", language, "is not a valid language")
            sys.exit(1)
        events = []
        for line in f:
            event = line.strip()
            event_id = events_ids.get(event)
            if event_id is None:
                print("Error:", event, "is not a valid event")
                sys.exit(1)
            if event in filter:
                continue
            events.append(events_ids[event])

    with open(nwspath, "wb") as f:
        f.write(BIN_HEADER)
        f.write(version.encode())
        f.write(bytes([formatVersion]))
        f.write(language.encode())
        f.write(bytes(events))
