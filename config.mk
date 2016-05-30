PRJTYPE = StaticLib

# Test rule
TESTDIR = test
test: build
	@$(MAKE) -C $(strip $(TESTDIR)) -f $(MKLOC) clean
	@$(MAKE) -C $(strip $(TESTDIR)) -f $(MKLOC) run
