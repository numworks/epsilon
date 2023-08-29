import sys, os, shutil, subprocess
from pathlib import Path
import helper

help_message = "Use command line: take_for_each_step.py <state_file.nws> [<output_folder>]"
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

   # Get output folder
   output_folder = "screenshots_each_step"
   if len(argv) == 2:
      output_folder = argv[1]

   return state_file, output_folder

def main(argv):

   # Get params
   state_file, output_folder = get_args(argv)

   # Clean or create output folder
   if os.path.exists(output_folder):
      print("Warning:", output_folder, "already exists. Its content will be erased.")
      shutil.rmtree(output_folder)
   os.mkdir(output_folder)

   # Generate the screenshots
   executable = helper.executable_built_path()
   helper.generate_all_screenshots(state_file, executable, output_folder)
   list_images = [image.as_posix() for image in sorted(Path(output_folder).glob("*" + screenshot_extension))]
   if len(list_images) == 0:
      print("Error: couldn't take screenshots")
      sys.exit(1)
   print("All done, screenshots taken in", output_folder)

   # Create gif
   print("Creating gif")
   gif = os.path.join(output_folder, "scenario.gif")
   subprocess.run("convert -set delay '%[fx:t==(n-1) ? 175 : 35]' " + ' '.join(list_images) + " " + gif, shell=True)
   if not os.path.exists(gif):
      print("Error: couldn't create gif")

if __name__ == "__main__":
    main(sys.argv[1:])
