import sys, os, shutil, argparse
import helper
import args_types

parser = argparse.ArgumentParser(description='This script takes the new images saved in the output folder of compare.py and move them in the test screenshots dataset to replace the old ones. It also updates the crc32.')
parser.add_argument('compare_output_folder', type=args_types.existing_directory, help='output folder of compare.py')
parser.add_argument('-e', '--executable', default=helper.executable_built_path(), type=args_types.existing_file, help='epsilon executable')

def main():
   # Parse args
   args = parser.parse_args()

   extension = "-2.png"

   for file in os.listdir(args.compare_output_folder):
      if not file.endswith(extension):
         continue
      current_image_path =  os.path.join(args.compare_output_folder, file)
      scenario_name = file.replace(extension, '')
      scenario_folder = helper.folder(scenario_name)

      # Update screenshot
      destination_image_path = os.path.join(scenario_folder, "screenshot.png")
      if os.path.exists(destination_image_path):
         print("Replacing", destination_image_path, "with", current_image_path)
      else:
         print("Warning: couldn't find previous image for", scenario_name)
      shutil.copy(current_image_path, destination_image_path)

      # Update crc32
      state_file = helper.get_file_with_extension(scenario_folder, ".nws")
      crc_file = helper.get_file_with_extension(scenario_folder, ".txt")
      print("Rewriting", crc_file)
      helper.compute_and_store_crc32(state_file, args.executable, crc_file)

if __name__ == "__main__":
    main()
