#!/usr/bin/env bash

# Run a command in docker, mounting /monorepo and passing flags to docker.
# You may want to pass -d or -it and then the image, say oasis-linux, then the command.
# Examples:
# ./tools/docker_run.sh oasis-linux make -j8 epsilon_test.bin.run
# ./tools/docker_run.sh -it oasis-linux bash

if [[ ! -d .git ]]; then
  echo "docker_run.sh: must be run from the repo root" >&2
  exit 1
fi

exec docker run \
  --rm \
  --pull=never \
  --user "$(id -u):$(id -g)" \
  --mount type=bind,source="$PWD",target=/monorepo \
   "$@"
