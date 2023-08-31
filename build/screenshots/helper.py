import sys, os, subprocess

def dataset():
   return "tests/screenshots_dataset"

def folder(scenario_name):
   return os.path.join(dataset(), scenario_name)

def get_file_with_extension(folder, file_extension):
   found_file = ''
   for file in sorted(os.listdir(folder)):
      if os.path.splitext(file)[1] == file_extension:
         if found_file != '':
            print("Error: too many", file_extension, "in", folder)
            sys.exit(1)
         found_file = os.path.join(folder, file)
   if found_file == '':
      print("Error: no", file_extension, "in", folder)
      sys.exit(1)
   return found_file

def executable_built_path():
   host = os.uname()[0]
   if host == "Linux":
      exe = "linux/epsilon.bin"
   elif host == "Darwin":
      exe = "macos/epsilon.app/Contents/MacOS/Epsilon"
   else:
      print("Error: couldn't find executable")
      sys.exit(1)
   return os.path.join("output/debug/simulator", exe)

def generate_screenshot(state_file, executable, screenshot):
   print("Generating screenshot of", state_file)
   subprocess.run("./" + executable + " --headless --load-state-file " + state_file + " --take-screenshot " + screenshot, shell=True, stdout=subprocess.DEVNULL)
   if not os.path.isfile(screenshot):
      print("Error: couldn't take screenshot")
      sys.exit(1)

def generate_all_screenshots(state_file, executable, folder):
   print("Generating all screenshots of", state_file)
   subprocess.run("./" + executable + " --headless --load-state-file " + state_file + " --take-all-screenshots " + folder, shell=True, stdout=subprocess.DEVNULL)

def find_crc32_in_log(log_file):
   with open(log_file) as f:
      lines = f.readlines()
   assert len(lines) > 2
   crc_line = lines[-2]
   assert "CRC32 of all screenshots: " in crc_line
   return crc_line.split()[-1]
   return

def compute_crc32(state_file, executable, log_file):
   print("Computing crc32 of", state_file)
   with open(log_file, "w") as f:
      subprocess.run("./" + executable + " --headless --load-state-file " + state_file + " --compute-hash", shell=True, stdout=f)
   return find_crc32_in_log(log_file)

def store_crc32(crc32, crc_file):
   with open(crc_file, "w") as f:
      f.write(crc32)

def compute_and_store_crc32(state_file, executable, crc_file):
   crc32 = compute_crc32(state_file, executable, crc_file)
   store_crc32(crc32, crc_file)

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
