import sys, getopt, os, shutil, subprocess
from datetime import datetime
import helper

help_message = "Use command line: compare.py [-e <executable>] [-g <git_ref>]"
state_file_extension = '.nws'
screenshot_extension = '.png'

def parse_args(argv):
   # Default values
   executable = helper.executable_built_path()
   git_ref = ''
   # Get arguments
   try:
      opts, args = getopt.getopt(argv,"he:g:",["help"])
   except getopt.GetoptError:
      print(help_message)
      sys.exit(1)
   for opt, arg in opts:
      if opt in ("-h", "--help"):
         print(help_message)
         print("Compare two sources of screenshots on a sequence of scenari (state files).")
         print("- executable: the epsilon executable")
         print("- git_ref: the epsilon git reference to build the executable")
         sys.exit(1)
      elif opt == "-e":
         executable = arg
      elif opt == "-g":
         git_ref = arg
   return executable, git_ref

def compare_images(screenshot_1, screenshot_2, screenshot_diff):
   res = subprocess.getoutput(" ".join(["compare", "-metric", "mae", screenshot_1, screenshot_2, screenshot_diff]))
   mae_value = res.split(" ")[0]
   if mae_value == "0":
      os.remove(screenshot_1)
      os.remove(screenshot_2)
      os.remove(screenshot_diff)
      return True
   return False

def print_report(fails, count):
   if count == 0:
      print("No state file found")
      return
   print("==============================")
   print(count, "scenari tested")
   if fails > 0:
      print(fails, "failed")
   else:
      print("All good!")

def main(argv):
   # Parse arguments
   executable, git_ref = parse_args(argv)
   dataset = 'tests/screenshots_dataset'

   # Create output folder
   output_folder = 'compare_output_' + datetime.today().strftime('%d-%m-%Y_%Hh%M')
   os.mkdir(output_folder)

   # Create executable
   if git_ref != '':
      executable = helper.create_executable(git_ref)
   helper.check_executable(executable)

   # Compare screenshots
   print("Comparing screenshots")
   print("==============================")
   fails = 0
   count = 0
   for scenario_name in sorted(os.listdir(dataset)):
      scenario_folder = os.path.join(dataset, scenario_name)
      if not os.path.isdir(scenario_folder):
         continue
      count = count + 1

      # Get state file
      state_file = helper.get_file_with_extension(scenario_folder, state_file_extension)

      # Get reference screenshot
      reference_image = helper.get_file_with_extension(scenario_folder, screenshot_extension)
      screenshot_1 = os.path.join(output_folder, scenario_name + '-1' + screenshot_extension)
      shutil.copy(reference_image, screenshot_1)

      # Generate new screenshot
      screenshot_2 = os.path.join(output_folder, scenario_name + '-2' + screenshot_extension)
      helper.generate_screenshot(state_file, executable, screenshot_2)

      # Compare screenshots
      screenshot_diff = os.path.join(output_folder, scenario_name + '-diff' + screenshot_extension)
      same_image = compare_images(screenshot_1, screenshot_2, screenshot_diff)
      if same_image:
         print('\033[1m' + scenario_folder + '\t \033[32mOK\033[0m')
      else:
         fails = fails + 1
         print('\033[1m' + scenario_folder + '\t \033[31mFAILED\033[0m')

   # Print report
   print_report(fails, count)

   # Clean up
   if fails == 0:
      shutil.rmtree(output_folder)

   sys.exit(fails)

if __name__ == "__main__":
    main(sys.argv[1:])
