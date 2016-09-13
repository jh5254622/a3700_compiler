################################################################################
#
# dmtest
#
################################################################################
DMTEST_SOURCE=
DMTEST_PACKAGE_NAME=dmtest
PRJ_BUILD_DIR = $(BUILD_DIR)/$(DMTEST_PACKAGE_NAME)-undefined
STRIP = $(TARGET_CROSS)strip

define dmtest_OPTS
	CC=$(TARGET_CROSS)gcc \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define DMTEST_BUILD_CMDS
	mkdir -p $(PRJ_BUILD_DIR)
	cp -rf package/$(DMTEST_PACKAGE_NAME)/src/* $(PRJ_BUILD_DIR)/
	$(dmtest_OPTS) \
		$(MAKE) -C $(PRJ_BUILD_DIR) -I$(PRJ_BUILD_DIR)/
endef

define DMTEST_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -s --strip-program=$(STRIP) $(PRJ_BUILD_DIR)/dmtest $(TARGET_DIR)/usr/bin/dmtest
endef

dmtest-clean:
	$(MAKE) -C $(PRJ_BUILD_DIR) clean
#	rm -rf $(PRJ_BUILD_DIR)

$(eval $(generic-package))
