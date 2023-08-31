import sys, os, shutil, subprocess, argparse
from datetime import datetime
import helper
import args_types

parser = argparse.ArgumentParser(description='This script compares the screenshots of the test screenshots dataset with screenshots generated from a given epsilon executable.')
parser.add_argument('-e', '--executable', default=helper.executable_built_path(), type=args_types.existing_file, help='epsilon executable')

def compare_images(screenshot_1, screenshot_2, screenshot_diff):
   res = subprocess.getoutput(" ".join(["compare", "-metric", "mae", screenshot_1, screenshot_2, screenshot_diff]))
   mae_value = res.split(" ")[0]
   if mae_value == "0":
      os.remove(screenshot_1)
      os.remove(screenshot_2)
      os.remove(screenshot_diff)
      return True
   return False

def main():
   # Parse args
   args = parser.parse_args()

   # Create output folder
   output_folder = 'compare_output_' + datetime.today().strftime('%d-%m-%Y_%Hh%M')
   helper.clean_or_create_folder(output_folder)

   # Compare screenshots
   print("Comparing screenshots")
   print("==============================")
   fails = 0
   count = 0
   for scenario_name in sorted(os.listdir(helper.dataset())):
      scenario_folder = helper.folder(scenario_name)
      if not os.path.isdir(scenario_folder):
         continue
      count = count + 1

      # Get state file
      state_file = helper.get_file_with_extension(scenario_folder, '.nws')

      # Get reference screenshot
      reference_image = helper.get_file_with_extension(scenario_folder, '.png')
      screenshot_1 = os.path.join(output_folder, scenario_name + '-1.png')
      shutil.copy(reference_image, screenshot_1)

      # Generate new screenshot
      screenshot_2 = os.path.join(output_folder, scenario_name + '-2.png')
      helper.generate_screenshot(state_file, args.executable, screenshot_2)

      # Compare screenshots
      screenshot_diff = os.path.join(output_folder, scenario_name + '-diff.png')
      same_image = compare_images(screenshot_1, screenshot_2, screenshot_diff)
      if same_image:
         print('\033[1m' + scenario_folder + '\t \033[32mOK\033[0m')
      else:
         fails = fails + 1
         print('\033[1m' + scenario_folder + '\t \033[31mFAILED\033[0m')

   # Print report
   helper.print_report(fails, count)

   # Clean up
   if fails == 0:
      shutil.rmtree(output_folder)

   sys.exit(fails)

if __name__ == "__main__":
    main()
