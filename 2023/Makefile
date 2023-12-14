# debug with
# make -pRrq -n

#Utility function from https://stackoverflow.com/questions/16144115/makefile-remove-duplicate-words-without-sorting
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

# Bin directory
BIN_DIR=build/
SRC_DIR=src/
INC_DIR=
LIB_DIR=lib/

DAYS=dayTemplate day1 day2 day3 day4 day5 day6 day7 day8 day9

CURRENT_DAY= day9

# Implicitly "created" rules like:
# day1_SRC=day1.cpp
# day2_SRC=day2.cpp
# Can define a day rule if any extra file needs to be included
dayTemplate_SRC=dayTemplateExtra.cpp

# day2_OBJECTS=$(addprefix $(BIN_DIR), $(day2_SRC))
# Generate rule like: day2_OBJECTS: day2.cpp.o
# By default it will search for day1.cpp. No need for day1_SRC if no extra file is needed
$(foreach _day,$(DAYS),$(eval $(_day)_OBJECTS=$(addsuffix .o, $(addprefix $(BIN_DIR),$(_day).cpp $($(_day)_SRC)))))

# day1_BIN=/bin/day1
$(foreach _day,$(DAYS),$(eval $(_day)_BIN=$(BIN_DIR)$(_day)))

# $(day1_BIN): $(day1_OBJECTS)
$(foreach _day,$(DAYS),$(eval $($(_day)_BIN): $($(_day)_OBJECTS)))

TARGET_BINS=$(foreach _day,$(DAYS),$($(_day)_BIN))

# Compilation flags
CXXFLAGS= -std=c++20 -Werror -Wall -Wextra -Wpedantic -g -fsanitize=address,leak,undefined

CXXFLAGS+=  $(addprefix -I, $(INC_DIR))

LDFLAGS=    $(addprefix -L, $(LIB_DIR)) \
            $(addprefix -l, $(LIBS)) \
			-fsanitize=address,leak,undefined

# compile cpp files
$(BIN_DIR)%.cpp.o: $(SRC_DIR)%.cpp
	@echo compiling $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# Create dependency files
$(BIN_DIR)%.cpp.d: $(SRC_DIR)%.cpp
	@echo creating $@
	@$(CXX) $< -MM -MT '$(patsubst %.d,%.o, $@) $@' -MF $@

DEPENDENCIES= $(foreach _day,$(DAYS), $(patsubst %.o,%.d,$($(_day)_OBJECTS)))
#$(foreach _day,$(DAYS), $($(_day)_OBJECTS)) $(DEPENDENCIES) : Makefile
$(DEPENDENCIES) : Makefile

define DAY_LINK_RULE

# this rule is only linking, no CFLAGS required
${1}: ${2}
	@echo linking $$@
	@$$(CXX) $$(LDFLAGS) $$(TARGET_ARCH) $$^ $$(LOADLIBES) $$(LDLIBS) -o $$@

endef

$(foreach _day,$(DAYS),$(eval $(call DAY_LINK_RULE,$($(_day)_BIN), $($(_day)_OBJECTS))))

define call_bin
	@echo
	@echo Running ${1}
	@${1}

endef

run: $(TARGET_BINS)
	$(foreach _bin,$(TARGET_BINS),$(call call_bin,$(_bin)))

build_current: $($(CURRENT_DAY)_BIN)

run_current: build_current
	$(call call_bin,$($(CURRENT_DAY)_BIN))

clean:
	-$(RM) build/*

.PHONY: run run_current build_current clean compile/src/%.cpp

# Marks them as secondary so it won't rebuild them if not needed
.SECONDARY: $(foreach _day,$(DAYS),$($(_day)_OBJECTS)) $(DEPENDENCIES)

# intermediate removes the files if it no longer needed
# .INTERMEDIATE: $(OBJECTS)
.INTERMEDIATE: %.cpp.d

.DEFAULT_GOAL=run_current

# Rule that will be used from the vscode compile file rule.
# example invocation make 'compile/src/day1.cpp'
.SECONDEXPANSION:
compile/%.cpp: build/$$(subst \,/,%).cpp.o
	@echo compiled '$<'

include $(DEPENDENCIES)
