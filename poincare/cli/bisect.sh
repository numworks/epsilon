# To find when 2+2 stopped being equal to 4 run:
# git bisect run ./poincare/cli/bisect.sh simplify 2+2 4

if make PLATFORM=host -j8 -f poincare/cli/Makefile poincare_cli.bin; then
    echo $3 > /tmp/poincare_cli_expected
    if ./output/debug/macos/arm64/poincare_cli.bin $1 $2 > /tmp/poincare_cli_actual; then
        if cmp /tmp/poincare_cli_expected /tmp/poincare_cli_actual > /dev/null; then
            exit 0
        else
            exit 1
        fi
    else
        exit 1
    fi
else
    # The build failed, tell bisect to skip that commit
    exit 125
fi
