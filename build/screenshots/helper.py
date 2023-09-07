import sys, os, shutil
from subprocess import Popen, DEVNULL, PIPE
from pathlib import Path

def dataset():
   return "tests/screenshots_dataset"

def folder(scenario_name):
   return os.path.join(dataset(), scenario_name)

def clean_or_create_folder(folder):
   if os.path.exists(folder):
      print("Warning:", folder, "already exists. Its content will be erased.")
      shutil.rmtree(folder)
   os.mkdir(folder)

def get_file_with_extension(folder, file_extension):
   found_file = ''
   for file in sorted(os.listdir(folder)):
      if os.path.splitext(file)[1] == file_extension:
         if found_file != '':
            print("\033[1m\033[31mError:\033[0m too many ", file_extension, "in", folder)
            return ''
         found_file = os.path.join(folder, file)
   if found_file == '':
      print("\033[1m\033[31mError:\033[0m no", file_extension, "in", folder)
   return found_file

def list_images_in_folder(folder):
   return [image.as_posix() for image in sorted(Path(folder).glob("*.png"))]

def print_error(stderr):
   err = stderr.read().decode()
   if err:
      if err[-1] == "\n":
         err = err[:-1]
      print(err)
      sys.exit(1)

def generate_screenshot(state_file, executable, screenshot):
   print("Generating screenshot of", state_file)
   p = Popen("./" + executable + " --headless --load-state-file " + state_file + " --take-screenshot " + screenshot, shell=True, stdout=DEVNULL, stderr=PIPE)
   print_error(p.stderr)
   if not os.path.isfile(screenshot):
      print("Error: couldn't take screenshot")
      sys.exit(1)

def generate_all_screenshots(state_file, executable, folder):
   print("Generating all screenshots of", state_file)
   clean_or_create_folder(folder)
   p = Popen("./" + executable + " --headless --load-state-file " + state_file + " --take-all-screenshots " + folder, shell=True, stdout=DEVNULL, stderr=PIPE)
   print_error(p.stderr)
   list_images = list_images_in_folder(folder)
   if len(list_images) == 0:
      print("Error: no screenshots taken")
      sys.exit(1)
   print("All done, screenshots taken in", folder)
   return list_images

def create_gif(list_images, folder, gif_name = "scenario"):
   print("Creating gif")
   gif = os.path.join(folder, gif_name + ".gif")
   p = Popen("convert -set delay '%[fx:t==(n-1) ? 175 : 35]' " + ' '.join(list_images) + " " + gif, shell=True, stdout=DEVNULL, stderr=PIPE)
   print_error(p.stderr)
   if not os.path.exists(gif):
      print("Error: couldn't create gif")
      sys.exit(1)
   print("Done, gif created in", folder)

def generate_all_screenshots_and_create_gif(state_file, executable, folder):
   clean_or_create_folder(folder)
   list_images = generate_all_screenshots(state_file, executable, os.path.join(folder, "images"))
   create_gif(list_images, folder)
   return list_images

def find_crc32_in_log(log_file):
   with open(log_file) as f:
      lines = f.readlines()
   # Assertion mode: crc32 line is the only line
   crc_line = lines[0]
   if len(lines) > 1:
      # Debug mode: crc32 line is the before to last line
      assert len(lines) > 3
      crc_line = lines[-2]
   assert "CRC32 of all screenshots: " in crc_line
   return crc_line.split()[-1]
   return

def compute_crc32(state_file, executable, log_file):
   print("Computing crc32 of", state_file)
   with open(log_file, "w") as f:
      p = Popen("./" + executable + " --headless --load-state-file " + state_file + " --compute-hash", shell=True, stdout=f, stderr=PIPE)
      print_error(p.stderr)
   return find_crc32_in_log(log_file)

def store_crc32(crc32, crc_file):
   with open(crc_file, "w") as f:
      f.write(crc32)

def compute_and_store_crc32(state_file, executable, crc_file):
   crc32 = compute_crc32(state_file, executable, crc_file)
   store_crc32(crc32, crc_file)

def images_are_identical(screenshot_1, screenshot_2, screenshot_diff):
   p = Popen(" ".join(["compare", "-metric", "mae", screenshot_1, screenshot_2, screenshot_diff]), shell=True, stdout=DEVNULL, stderr=PIPE)
   mae = p.stderr.read().decode().split(" ")[0]
   return mae == "0"

def print_report(fails, count):
   if count == 0:
      print("No state file found")
      return
   print("==============================")
   print(count, "scenari tested")
   if fails > 0:
      print(fails, "failed")
   else:
      print("All good!")
   print("")

def concatenate_images(list_images, output):
   from PIL import Image
   import numpy as np
   # Concatenate same size images
   concatenated = Image.fromarray(np.concatenate([np.array(Image.open(im).convert("RGBA")) for im in list_images], axis=1))
   concatenated.save(output)

def create_diff_gif(list_images_1, list_images_2, gif_destination_folder):
   diff_folder = os.path.join(gif_destination_folder, "diff")
   os.mkdir(diff_folder)
   n = len(list_images_2)
   if len(list_images_1) != n:
      print("Error: lists of images are not the same size. Cannot compare.")
      sys.exit(1)
   print("Generating all diff images")
   diff_image = os.path.join(diff_folder, "diff.png")
   for i in range(n):
      images_are_identical(list_images_1[i], list_images_2[i], diff_image)
      concatenated_image = os.path.join(diff_folder, "img-{:04d}.png".format(i))
      concatenate_images([list_images_1[i], list_images_2[i], diff_image], concatenated_image)
   os.remove(diff_image)
   print("All done")
   list_diff_images = list_images_in_folder(diff_folder)
   assert len(list_diff_images) == n
   create_gif(list_diff_images, gif_destination_folder, "diff")
   shutil.rmtree(diff_folder)
