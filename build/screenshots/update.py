import sys, os, argparse
import helper
import args_types

parser = argparse.ArgumentParser(description='This script updates the screenshot and the crc32 of a state file in the test screenshots dataset.')
parser.add_argument('name', metavar='NAME', help='name of scenario folder in the test screenshots dataset')
parser.add_argument('-e', '--executable', default=helper.executable_built_path(), type=args_types.existing_file, help='epsilon executable')

def main():
   # Parse args
   args = parser.parse_args()

   # Print warning
   print("\033[1mYour are updating screenshots and crc32 for this scenario.\033[0m")
   print("\033[1mPlease make sure before launching this script that you checked screenshots AT EACH STEP and with THE SAME executable.\033[0m")
   print("\033[1mIf you have a doubt, run take_for_each_step.py with the same executable.\033[0m")
   print("-------")

   scenario_folder = helper.folder(args.name)
   if not os.path.isdir(scenario_folder):
      print("Error:", args.name, "is not a folder in the test screenshots dataset")
   state_file = helper.get_file_with_extension(scenario_folder, ".nws")
   reference_image =  helper.get_file_with_extension(scenario_folder, ".png")
   crc_file = helper.get_file_with_extension(scenario_folder, ".txt")

   # Update screenshot
   print("Replacing", reference_image)
   os.remove(reference_image)
   helper.generate_screenshot(state_file, args.executable, reference_image)

   # Update crc32
   print("Rewriting", crc_file)
   helper.compute_and_store_crc32(state_file, args.executable, crc_file)

if __name__ == "__main__":
    main()
