# Haussmann

Shared build system between C++ projects, using makefiles.

## Building

Run `make help` (or simply `make`) for a list of the main targets.
There are two types of targets:
- exectuable targets: combine one of the named goals with an extension, along with optional flavors, e.g. `make kernel.A.allow3rdparty.elf`
- miscellaneous targets: do not necessarily resolve to an executable file, e.g. `make format`, `make epsilon.dfu`

Build is controlled by variables that can be supplied on the command line:
- `DEBUG`: 0 or 1
- `PLATFORM`: one of the supported platforms (e.g. `n0110`, `macos`...). The special values `simulator` and `host` will cause PLATFORM to become the name of the host environment.
- `VERBOSE`: set to 1 to print the commands as they are exectuted

## Including Haussmann in a project

In your application, add this repository as a submodule.

In your makefile, define the following variables:
- `PATH_haussmann`: the path to the submodule in the project
- `OUTPUT_ROOT`: files will be built in `$(OUTPUT_ROOT)/<'debug'|'release'>/$(PLATFORM)/`
- `APP_NAME`
- `APP_VERSION`
- you might give default values to `DEBUG` and `PLATFORM`

... then `include $(PATH_haussmann)/Makefile`.

## Creating goals

The function `create_goal` is used to generate rules for making an executable target.
Its arguments are:
- the name of the goal, must be letters, numbers, and underscores
- a list of modules
- (optional) a directory to append to the output path
- (optional) a description

```
$(call create_goal,demo, \
  database \
  gui \
  render \
  ,, \
  A simple demo of the app
)
```
This will create a goal called demo, with 3 modules, no subdirectory, and a description. If `.bin` is an extension on the current platform, `make demo.bin` is an acceptable target.

When building a goal, the list of modules indicate which source files and which compilation flags will be used (see modules below for more explanations). Source files can be further configured using flavors :
- Invoking `make demo.test.bin` will forward the `test` flavor to all modules.
- In the module list, writing `gui.light` will forward the `light` flavor to the `gui` module every build.

> [!CAUTION]
> Because of some limitations, the subdirectory argument must be declared by adding its name to the variable `ALL_SPECIAL_SUBDIRECTORIES` prior to including Haussmann.

## Using modules

A module is a collection of files and flags, grouped into several variables:
- `PATH_<module>`
- `VERSION_<module>`
- `SOURCES_<module>`: the list of source files for this module, with optional flags. The paths may be relative to the root of the project, or the output directory.
- `SFLAGS_<module>`: those flags are used when building all objects of goals involving this module.
- `PRIVATE_SFLAGS_<module>`: on the other hand, those flags are only used when building the objects of this module.
- `LDFLAGS_<module>`: flags used for linking executables using this module.
- `LDDEPS_<module>`: linked executables using this module will depend on those non-sources files (e.g. linker scripts).
- `PRIORITY_TARGETS_<module>`: those files will be created before the others in the goal using this module.

Modules are typically created in their own subdirectory, using a file named `module.mak`. The function `import_module` takes the path of the module and include the `module.mak` file if found.

```
$(call import_module,database,src/database)
```

The function `create_module` is used to define the basics of a new module: its version, some sources, and the flag `-I$(PATH_<module>/include`.
Its arguments are:
- the name of the module
- the version, an integer
- a list of sources, with optional tastes. The paths are relative to the module's path.

```
$(call create_module,database,3, \
  query.cpp \
  transaction.cpp \
  postgresql.cpp:-lite \
  sqlite:+lite \
)
```

The other variables of the module need to be edited by hand.

> [!TIP]
> A module does not always require a `module.mak` file, but make sure `import_module` is called before `create_module`, as the prior will define `PATH_<module>` used by the latter.

## Flavors and tastes

Tastes modify the sources that are built depending on the flavors passed to
the module at compilation, e.g.:
- `a.cpp:+f1` means a.cpp is only compiled if the flavor `f1` is provided (e.g. `make goal.f1.exe` or `$(call create_goal,goal,module.f1)`
- `b.cpp:-f1` is only compiled if the flavor `f1` is not provided.

If several tastes qualify the same file, they combine as an AND i.e. the file is only compiled if all its flavors are satified, e.g.:
- `c.cpp:+f1:+f2:-f3` is compiled only if both `f1` and `f2` are provided and `f3` is not.

To combine tastes as an OR, you can specify the file several times:
```
$(call create_module,module,1, \
  d.cpp:+f1 \
  d.cpp:+f2 \
)
```
`d.cpp` will be compiled if either `f1` or `f2` is provided.

## Modules inter-dependencies

### Locking dependencies

A module might depend on some other modules. To prevent conflicts, the version
of the dependencies can be listed in a lock file; if someone attempts to build
with versions not listed in the lock, the compilation will fail.

e.g. Say a module `main_mod` depends on `depA` and `depB`.
- In the repository for `main_mod`, there should be a goal called `main_mod` requiring `depA` and `depB`.
- Calling `make main_mod.lock` will create the file `lock.mak`, listing the dependencies and their current versions.
- In another project, when building a goal requiring `main_mod`, the build system will look for `lock.mak` at the root of `main_mod`and compare the listed versions with the ones provided.

> [!CAUTION]
> Lock files should only be re-generated once the module has been tested against the new versions of its dependencies.

### Solving modules versions conflicts

If a project uses module `modA` at version 2, but `modB` has only been tested
against version 1 of `modA`, the compilation will fail (provided there is a
lock.mak file in `modB`).

The project can either:
- downgrade to `modA` version 1, if possible.
- if `modA` version 2 is required, the `modB` maintainers will need to release a new version that uses the new `modA` version.

> [!TIP]
> Compilation will fail as soon as a conflict is encountered.
> To list all conflicts as once for a specific goal, use `make goal.modules`.
> This will display a list of all modules required by `goal` (with the version provided), their respective dependencies (as specified in their `lock.mak`, with the version required), and will highlight any discrepancy.

## Documentation

Goals are automatically added to `make help` along with their description.

The functions `document_extension` and `document_other_target` add new entries to their respective sections.
```
$(call document_extension,exe,A windows executable)
$(call document_other_target,format,Format modified .cpp and .py files)
```

## Examples

A reusable library:
```
SomeLib
├── src
│   ├── core.c
│   ├── dangerous.c
│   └── safe.c
├── test
│   ├── haussmann
│   └── SomeOtherLib
├── Makefile
│   └── Define PATH_haussmann, DEBUG, PLATFORM, OUTPUT_ROOT...
│       include haussmann/Makefile
│       $(call import_module,some_lib,.)
│       $(call import_module,some_other_lib,SomeOtherLib)
│       $(call create_goal,test_runner,some_lib some_other_lib)
└── module.mak
    └── $(call create_module,some_lib,2, \
          src/core.c \
          src/dangerous.c:+unsafe \
          src/safe.c:-unsafe \
        )
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
        $(call import_module,some_lib,sources/external/SomeLib)
        $(call import_module,first_module,sources/FirstModule)
        $(call import_module,second_module,sources/SecondModule)
        $(call create_goal,app,some_lib.unsafe first_module second_module)
```

## Commits

Commit messages follow the [Angular Guidelines](https://www.conventionalcommits.org/en/v1.0.0/).
See [conventional-commit-types](https://github.com/pvdlg/conventional-commit-types?tab=readme-ov-file#commit-types) for a description of each type.
