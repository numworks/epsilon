import sys, os, shutil, argparse
from datetime import datetime
import helper
import args_types

parser = argparse.ArgumentParser(description='This script compares the crc32 of the test screenshots dataset with crc32 generated from a given epsilon executable.')
parser.add_argument('-e', '--executable', default=helper.executable_built_path(), type=args_types.existing_file, help='epsilon executable')

def main():
   # Parse args
   args = parser.parse_args()

   # Create output folder
   output_folder = 'compare_crc_output_' + datetime.today().strftime('%d-%m-%Y_%Hh%M')
   os.mkdir(output_folder)

   # Compare crc32
   print("Comparing crc32")
   print("==============================")
   fails = 0
   count = 0
   log_file = os.path.join(output_folder, "log.txt")
   for scenario_name in sorted(os.listdir(helper.dataset())):
      scenario_folder = helper.folder(scenario_name)
      if not os.path.isdir(scenario_folder):
         continue
      count = count + 1

      # Get state file
      state_file = helper.get_file_with_extension(scenario_folder, '.nws')

      # Get reference crc32
      reference_crc32_file = helper.get_file_with_extension(scenario_folder, '.txt')
      with open(reference_crc32_file) as f:
         lines = f.readlines()
      assert len(lines) == 1
      reference_crc32 = lines[0]

      # Compute new crc32
      computed_crc32 = helper.compute_crc32(state_file, args.executable, log_file)

      # Compare crc32
      if computed_crc32 == reference_crc32:
         print('\033[1m' + scenario_folder + '\t \033[32mOK\033[0m')
      else:
         fails = fails + 1
         print('\033[1m' + scenario_folder + '\t \033[31mFAILED\033[0m')

   # Print report
   helper.print_report(fails, count)

   # Clean up
   shutil.rmtree(output_folder)

   sys.exit(fails)

if __name__ == "__main__":
    main()
