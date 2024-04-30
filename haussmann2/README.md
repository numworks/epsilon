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

Repository that are not applications but shared libraries should not include
this repository directly in their primary makefiles, only in their tests, as it
will be provided by the application.

## Creating modules

Modules are thematically appropriate sections of code. They are characterized by
the following variables:
- `SOURCES_<name>`: The list of all sources, in varying formats, that form the module.
- `PRIVATE_SFLAGS_<name>`: A list of compilation flags that are only provided when building that module's objects.
- `SFLAGS_<name>` and `LDFLAGS_<name>`: Compilation and link flags added to all objects built along with that module.

A simple module can be defined with the following syntax:
```
$(eval $(call create_module,module_name,\
  first_source.c\
  directory/second_source.cpp\
  tasteful_source.cpp:+good_taste:-bad_taste\
))
```

This will take care of initializing `SOURCES_module_name` with the given files,
and `SFLAGS_module_name` with the `-I` flag for this module's `include` directory.

_Tastes_ modify the sources that are built depending on the _flavors_ passed to
the module at compilation.
e.g.:
- `a.cpp:+f1:-f2` means a.cpp is only compiled if the flavor `f1` is provided and `f2` is not provided.
- `b.cpp:+f1:+f2` is compiled if both `f1` and `f2` are provided.
- `c.cpp:-f1:-f2` is compiled if neither `f1` and `f2` are provided.

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
Those are passed to modules, along with any flavors imposed when creating the
goal, to be compared against the sources' tastes.

e.g. When building `goal_name.alternate.elf`, `first_module` will be compiled
with the `alternate` flavor, and `second_module` with the `alternate`, `flavor1`
and `flavor2` flavors.

## Commits

Commit messages follow the [Angular Guidelines](https://www.conventionalcommits.org/en/v1.0.0/).
See [conventional-commit-types](https://github.com/pvdlg/conventional-commit-types?tab=readme-ov-file#commit-types) for a description of each type.
