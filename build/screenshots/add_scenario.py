import sys, os, shutil
import helper

help_message = "Use command line: add_scenario.py <state_file.nws> <name>\nIf no name is given, we use the one of the state file."
state_file_extension = '.nws'
screenshot_extension = '.png'

def get_args(argv):
   if len(argv) < 1:
      print("Error: missing parameter")
      print(help_message)
      sys.exit(1)
   if len(argv) > 2:
      print("Error: too many parameters")
      print(help_message)
      sys.exit(1)

   # Get state file
   state_file = argv[0]
   if not os.path.exists(state_file) or os.path.splitext(state_file)[1] != state_file_extension:
      print("Error:", state_file, "is not a state file")
      sys.exit(1)

   # Get name
   name = os.path.splitext(state_file)[0]
   if len(argv) == 2:
      name = argv[1]

   return state_file, name

def main(argv):

   # Get params
   state_file, name = get_args(argv)

   # Create new folder
   new_dir = helper.folder(name)
   if os.path.exists(new_dir):
      print("Error:", new_dir, "already exists. Choose another name.")
      sys.exit(1)
   os.mkdir(new_dir)

   # Add the state file
   new_state_file_path = os.path.join(new_dir, "scenario" + state_file_extension)
   shutil.copy(state_file, new_state_file_path)

   # Generate the corresponding screenshot
   screenshot = os.path.join(new_dir, "screenshot" + screenshot_extension)
   executable = helper.executable_built_path()
   helper.generate_screenshot(new_state_file_path, executable, screenshot)

if __name__ == "__main__":
    main(sys.argv[1:])
