PRJTYPE = StaticLib

# Test rule
TESTDIR = test
test: build
	@$(MAKE) -C $(strip $(TESTDIR)) -f $(MKLOC) clean
	@$(MAKE) -C $(strip $(TESTDIR)) -f $(MKLOC) run

# Demo rule
DEMODIR = demo
demo: build
	@$(MAKE) -C $(strip $(DEMODIR)) -f $(MKLOC) clean
	@$(MAKE) -C $(strip $(DEMODIR)) -f $(MKLOC) run
