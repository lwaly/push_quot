CREATE TABLE `tb_camera` (
  `ikey` int(11) NOT NULL AUTO_INCREMENT,
  `type_id` int(11) NOT NULL DEFAULT '0' COMMENT '摄像头类型ID',
  `cmr_brand` varchar(50) NOT NULL DEFAULT '',
  `cmr_time` int(11) NOT NULL DEFAULT '0',
  `create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  PRIMARY KEY (`ikey`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8;

CREATE TABLE `tb_audio_type` (
  `ikey` int(11) NOT NULL AUTO_INCREMENT,
  `parent_type_id` int(11) NOT NULL DEFAULT '0' COMMENT '父类id',
  `type_name` varchar(20) NOT NULL DEFAULT '' COMMENT '分类名',
  `orders` int(1) NOT NULL DEFAULT '0' COMMENT '分类',
  `flag` varchar(10) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `ctime` int(11) NOT NULL DEFAULT '0',
  `create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  PRIMARY KEY (`ikey`)
) ENGINE=InnoDB AUTO_INCREMENT=134 DEFAULT CHARSET=utf8;

CREATE TABLE `tb_audiolist` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '音频列表唯一ID',
  `audio_uuid` int(11) NOT NULL DEFAULT '0' COMMENT '用户id',
  `audio_name` varchar(110) NOT NULL DEFAULT '' COMMENT '显示给客户端的文件名',
  `audio_file` varchar(110) NOT NULL DEFAULT '' COMMENT '实际保存的文件名',
  `audio_duration` int(11) NOT NULL DEFAULT '0' COMMENT '时长',
  `audio_uptime` int(11) NOT NULL DEFAULT '0' COMMENT '上传时间',
  `audio_leixing` int(11) NOT NULL DEFAULT '0' COMMENT '音效的类型',
  `audio_type` int(1) NOT NULL DEFAULT '0' COMMENT '网页上传还是客户端调用接口',
  `audio_down` int(5) NOT NULL DEFAULT '0',
  `audio_pic` varchar(110) NOT NULL DEFAULT '',
  `audio_fdfs_key` bigint(20) NOT NULL DEFAULT '0',
  `audio_fdfs_group_name` varchar(110) NOT NULL DEFAULT '',
  `audio_fdfs_remote_file_name` varchar(256) NOT NULL DEFAULT '',
  `audio_status` int(1) NOT NULL DEFAULT '0',
  `audio_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `audio_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  `app_type` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'APP类型AP or VP',
  `audio_new_name` varchar(110) NOT NULL DEFAULT '' COMMENT '音频别名。用于我的上传列表显示的名称',
  `audit_status` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '审核状态，0：未审核；1：审核通过；2：审核未通过',
  PRIMARY KEY (`ikey`),
  KEY `leixing_index` (`audio_leixing`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `tb_audio_download` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '音频列表唯一ID',
  `audio_uid` int(11) NOT NULL DEFAULT '0' COMMENT '用户id',
  `audio_ikey` bigint(20) NOT NULL DEFAULT '0',
  `audio_dtime` int(11) NOT NULL DEFAULT '0' COMMENT '下载时间',
  `audio_status` int(1) NOT NULL DEFAULT '1',
  `audio_name` varchar(110) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `audio_file` varchar(110) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `audio_duration` int(11) NOT NULL DEFAULT '0',
  `audio_fdfs_key` bigint(20) NOT NULL DEFAULT '0',
  `audio_fdfs_group_name` varchar(110) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `audio_fdfs_remote_file_name` varchar(256) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `app_type` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'app 类型',
  `type_id` int(11) NOT NULL DEFAULT '0' COMMENT '音频类型ID',
  `audio_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `audio_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  PRIMARY KEY (`ikey`),
  UNIQUE KEY `id` (`audio_uid`,`audio_ikey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='音效下载记录';

CREATE TABLE `tb_pictures` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '画质列表唯一ID',
  `pc_name` varchar(110) NOT NULL DEFAULT '',
  `pc_file` varchar(50) NOT NULL DEFAULT '',
  `pc_uid` int(11) NOT NULL DEFAULT '0',
  `pc_brand` varchar(20) NOT NULL DEFAULT '',
  `pc_effect` varchar(20) NOT NULL DEFAULT '',
  `pc_camera` varchar(20) NOT NULL DEFAULT '',
  `pc_time` int(11) NOT NULL DEFAULT '0',
  `pc_view` int(11) unsigned zerofill NOT NULL DEFAULT '0',
  `pc_use` int(11) unsigned zerofill NOT NULL DEFAULT '0',
  `pc_status` int(1) NOT NULL DEFAULT '0',
  `pc_addtime` int(11) NOT NULL DEFAULT '0',
  `pc_fdfs_key` bigint(20) NOT NULL DEFAULT '0' COMMENT '文件服务器键值',
  `pc_fdfs_group_name` varchar(100) NOT NULL DEFAULT '',
  `pc_fdfs_remote_file_name` varchar(256) NOT NULL DEFAULT '',
  `pc_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `pc_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  `app_type` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'APP类型AP or VP',
  `new_name` varchar(110) NOT NULL DEFAULT '' COMMENT '画质别名',
  `type_id` int(11) NOT NULL DEFAULT '0' COMMENT '画质类型id',
  `audit_status` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '审核状态，0：未审核；1：审核通过；2：审核未通过',
  PRIMARY KEY (`ikey`),
  KEY `picture_camera` (`pc_camera`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `tb_pictures_download` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '画质列表唯一ID',
  `pc_uid` int(11) NOT NULL DEFAULT '0',
  `pc_ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '画质总表唯一ID',
  `pc_time` int(11) NOT NULL DEFAULT '0',
  `pc_inuse` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '1可用,0不可用',
  `pc_status` int(11) NOT NULL DEFAULT '1' COMMENT '下载的音频是否删除',
  `pc_name` varchar(110) NOT NULL DEFAULT '' COMMENT '画质名称',
  `pc_file` varchar(50) NOT NULL DEFAULT '' COMMENT '画质文件名称',
  `pc_brand` varchar(20) NOT NULL DEFAULT '' COMMENT '品牌名',
  `pc_effect` varchar(20) NOT NULL DEFAULT '' COMMENT '特效名',
  `pc_camera` varchar(20) NOT NULL DEFAULT '' COMMENT '摄像头名称',
  `pc_fdfs_key` int(11) NOT NULL DEFAULT '0' COMMENT '文件服务器键值',
  `pc_fdfs_group_name` varchar(110) NOT NULL DEFAULT '' COMMENT '文件服务器组名',
  `pc_fdfs_remote_file_name` varchar(256) NOT NULL DEFAULT '' COMMENT '文件服务器文件名',
  `type_id` int(11) NOT NULL DEFAULT '0' COMMENT '摄像头类型ID',
  `app_type` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'app类型AP,VP',
  `pc_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `pc_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  PRIMARY KEY (`ikey`),
  UNIQUE KEY `id` (`pc_uid`,`pc_ikey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `tb_effect` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '特效列表唯一ID',
  `effect_uuid` int(11) NOT NULL DEFAULT '0' COMMENT '用户id',
  `effect_name` varchar(110) NOT NULL DEFAULT '' COMMENT '显示给客户端的文件名',
  `effect_file` varchar(110) NOT NULL DEFAULT '' COMMENT '实际保存的文件名',
  `effect_duration` int(11) NOT NULL DEFAULT '0' COMMENT '时长',
  `effect_type` int(1) NOT NULL DEFAULT '0' COMMENT '网页上传还是客户端调用接口',
  `effect_down` int(5) NOT NULL DEFAULT '0' COMMENT '特效下载次数',
  `effect_pic` varchar(110) NOT NULL DEFAULT '',
  `effect_fdfs_key` bigint(20) NOT NULL DEFAULT '0',
  `effect_fdfs_group_name` varchar(110) NOT NULL DEFAULT '',
  `effect_fdfs_remote_file_name` varchar(256) NOT NULL DEFAULT '',
  `effect_status` int(1) NOT NULL DEFAULT '0',
  `effect_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `effect_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  `app_type` tinyint(4) NOT NULL unsigned DEFAULT '1' COMMENT 'APP类型AP or VP',
  `effect_new_name` varchar(110) NOT NULL DEFAULT '' COMMENT '特效别名。用于我的上传列表显示的名称',
  PRIMARY KEY (`ikey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `tb_effect_download` (
  `ikey` bigint(20) NOT NULL DEFAULT '0' COMMENT '特效列表唯一ID',
  `effect_uid` int(11) NOT NULL DEFAULT '0' COMMENT '用户id',
  `effect_ikey` bigint(20) NOT NULL DEFAULT '0',
  `effect_dtime` int(11) NOT NULL DEFAULT '0' COMMENT '下载时间',
  `effect_status` int(1) NOT NULL DEFAULT '1' COMMENT '是否已删除',
  `effect_name` varchar(110) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `effect_file` varchar(110) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `effect_duration` int(11) NOT NULL DEFAULT '0',
  `effect_fdfs_key` bigint(20) NOT NULL DEFAULT '0',
  `effect_fdfs_group_name` varchar(110) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `effect_fdfs_remote_file_name` varchar(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `app_type` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'app 类型',
  `effect_create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `effect_update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  PRIMARY KEY (`ikey`),
  UNIQUE KEY `id` (`effect_uid`,`effect_ikey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;