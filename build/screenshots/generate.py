import sys, getopt, os, time, shutil, subprocess
from datetime import datetime
import helper

help_message = "Use command line: compare.py <folder_with_scenari> [-e <executable>] [-g <git_ref>]"
state_file_extension = '.nws'
screenshot_extension = '.png'

def parse_args(argv):
   if len(argv) < 1:
      print("Error: missing parameters")
      print(help_message)
      sys.exit()
   # Default arguments
   scenari = argv[0]
   executable = helper.executable_built_path()
   git_ref = ''
   # Get arguments
   try:
      opts, args = getopt.getopt(argv[1:],"he:g:",["help"])
   except getopt.GetoptError:
      print(help_message)
      sys.exit(2)
   for opt, arg in opts:
      if opt in ("-h", "--help"):
         print(help_message)
         print("Generate a screenshot of the final state of each scenari.")
         print("- folder_with_scenari: the folder containing the scenari")
         print("- executable: the epsilon executable")
         print("- git_ref: the epsilon git reference to build the executable")
         sys.exit()
      elif opt == "-e":
         executable = arg
      elif opt == "-g":
         git_ref = arg
   return scenari, executable, git_ref

def main(argv):
   # Parse arguments
   scenari, executable, git_ref = parse_args(argv)

   # Create output folder
   output_folder = 'generated_screenshots_' + datetime.today().strftime('%d-%m-%Y_%Hh%M')
   os.mkdir(output_folder)

   # Create executable
   if git_ref != '':
      executable = helper.create_executable(git_ref)
   helper.check_executable(executable)

   # Compare screenshots
   print("Generating screenshots")
   count = 0
   for scenario in sorted(os.listdir(scenari)):

      # Get scenario name
      if os.path.splitext(scenario)[1] != state_file_extension:
         continue
      count = count + 1
      scenario_name = os.path.splitext(scenario)[0]

      # Generate new screenshot
      state_file = os.path.join(scenari, scenario)
      screenshot = os.path.join(output_folder, scenario_name + '-2' + screenshot_extension)
      helper.generate_screenshot(state_file, executable, screenshot)

   if count == 0:
      print("No state file found in", scenari)

if __name__ == "__main__":
    main(sys.argv[1:])
