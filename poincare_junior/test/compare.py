#! python3
import argparse, collections, pathlib, sys

parser = argparse.ArgumentParser(description='Compare CSV test results to measure improvement and detect regressions')
parser.add_argument('before', metavar='before.csv', type=pathlib.Path)
parser.add_argument('after', metavar='after.csv', type=pathlib.Path)
parser.add_argument('--markdown', action='store_true', help="format output with Markdown")

def main():
   args = parser.parse_args()

   if not args.before.exists() or not args.after.exists():
      print("File not found", file=sys.stderr)
      sys.exit(1)

   before = args.before.read_text().split('\n')
   after = args.after.read_text().split('\n')

   if len(before) != len(after):
      print("The two CSVs must be created with the same test suite, please rebase the target branch", file=sys.stderr)
      sys.exit(1)

   fixed, broken, changed = [], [], []
   for old, new in zip(before, after):
       if not old : continue
       old_result, *old_args = old.split('\t')
       new_result, *new_args = new.split('\t')
       assert(old_args[0] == new_args[0])
       if old_result != new_result:
           if new_result == 'OK':
              fixed.append(new_args[0])
           elif old_result == 'CRASH':
              changed.append(f"{new_args[0]}\n  no longer crashes")
           else:
              if new_result == 'CRASH':
                 broken.append(f"{new_args[0]}\n  now crashes")
              else:
                 changed.append(f"{new_args[0]}\n  returns     {new_args[1]}\n  expected  {new_args[2]}")
       elif old_result == 'BAD' and old_args[2] != new_args[2]:
          changed.append(f"{new_args[0]}\n  from {old_args[2]}\n  to   {new_args[2]}")

   for items, name in (fixed, 'fixed'), (changed, 'changed'), (broken, 'broken'):
      if not items: continue
      if args.markdown:
         print(f"<details><summary>{len(items)} {name}</summary><pre>")
      else:
         print("="*8, len(items), name, "="*8)
      for item in items:
         print(item)
      if args.markdown:
         print(f"</pre></details>")

if __name__ == "__main__":
   main()
