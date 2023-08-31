import sys, os, shutil, argparse
import helper
import args_types

parser = argparse.ArgumentParser(description='This script takes the new images saved in the output folder of compare.py and move them in the test screenshots dataset to replace the old ones.')
parser.add_argument('compare_output_folder', type=args_types.existing_directory, help='output folder of compare.py')

def main():
   # Parse args
   args = parser.parse_args()

   extension = "-2.png"

   for file in os.listdir(args.compare_output_folder):
      if not file.endswith(extension):
         continue
      current_image_path =  os.path.join(args.compare_output_folder, file)
      scenario_name = file.replace(extension, '')
      destination_image_path = os.path.join(helper.folder(scenario_name), "screenshot.png")
      if os.path.exists(destination_image_path):
         os.remove(destination_image_path)
      else:
         print("Couldn't find previous image for", scenario_name)
      shutil.copy(current_image_path, destination_image_path)

if __name__ == "__main__":
    main()
