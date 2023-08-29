import sys, os, time, subprocess

def checkout_ref(git_ref):
   return subprocess.run(["git", "checkout", git_ref], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)

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

def create_executable(git_ref):
   previous_git_ref = subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('ascii').strip()
   print("Creating executable from ref", git_ref, "...")
   time.sleep(1) # To avoid changed files and target having the same last modified date
   subprocess.run(["git", "checkout", git_ref], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
   checkout_ref(git_ref)
   subprocess.run("make -j32 PLATFORM=simulator DEBUG=1", shell=True, stdout=subprocess.DEVNULL)
   print("Executable successfully created")
   checkout_ref(previous_git_ref)
   return executable_built_path()

def check_executable(executable):
   if not os.path.isfile(executable):
      print("Error: couldn't find executable")
      sys.exit(1)

def generate_screenshot(state_file, executable, screenshot):
   print("Generating screenshot of", state_file)
   subprocess.run("./" + executable + " --headless --load-state-file " + state_file + " --take-screenshot " + screenshot, shell=True, stdout=subprocess.DEVNULL)
   if not os.path.isfile(screenshot):
      print("Error: couldn't take screenshot")
      sys.exit(1)
