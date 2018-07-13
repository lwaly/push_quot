-- MySQL dump 10.13  Distrib 5.6.22, for Linux (x86_64)
--
-- Host: localhost    Database: dp_db
-- ------------------------------------------------------
-- Server version	5.6.22-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `tb_logo`
--

DROP TABLE IF EXISTS `tb_logo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_logo` (
  `id` bigint(20) NOT NULL COMMENT '台标唯一id',
  `user_id` int(11) NOT NULL COMMENT '台标所属用户id',
  `type` int(11) NOT NULL COMMENT '台标类型',
  `order` int(11) NOT NULL COMMENT '台标顺序',
  `size` int(11) NOT NULL COMMENT '台标大小',
  `picture_file_name` varchar(256) COLLATE utf8mb4_unicode_ci DEFAULT '' COMMENT '台标图片名称',
  `status` tinyint(4) DEFAULT '1' COMMENT '台标状态',
  `create_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '创建时间',
  `update_time` datetime NOT NULL DEFAULT '1970-01-01 00:00:00' COMMENT '最近更新时间',
  `fdfs_key` bigint(20) NOT NULL COMMENT '文件服务器id',
  `fdfs_group_name` varchar(256) COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '文件服务器组名',
  `fdfs_remote_file_name` varchar(256) COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '文件服务器文件名称',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_logo`
--

LOCK TABLES `tb_logo` WRITE;
/*!40000 ALTER TABLE `tb_logo` DISABLE KEYS */;
INSERT INTO `tb_logo` VALUES (3207869309128802304,1019540,1,1,1,'PicName',0,'2017-05-03 11:02:32','2017-05-03 11:02:32',123123123,'GroupName','RemoteFile'),(3207869708560760832,1019540,1,1,1,'PicName',0,'2017-05-03 11:05:38','2017-05-03 11:05:38',123123123,'GroupName','RemoteFile'),(3207890064558260224,1019540,1,1,1,'PicName',0,'2017-05-03 13:43:37','2017-05-03 13:43:37',123123123,'GroupName','RemoteFile'),(3207891911394197504,1019540,1,1,1,'PicName',1,'2017-05-03 13:57:57','2017-05-03 13:57:57',123123123,'GroupName','RemoteFile'),(3207898701737492480,1019540,1,1,1,'PicName',0,'2017-05-03 14:50:39','2017-05-03 14:50:39',123123123,'GroupName','RemoteFile'),(3207912954586464256,1019540,1,1,1,'PicName',1,'2017-05-03 16:41:16','2017-05-03 16:41:16',123123123,'GroupName','RemoteFile');
/*!40000 ALTER TABLE `tb_logo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-05-04 10:13:40
