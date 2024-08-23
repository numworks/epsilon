def format_include_guard(header_file, should_correct_error):
    """Check that include guards are correctly formatted. The correct format is RELATIVE_PATH_TO_FILE_H_"""
    from pathlib import Path
    header_path = Path(header_file)
    guard_macro = ''.join([c if c.isalnum() else '_' for c in header_path.as_posix().upper()])
    with open(header_path, 'r') as file:
        lines = file.readlines()
        if len(lines) < 3:
            return False
        if not lines[0].strip() == f'#ifndef {guard_macro}':
            if should_correct_error:
                lines[0] = f'#ifndef {guard_macro}\n'
            else:
                return False
        if not lines[1].strip() == f'#define {guard_macro}':
            if should_correct_error:
                lines[1] = f'#define {guard_macro}\n'
            else:
                return False
        if not lines[-1].strip() == f'#endif // {guard_macro}':
            if should_correct_error:
                lines[-1] = f'#endif // {guard_macro}\n'
            else:
                return False
    if should_correct_error:
        with open(header_path, 'w') as file:
            file.writelines(lines)
    return True

# Run format_include_guard on all files passed as arguments (using argparse)
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Check that include guards are correctly formatted.')
    parser.add_argument('--dry-run', action='store_true', help='Do not correct errors')
    parser.add_argument('header_files', nargs='*', help='Header files to check')
    args = parser.parse_args()
    has_error = False
    for header_file in args.header_files:
        if not format_include_guard(header_file, not args.dry_run):
            has_error = True
            print(f'Wrong header guard format in {header_file}')
    if has_error and args.dry_run:
        exit(1)



