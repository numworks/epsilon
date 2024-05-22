import argparse
import pathlib

from helpers.args_types import *


parser = argparse.ArgumentParser(
    description="This script deletes images in the folder corresponding to events before the given event, used as start marker to create gifs. If the event occurs several times, it starts from the last occurrence."
)
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=existing_state_file,
    help="state file (with extension .nws)",
)
parser.add_argument(
    "-e",
    "--event-number",
    default=221,  # BatteryCharching
    help="marker event id (default 221 = BatteryCharging)",
)
parser.add_argument(
    "-o", "--output-folder", default="screenshots_each_step", help="output folder"
)


def main():
    args = parser.parse_args()

    # Find last marker event occurrence
    markerEventIndex = -1
    with open(args.state_file, "rb") as f:
        f.read(15)  # skip header
        events = list(f.read())
    for i, e in enumerate(events):
        if e == args.event_number:
            markerEventIndex = i

    # Remove images up to markerEventIndex
    folder = pathlib.Path(args.output_folder) / "images"
    for i in range(markerEventIndex):
        png = folder / f"img-{i:04}.png"
        if png.exists():
            png.unlink()


if __name__ == "__main__":
    main()
