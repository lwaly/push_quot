CREATE TABLE `bsw_audio` (
  `id` bigint(20) NOT NULL DEFAULT '0',
  `uuid` bigint(20) DEFAULT NULL,
  `audio_name` varchar(256) COLLATE utf8_unicode_ci DEFAULT NULL,
  `audio_path` varchar(512) COLLATE utf8_unicode_ci DEFAULT NULL,
  `audio_duration` int(11) DEFAULT NULL,
  `fs_id` int(11) DEFAULT NULL,
  `fs_group_id` int(11) DEFAULT NULL,
  `fs_area_id` int(11) DEFAULT NULL,
  `audio_uptime` int(11) DEFAULT NULL,
  `audio_type` int(11) DEFAULT NULL,
  `audio_down` int(11) DEFAULT NULL,
  `audio_pic` varchar(512) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

