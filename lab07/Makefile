GCC_FLAGS = -Wall -Wextra -Werror -std=c2x -O2

INSTALL_DIR = /usr/local/bin
BUILD_DIR = ./build


all: $(BUILD_DIR)/net_clock $(BUILD_DIR)/change_time


$(BUILD_DIR)/net_clock: net_clock.c cap_drop.c
	mkdir -p -- $(BUILD_DIR)
	gcc $^ $(GCC_FLAGS) -lcap -o $@
	sudo chown root:root $@

$(BUILD_DIR)/change_time: change_time.c cap_drop.c
	mkdir -p -- $(BUILD_DIR)
	gcc $^ $(GCC_FLAGS) -lcap -o $@
	sudo chown root:root $@
	sudo setcap 'cap_sys_time=ep' $@

install: $(BUILD_DIR)/net_clock $(BUILD_DIR)/change_time
	sudo cp -f --preserve=all -t $(INSTALL_DIR) $^

uninstall:
	sudo rm -f -- $(INSTALL_DIR)/net_clock $(INSTALL_DIR)/change_time

clean:
	sudo rm -rf -- $(BUILD_DIR)  # sudo needed to remove root owned files
