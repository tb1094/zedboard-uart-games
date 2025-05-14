ROOT_DIR=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
PETALINUX_PROJ_NAME=zedboard-plnx
ABS_SW_PATH=$(ROOT_DIR)/$(PETALINUX_PROJ_NAME)
PETALINUX_DIR=$(ABS_SW_PATH)
PETALINUX_CONFIG=$(PETALINUX_DIR)/project-spec/configs/config
PETALINUX_ROOTFS_CONFIG=$(PETALINUX_DIR)/project-spec/configs/rootfs_config
$(info ROOT_DIR is: $(ROOT_DIR))

.PHONY: hardware, software, clean, all


all: hardware ${PETALINUX_DIR}

${PETALINUX_DIR}: software

software: petalinux_proj petalinux_build 

hardware: systemz_wrapper.xsa


update_config:
	cat $(ROOT_DIR)/adz-project-spec/config > $(PETALINUX_CONFIG)

update_rootfs_config:
	cp -r $(ROOT_DIR)/adz-project-spec/meta-user/* $(PETALINUX_DIR)/project-spec/meta-user
	cat $(ROOT_DIR)/adz-project-spec/rootfs_config > $(PETALINUX_ROOTFS_CONFIG)

petalinux_proj:
	mkdir -p $(ROOT_DIR);cd $(ROOT_DIR); petalinux-create -t project --template zynq --force -n $(PETALINUX_PROJ_NAME)
	petalinux-config -p $(PETALINUX_DIR) --get-hw-description=$(ROOT_DIR) --silentconfig
	$(MAKE) -f $(lastword $(MAKEFILE_LIST)) update_config
	$(MAKE) -f $(lastword $(MAKEFILE_LIST)) update_rootfs_config

petalinux_build: 
	petalinux-build -c kernel -p ${PETALINUX_DIR}
	petalinux-build -p $(PETALINUX_DIR)
	cd $(PETALINUX_DIR)/images/linux && petalinux-package --boot --fsbl zynq_fsbl.elf --fpga system.bit --u-boot u-boot.elf --force


clean:
	rm -rf ${PETALINUX_DIR}


