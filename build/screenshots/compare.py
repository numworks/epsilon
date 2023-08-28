import sys, getopt, os, time, shutil, subprocess
from datetime import datetime
import helper

help_message = "Use command line: compare.py [-d <dataset>] [-e <executable>] [-g <git_ref>]"
state_file_extension = '.nws'
screenshot_extension = '.png'

def parse_args(argv):
   # Default values
   dataset = 'tests/screenshots_dataset'
   executable = helper.executable_built_path()
   git_ref = ''
   # Get arguments
   try:
      opts, args = getopt.getopt(argv,"hd:e:g:",["help"])
   except getopt.GetoptError:
      print(help_message)
      sys.exit(1)
   for opt, arg in opts:
      if opt in ("-h", "--help"):
         print(help_message)
         print("Compare two sources of screenshots on a sequence of scenari (state files).")
         print("- dataset: the folder containing a first folder with the scenari and a second folder with expected screenshots")
         print("- executable: the epsilon executable")
         print("- git_ref: the epsilon git reference to build the executable")
         sys.exit(1)
      elif opt == "-d":
         dataset = arg
      elif opt == "-e":
         executable = arg
      elif opt == "-g":
         git_ref = arg
   return dataset, executable, git_ref

def check_dataset(dataset):
   if not os.path.isdir(dataset):
      print("Error:", dataset, "is not a directory")
      sys.exit(1)
   scenari = os.path.join(dataset, 'scenari')
   if not os.path.isdir(scenari):
      print("Error: couldn't find scenari folder in", dataset)
      sys.exit(1)
   reference_images = os.path.join(dataset, 'reference_images')
   if not os.path.isdir(dataset):
      print("Error: couldn't find reference images folder in", dataset)
      sys.exit(1)
   return scenari, reference_images

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
   dataset, executable, git_ref = parse_args(argv)
   scenari, reference_images = check_dataset(dataset)

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
   for scenario in sorted(os.listdir(scenari)):

      # Get scenario name
      if os.path.splitext(scenario)[1] != state_file_extension:
         continue
      count = count + 1
      scenario_name = os.path.splitext(scenario)[0]

      # Get reference screenshot
      reference_image = os.path.join(reference_images, scenario_name + screenshot_extension)
      print("Get image from", reference_image)
      if not os.path.isfile(reference_image):
         print("Error: couldn't find reference image for scenario", scenario_name)
         sys.exit(1)
      screenshot_1 = os.path.join(output_folder, scenario_name + '-1' + screenshot_extension)
      shutil.copy(reference_image, screenshot_1)

      # Generate new screenshot
      state_file = os.path.join(scenari, scenario)
      screenshot_2 = os.path.join(output_folder, scenario_name + '-2' + screenshot_extension)
      helper.generate_screenshot(state_file, executable, screenshot_2)

      # Compare screenshots
      screenshot_diff = os.path.join(output_folder, scenario_name + '-diff' + screenshot_extension)
      same_image = compare_images(screenshot_1, screenshot_2, screenshot_diff)
      if same_image:
         print('\033[1m' + state_file + '\t \033[32mOK\033[0m')
      else:
         fails = fails + 1
         print('\033[1m' + state_file + '\t \033[31mFAILED\033[0m')

   # Print report
   print_report(fails, count)

   # Clean up
   if fails == 0:
      shutil.rmtree(output_folder)

   sys.exit(fails)

if __name__ == "__main__":
    main(sys.argv[1:])
