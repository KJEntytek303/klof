SRC_DIR := ./src
BUILD_DIR := ./build
SRCS := $(shell find $(SRC_DIR) -type f | grep -E '\.java')
COMP_RESULTS := $(SRCS:%.java/%.class)
OBJS := $(SRCS:$(SRC_DIR)/%.java=$(BUILD_DIR)/%.class)
JAVA_HOME := /lib/jvm/jdk-21-openjdk
export JAVA_HOME

.PHONY: all
all: buildClasses

.PHONY: buildClasses
buildClasses: $(OBJS)
	$(shell find $(SRC_DIR) -type d | sed -e 's#^$(SRC_DIR)#$(BUILD_DIR)#' | xargs mkdir -p)

$(BUILD_DIR)/%.class: $(SRC_DIR)/%.java
	javac $<
	mv $(shell echo $< | sed -e 's#.java$$#.class#') $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
