# Haussmann

Shared build system between C++ projects, using makefiles.

## Using this repository

In your application, add this repository as a submodule.

In your makefiles, define the following variables:
- `PATH_haussmann`: the position of the submodule
- `OUTPUT_ROOT`: the directory where the outputs are tucked away
- `DEBUG`: typically 0 or 1
- `PLATFORM`: indicates which toolchain and special rules to use

... then `include $(PATH_haussmann)/Makefile`.

Additionally, a `VERBOSE` variable can be set to 1 to make a verbose build.

Repositories that are not applications but shared libraries should not include
this repository directly in their primary makefiles, only in their tests, as it
will be provided by the application.

## Creating modules

### Overview

Modules are thematically appropriate sections of code. They are characterized by
the following variables:
- `VERSION_<name>`: The version of the module.
- `SOURCES_<name>`: The list of all sources, in varying formats, that form the module.
- `PRIVATE_SFLAGS_<name>`: A list of compilation flags that are only provided when building that module's objects.
- `SFLAGS_<name>` and `LDFLAGS_<name>`: Compilation and link flags added to all objects built along with that module.

### Helpers and syntax

A simple module named "example_module" in version 3 can be defined with the following syntax:
```
$(eval $(call create_module,example_module,3,\
  first_source.c\
  directory/second_source.cpp\
  tasteful_source.cpp:+good_taste:-bad_taste\
))
```

This will take care of initializing `SOURCES_example_module` with the given files,
and `SFLAGS_example_module` with the `-I` flag for this module's `include` directory.

_Tastes_ modify the sources that are built depending on the _flavors_ passed to
the module at compilation.
e.g.:
- `a.cpp:+f1:-f2` means a.cpp is only compiled if the flavor `f1` is provided and `f2` is not provided.
- `b.cpp:+f1:+f2` is compiled if both `f1` and `f2` are provided.
- `c.cpp:-f1:-f2` is compiled if neither `f1` and `f2` are provided.

### Locking modules

A module might depend on some other modules. To prevent conflicts, the version
of the dependencies can be listed in a lock file; if someone attempts to build
with versions not listed in the lock, the compilation will fail.

e.g. Say a module `main_mod` depends on `depA` and `depB`.
- In the repository for `main_mod`, there should be a goal called `main_mod` requiring `depA` and `depB`.
- Calling `make main_mod.lock` will create the file `lock.mak`, listing the dependencies and their current versions.
- In another project, when building a goal requiring `main_mod`, the build system will look for `lock.mak` at the root of `main_mod`and compare the listed versions with the ones provided.

Lock files should only be re-generated once the module has been tested against
the new versions of its dependencies.

## Using modules

### Including modules

Applications select which modules they want to use with the following syntax:
```
$(eval $(call import_module,module_name,module/path))
```

If a file `module/path/Makefile` is found, it will be included. This Makefile
should typically contain the call to `create_module`.

### Creating goals

Applications can create a _goal_ with the following syntax:
```
$(eval $(call create_goal,goal_name,\
  first_module\
  second_module.flavor1.flavor2\
))
```

This will generate rules for building a binary file (e.g. for embedded
platforms `goal_name.elf`) composed of objects for the specified modules, along
with any platform-specific rules.

Goals can be invoked with additional _flavors_ (e.g. `goal_name.alternate.elf`).
Those are passed to modules, along with any flavor imposed when creating the
goal, to be compared against the sources' tastes.

e.g. When building `goal_name.alternate.elf`, `first_module` will be compiled
with the `alternate` flavor, and `second_module` with the `alternate`, `flavor1`
and `flavor2` flavors.

### Solving modules versions conflicts

If a project uses module `modA` at version 2, but `modB` has only been tested
against version 1 of `modA`, the compilation will fail (provided there is a
lock.mak file in `modB`).

The project can either:
- downgrade to `modA` version 1, if possible.
- if `modA` version 2 is required, the `modB` maintainers will need to release a new version that uses the new `modA` version.

Compilation will fail as soon as a conflict is encountered. To list all
conflicts as once for a specific goal, use `make <goal>.versions`.
This will display a list of all modules required by <goal> (with the version
provided), their respective dependencies (as specified in their `lock.mak`, with
the version required), and will highlight any discrepancy.

## Examples

A reusable library:
```
SomeLib
├── sources
│   ├── core.c
│   ├── dangerous.c
│   └── safe.c
├── test
│   ├── haussmann
│   ├── SomeOtherLib
│   └── Makefile
│       └── Define PATH_haussmann, DEBUG, PLATFORM, OUTPUT_ROOT
│           include haussmann/Makefile
│           $(eval $(call import_module,some_lib,..))
│           $(eval $(call import_module,some_other_lib,SomeOtherLib))
│           $(eval $(call create_goal,test_runner,some_lib some_other_lib))
└── Makefile
    └── $(eval $(call create_module,some_lib,2,\
          core.c\
          dangerous.c:+unsafe\
          safe.c:-unsafe\
        ))
```

An application using the previous library and some other private modules:
```
SomeApp
├── haussmann
├── sources
│   ├── external
│   │   └── SomeLib
│   ├── FirstModule
│   └── SecondModule
└── Makefile
    └── Define PATH_haussmann, DEBUG, PLATFORM, OUTPUT_ROOT
        include haussmann/Makefile
        $(eval $(call import_module,some_lib,sources/external/SomeLib))
        $(eval $(call import_module,first_module,sources/FirstModule))
        $(eval $(call import_module,second_module,sources/SecondModule))
        $(eval $(call create_goal,app,some_lib.unsafe first_module second_module))
```

## Commits

Commit messages follow the [Angular Guidelines](https://www.conventionalcommits.org/en/v1.0.0/).
See [conventional-commit-types](https://github.com/pvdlg/conventional-commit-types?tab=readme-ov-file#commit-types) for a description of each type.
