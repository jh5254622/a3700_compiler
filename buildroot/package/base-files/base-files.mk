################################################################################
#
# base-files
#
################################################################################

# source included in buildroot
BASE_FILES_SOURCE =

define BASE_FILES_INSTALL_TARGET_CMDS
	mkdir -p  $(TARGET_DIR)/etc/
	mkdir -p  $(TARGET_DIR)/etc/init.d
	mkdir -p  $(TARGET_DIR)/etc/mdev
	mkdir -p  $(TARGET_DIR)/usr/
	mkdir -p  $(TARGET_DIR)/usr/factory
	mkdir -p  $(TARGET_DIR)/usr/data
	mkdir -p  $(TARGET_DIR)/usr/sbin

	$(INSTALL) -D -m 0755 package/base-files/files/etc/init.d/* $(TARGET_DIR)/etc/init.d/
	$(INSTALL) -D -m 0755 package/base-files/files/etc/mdev/* $(TARGET_DIR)/etc/mdev/
	$(INSTALL) -D -m 0755 package/base-files/files/etc/*.conf $(TARGET_DIR)/etc/
	$(INSTALL) -D -m 0755 package/base-files/files/usr/sbin/* $(TARGET_DIR)/usr/sbin/
endef

$(eval $(generic-package))
