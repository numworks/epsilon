import sys, os, shutil, argparse
import helper
import args_types

parser = argparse.ArgumentParser(description='This script adds a scenario to the test screenshots dataset. It takes a state file, generates its screenshot, and place them is a subfolder of the screenshots dataset.')
parser.add_argument('state_file', metavar='STATE_FILE', type=args_types.existing_state_file, help='state file (with extension .nws)')
parser.add_argument('-n', '--name', help='name of the subfolder (if no name is given, we take the name of the state file)')
parser.add_argument('-e', '--executable', default=helper.executable_built_path(), type=args_types.existing_file, help='epsilon executable')

def main():
   # Parse args
   args = parser.parse_args()
   name = args.name
   if name is None:
      # If no name was given, we take the name of the state file
      name = os.path.splitext(args.state_file)[0]

   # Create new folder
   new_dir = helper.folder(name)
   if os.path.exists(new_dir):
      print("Error:", new_dir, "already exists. Choose another name.")
      sys.exit(1)
   print("Creating folder", new_dir)
   os.mkdir(new_dir)

   # Add the state file
   new_state_file_path = os.path.join(new_dir, "scenario.nws")
   shutil.copy(args.state_file, new_state_file_path)

   # Generate the corresponding screenshot
   screenshot = os.path.join(new_dir, "screenshot.png")
   helper.generate_screenshot(new_state_file_path, args.executable, screenshot)

if __name__ == "__main__":
    main()
