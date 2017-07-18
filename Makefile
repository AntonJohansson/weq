#CXX ?= clang++
# Generate .clang_complete
CXX = cc_args.py clang++

BIN_NAME = weq.out

BIN_PATH = bin
BUILD_PATH = build
SRC_PATH = src
HDR_PATH = include

CXXFLAGS += -std=c++1z -Wall -Wextra -g

INCLUDES = -I $(HDR_PATH)
# Cocoa and IOKit required for OSX
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lentityx -lglfw3 -ldl -lfreeimage

SRC_EXT = cpp

# Function used to check variables. Use on the command line:
# make print-VARNAME
# Useful for debugging and adding features
print-%: ; @echo $*=$($*)

SRCS = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)')
OBJS = $(SRCS:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
DEPS = $(OBJS:.o=.d)

# Create directories
.PHONY: dirs
dirs:
	@echo "Creating directories";
	@mkdir -p $(dir $(OBJS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories!"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# Compile stuff
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Link the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJS)
	@echo "Linking: $@"
	$(CXX) $(OBJS) $(LIBS) -o $@

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@
