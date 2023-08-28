import sys, os, shutil

help_message = "Use command line: update_images.py <compare_output_folder>"
screenshot_extension = '.png'

def main(argv):
   if len(argv) != 1:
      print(help_message)
      sys.exit()
   compare_output_folder = argv[0]
   if not os.path.isdir(compare_output_folder):
      print("Error:", compare_output_folder, "is not a directory")
      sys.exit()

   extension = '-2' + screenshot_extension

   for file in os.listdir(compare_output_folder):
      if not file.endswith(extension):
         continue
      current_image_path =  os.path.join(compare_output_folder, file)
      scenario_name = file.replace(extension, '')
      destination_image_path = os.path.join('tests/screenshots_dataset', scenario_name, 'screenshot' + screenshot_extension)
      if os.path.exists(destination_image_path):
         os.remove(destination_image_path)
      else:
         print("Couldn't find previous image for", scenario_name)
      shutil.copy(current_image_path, destination_image_path)

if __name__ == "__main__":
    main(sys.argv[1:])
