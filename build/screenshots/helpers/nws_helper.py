import sys, os

BIN_HEADER = b"NWSF"
TXT_HEADER = "NWSF"

events_names = []
events_names_extended = []


def import_events_names(path, array):
    with open(path) as f:
        for line in f:
            if line.startswith("//"):
                continue
            array.append(line[1:-3])  # "Left",\n


import_events_names(
    "ion/src/shared/layout_events/epsilon/events_names.inc", events_names
)
import_events_names(
    "ion/src/shared/layout_events/epsilon/events_names_extended.inc",
    events_names_extended,
)

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
        f = sys.stdout.buffer
    else:
        f = open(txtpath, "wb")

    f.write(BIN_HEADER)
    f.write(b"\n")
    f.write(version)
    f.write(b"\n")
    f.write(bytes(str(formatVersion[0]), encoding="ascii"))
    f.write(b"\n")
    f.write(language)
    i = 0
    while i < len(events):
        f.write(b"\n")
        event_name = events_names_extended[events[i]]
        f.write(bytes(event_name, encoding="ascii"))
        if event_name == "ExternalText":
            f.write(b" ")
            i += 1
            while True:
                if i == len(events):
                    print("Error:", nwspath, "contains ill formatted external text")
                    sys.exit(1)
                if events[i] == 0:
                    break
                f.write(events[i].to_bytes(1))
                i += 1
        i += 1
    f.close()


def convert_txt_to_nws(txtpath, nwspath, filter=[]):
    if not os.path.isfile(txtpath) or os.path.splitext(txtpath)[1] != ".txt":
        raise argparse.ArgumentTypeError(txtpath + " is not a .txt")

    filteredEvents = 0
    with open(txtpath, encoding="utf-8") as f:
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
            splitted = line.strip().split(" ", 1)
            event = splitted[0]
            event_id = events_ids.get(event)
            if event_id is None:
                print("Error:", event, "is not a valid event")
                sys.exit(1)
            if event in filter:
                filteredEvents += 1
                continue
            events.append(events_ids[event].to_bytes(1))
            if event == "ExternalText":
                events.append(bytes(splitted[1], encoding="utf-8"))
                events.append(b"\x00")
                pass

    with open(nwspath, "wb") as f:
        f.write(BIN_HEADER)
        f.write(version.encode())
        f.write(bytes([formatVersion]))
        f.write(language.encode())
        for e in events:
            f.write(e)

    return filteredEvents
