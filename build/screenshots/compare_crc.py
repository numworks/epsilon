import sys, os, shutil, argparse
from datetime import datetime
import helper
import args_types

parser = argparse.ArgumentParser(description='This script compares the crc32 of the test screenshots dataset with crc32 generated from a given epsilon executable.')
parser.add_argument('executable', metavar='EXE', type=args_types.existing_file, help='epsilon executable to test')
parser.add_argument('-r', '--ref', type=args_types.existing_file, help='epsilon reference executable, only used for failed scenari to generate screenshots at each step')

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

      print("Getting data from", scenario_folder)

      # Get state file
      state_file = helper.get_file_with_extension(scenario_folder, '.nws')
      if state_file == '':
         continue

      # Get reference crc32
      reference_crc32_file = helper.get_file_with_extension(scenario_folder, '.txt')
      if reference_crc32_file == '':
         continue
      with open(reference_crc32_file) as f:
         reference_crc32 = f.read().splitlines()
      # There can be several lines if crc32 differs between computer architectures
      # TODO: fix inconsistent approximation accross platforms to only have one crc32
      assert len(reference_crc32) == 1 or len(reference_crc32) == 2

      # Compute new crc32
      computed_crc32 = helper.compute_crc32(state_file, args.executable, log_file)

      # Compare crc32
      success = computed_crc32 in reference_crc32

      # Print report
      count = count + 1
      if success:
         print('\033[1m' + scenario_folder + '\t \033[32mOK\033[0m')
      else:
         fails = fails + 1
         print('\033[1m' + scenario_folder + '\t \033[31mFAILED\033[0m')

      # Take screenshot at each step
      if not success:

         # Create output subfolder
         output_scenario_folder = os.path.join(output_folder, scenario_name)
         os.mkdir(output_scenario_folder)

         # Generate all screenshots and create a gif
         print("\033[4mBugged executable\033[0m:")
         computed_folder = os.path.join(output_scenario_folder, "computed")
         list_computed_images = helper.generate_all_screenshots_and_create_gif(state_file, args.executable, computed_folder)
         helper.store_crc32(computed_crc32, os.path.join(computed_folder, "crc32.txt"))

         # Compare with ref
         if not args.ref is None:
            print("\033[4mReference executable\033[0m:")
            reference_folder = os.path.join(output_scenario_folder, "reference")
            list_reference_images = helper.generate_all_screenshots_and_create_gif(state_file, args.ref, reference_folder)
            shutil.copy(reference_crc32_file, os.path.join(reference_folder, "crc32.txt"))

            # Generate diff gif
            print("\033[4mDiff\033[0m:")
            diff_folder = os.path.join(output_scenario_folder, "diff")
            os.mkdir(diff_folder)
            n = len(list_computed_images)
            assert len(list_reference_images) == n
            print("Generating all diff images")
            temp = os.path.join(diff_folder, "diff.png")
            for i in range(n):
               helper.images_are_identical(list_reference_images[i], list_computed_images[i], temp)
               diff_image = os.path.join(diff_folder, "img-{:04d}.png".format(i))
               helper.concatenate_images([list_reference_images[i], list_computed_images[i], temp], diff_image)
            os.remove(temp)
            print("All done")
            list_diff_images = helper.list_images_in_folder(diff_folder)
            assert len(list_diff_images) == n
            helper.create_gif(list_diff_images, output_scenario_folder, "diff")
            shutil.rmtree(diff_folder)

         print("--------")

   # Print report
   helper.print_report(fails, count)

   # Clean up
   if fails == 0:
      shutil.rmtree(output_folder)
   else:
      os.remove(log_file)

   sys.exit(fails)

if __name__ == "__main__":
    main()
