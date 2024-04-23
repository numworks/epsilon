import sys, os, shutil, argparse, re
from concurrent.futures import ThreadPoolExecutor

from helpers.args_types import *
from helpers.print_format import *
from helpers.crc_helper import *
from helpers.gif_helper import *
from helpers.miscellaneous import *
from helpers.screenshot_helper import *


parser = argparse.ArgumentParser(
    description="This script compares the crc32 of the test screenshots dataset with crc32 generated from a given epsilon executable."
)
parser.add_argument(
    "executable",
    metavar="EXE",
    type=existing_file,
    help="epsilon executable to test",
)
parser.add_argument(
    "-r",
    "--ref",
    type=existing_file,
    help="epsilon reference executable, only used for failed scenari to generate screenshots at each step",
)
parser.add_argument(
    "-f",
    "--filter",
    default="",
    help="Specify a regular expression to filter scenari by name.",
)
parser.add_argument(
    "-u",
    "--update",
    action="store_true",
    default=0,
    help="Update crc32 of failed scenari.",
)


def run_test(scenario_name, state_file, executable, computed_crc32_list):
    try:
        p = compute_crc32_process(state_file, executable)
        p.wait()
        print("Computing crc32 of", scenario_name)
        computed_crc32_list.append((scenario_name, find_crc32_in_log(p.stdout)))
    except:  # Handle Interrupt exceptions
        sys.exit(1)


def main():
    # Parse args
    args = parser.parse_args()

    # Create output folder
    output_folder = "compare_crc_output"
    clean_or_create_folder(output_folder)

    # Collect data from dataset
    print("\nCollecting data")
    print("==============================")
    ignored = 0
    computed_crc32_list = []

    with ThreadPoolExecutor(max_workers=8) as pool:
        for scenario_name in sorted(os.listdir(dataset())):
            if not re.match(args.filter, scenario_name):
                continue

            scenario_folder = folder(scenario_name)
            if not os.path.isdir(scenario_folder):
                continue

            print("Collecting data from", scenario_folder)
            state_file = get_file_with_extension(scenario_folder, ".nws")
            reference_crc32_file = get_file_with_extension(scenario_folder, ".txt")
            if state_file == "" or reference_crc32_file == "":
                ignored = ignored + 1
                continue

            pool.submit(
                run_test,
                scenario_name,
                state_file,
                args.executable,
                computed_crc32_list,
            )

    # Compare with ref
    print("\nComparing crc32")
    print("==============================")
    fails = 0
    count = 0
    for scenario_name, computed_crc32 in computed_crc32_list:
        scenario_folder = folder(scenario_name)
        assert os.path.isdir(scenario_folder)
        state_file = get_file_with_extension(scenario_folder, ".nws")
        assert state_file != ""
        reference_crc32_file = get_file_with_extension(scenario_folder, ".txt")
        assert reference_crc32_file != ""

        with open(reference_crc32_file) as f:
            reference_crc32 = f.read().splitlines()

        success = False
        if len(reference_crc32) in [1, 2]:
            # Compare crc32
            success = computed_crc32 in reference_crc32
        else:
            # There can be several lines if crc32 differs between computer architectures
            # TODO: fix inconsistent approximation accross platforms to only have one crc32
            print(
                bold("Error:"),
                reference_crc32_file,
                "contains",
                len(reference_crc32),
                "lines",
            )

        # Print report
        count = count + 1
        if success:
            print("Comparing crc32 of", scenario_name, bold(green("OK")))
        else:
            fails = fails + 1
            print("Comparing crc32 of", scenario_name, bold(red("FAILED")))

        # Take screenshot at each step
        if not success:
            # Create output subfolder
            output_scenario_folder = os.path.join(output_folder, scenario_name)
            os.mkdir(output_scenario_folder)

            # Generate all screenshots and create a gif
            print_underlined("Tested executable")
            computed_folder = os.path.join(output_scenario_folder, "computed")
            list_computed_images = generate_all_screenshots_and_create_gif(
                state_file, args.executable, computed_folder
            )
            store_crc32(computed_crc32, os.path.join(computed_folder, "crc32.txt"))

            # Compare with ref
            if args.ref is not None:
                print_underlined("Reference executable")
                reference_folder = os.path.join(output_scenario_folder, "reference")
                list_reference_images = generate_all_screenshots_and_create_gif(
                    state_file, args.ref, reference_folder
                )
                shutil.copy(
                    reference_crc32_file, os.path.join(reference_folder, "crc32.txt")
                )

                # Generate diff gif
                print_underlined("Diff")
                create_diff_gif(
                    list_reference_images, list_computed_images, output_scenario_folder
                )

            if args.update:
                # Update crc32
                print_underlined("Updating crc32")
                store_crc32(computed_crc32, reference_crc32_file)

            print("--------")

    # Print report
    print("==============================")
    if ignored > 0:
        print(ignored, "folders ignored")
    print(count, "scenari tested")
    if fails > 0:
        print(fails, "failed")
    else:
        print("All good!")
    print("")

    # Clean up
    if fails == 0:
        shutil.rmtree(output_folder)

    sys.exit(fails)


if __name__ == "__main__":
    main()
