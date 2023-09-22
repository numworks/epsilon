#! python3
import argparse, collections, pathlib, sys

parser = argparse.ArgumentParser(description='Compare CSV test results to measure improvement and detect regressions')
parser.add_argument('before', metavar='before.csv', type=pathlib.Path)
parser.add_argument('after', metavar='after.csv', type=pathlib.Path)

def main():
   args = parser.parse_args()

   if not args.before.exists() or not args.after.exists():
      print("File not found")
      sys.exit(1)

   before = args.before.read_text().split('\n')
   after = args.after.read_text().split('\n')

   if len(before) != len(after):
      print("The two CSVs must be created with the same test suite, please rebase the target branch")
      sys.exit(1)

   fixed, broken, changed = 0, 0, 0
   for old, new in zip(before, after):
       if not old : continue
       old_result, *old_args = old.split('\t')
       new_result, *new_args = new.split('\t')
       assert(old_args[0] == new_args[0])
       if old_result != new_result:
           if new_result == 'OK':
              fixed += 1
           elif old_result == 'CRASH':
              changed += 1
           else:
              broken += 1
              if new_result == 'CRASH':
                 print(new_args[0], "now crashes")
              else:
                 print(new_args[0], "fails")
                 print("  with     ", new_args[1])
                 print("  expected ", new_args[2])
       elif old_result == 'BAD' and old_args[2] != new_args[2]:
          print(new_args[0], "changed")
          print("  from ", old_args[2])
          print("  to   ", new_args[2])
          changed += 1

   print(f"{broken=} {fixed=} {changed=}")
   if broken:
      sys.exit(1)

if __name__ == "__main__":
   main()
