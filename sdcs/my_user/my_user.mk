################################################################################
#
# my_user
#
################################################################################

MY_USER_VERSION = 1.0.0
MY_USER_SOURCE = my_user-$(MY_USER_VERSION).tar.gz
MY_USER_SITE = http://dummyhost/distfiles


define MY_USER_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/bin/my_user
endef

$(eval $(autotools-package))