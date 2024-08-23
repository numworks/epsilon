def format_include_guard(header_file, should_correct_error):
    """Check that include guards are correctly formatted. The correct format for the file relative/path/to_file.h is RELATIVE__PATH__TO_FILE_H"""
    from pathlib import Path
    header_path = Path(header_file)
    # Use __ for / instead of _, otherwise dir/file.h and dir_file.h would have the same guard.
    guard_macro = ''.join([c if c.isalnum() else '__' if c == '/' else '_' for c in header_path.as_posix().upper()])
    with open(header_path, 'r') as file:
        lines = file.readlines()
        if len(lines) < 3:
            return False
        if lines[0].startswith('#ifndef') and lines[0].strip() != f'#ifndef {guard_macro}':
            if should_correct_error:
                lines[0] = f'#ifndef {guard_macro}\n'
            else:
                return False
        if lines[1].startswith('#define') and lines[1].strip() != f'#define {guard_macro}':
            if should_correct_error:
                lines[1] = f'#define {guard_macro}\n'
            else:
                return False
        if lines[-1].startswith('#endif') and lines[-1].strip() != f'#endif  // {guard_macro}':
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



