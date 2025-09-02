_head_commit := $(shell $(GIT) rev-parse --quiet HEAD^{commit})
BUILD_COMMIT ?= HEAD
_build_commit := $(shell $(GIT) rev-parse --verify --quiet $(BUILD_COMMIT)^{commit} || echo error)

ifneq ($(_build_commit),$(_head_commit))

ifeq ($(_build_commit),error)
$(error Error: Invalid base branch $(BUILD_COMMIT). Please use a valid branch name or commit hash.)
endif

_worktree_dir := $(OUTPUT_DIRECTORY)/$(_build_commit)
.PHONY: _worktree_build
_worktree_build:
	if [ ! -d $(_worktree_dir) ]; then \
		$(GIT) worktree add -f $(_worktree_dir) $(BUILD_COMMIT) && \
		rm -rf $(_worktree_dir)/.git && \
		$(GIT) worktree prune; \
	fi
	$(MAKE) -C $(_worktree_dir) $(MAKECMDGOALS) BUILD_COMMIT=

%: _worktree_build
	@ :

else
include $(PATH_haussmann)/src/rules.head.mak
endif
