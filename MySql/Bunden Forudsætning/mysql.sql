/*
 Navicat Premium Data Transfer

 Source Server         : localhost
 Source Server Type    : MySQL
 Source Server Version : 50643
 Source Host           : localhost:3306
 Source Schema         : skole

 Target Server Type    : MySQL
 Target Server Version : 50643
 File Encoding         : 65001

 Date: 03/04/2019 00:27:01
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for konto
-- ----------------------------
DROP TABLE IF EXISTS `konto`;
CREATE TABLE `konto`  (
  `Id` int(11) UNSIGNED NOT NULL DEFAULT 0 AUTO_INCREMENT,
  `EjerId` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `Saldo` double NULL DEFAULT NULL,
  `Posteringer` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `Oprettet` datetime(0) NULL DEFAULT CURRENT_TIMESTAMP(0) COMMENT 'Dato for oprettelse',
  `Slettet` datetime(0) NULL DEFAULT NULL COMMENT 'Data for sletning',
  PRIMARY KEY (`Id`, `EjerId`) USING BTREE,
  INDEX `Id`(`Id`) USING BTREE,
  INDEX `FK_KONTI_EJER_ID`(`EjerId`) USING BTREE,
  CONSTRAINT `FK_KONTI_EJER_ID` FOREIGN KEY (`EjerId`) REFERENCES `kunder` (`Id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Compact;

-- ----------------------------
-- Records of konto
-- ----------------------------
INSERT INTO `konto` VALUES (1, 1, 16000, 10, '2019-04-02 22:55:29', NULL);
INSERT INTO `konto` VALUES (2, 2, 48900, 18, '2019-04-02 22:55:29', NULL);

-- ----------------------------
-- Table structure for kunder
-- ----------------------------
DROP TABLE IF EXISTS `kunder`;
CREATE TABLE `kunder`  (
  `Id` int(11) UNSIGNED NOT NULL DEFAULT 0 AUTO_INCREMENT COMMENT 'Kundens Id',
  `Navn` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '' COMMENT 'Kundens Navn',
  `Oprettet` datetime(0) NULL DEFAULT CURRENT_TIMESTAMP(0) COMMENT 'Dato for oprettelse',
  `Slettet` datetime(0) NULL DEFAULT NULL COMMENT 'Data for sletning',
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Compact;

-- ----------------------------
-- Records of kunder
-- ----------------------------
INSERT INTO `kunder` VALUES (1, 'Bob', '2019-04-02 13:41:35', NULL);
INSERT INTO `kunder` VALUES (2, 'Per', '2019-04-02 13:41:44', NULL);
INSERT INTO `kunder` VALUES (3, 'Ole', '2019-04-02 14:11:16', NULL);

-- ----------------------------
-- Table structure for posteringer
-- ----------------------------
DROP TABLE IF EXISTS `posteringer`;
CREATE TABLE `posteringer`  (
  `Id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `KontoId` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `Værdi` double NOT NULL DEFAULT 0,
  `Dato` datetime(0) NOT NULL DEFAULT CURRENT_TIMESTAMP(0) ON UPDATE CURRENT_TIMESTAMP(0),
  PRIMARY KEY (`Id`) USING BTREE,
  INDEX `FK_POST_KONTO_ID`(`KontoId`) USING BTREE,
  CONSTRAINT `FK_POST_KONTO_ID` FOREIGN KEY (`KontoId`) REFERENCES `konto` (`Id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 30 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Compact;

-- ----------------------------
-- Records of posteringer
-- ----------------------------
INSERT INTO `posteringer` VALUES (1, 1, 100, '2019-04-02 13:52:30');
INSERT INTO `posteringer` VALUES (2, 1, -100, '2019-04-02 13:51:44');
INSERT INTO `posteringer` VALUES (3, 1, -200, '2019-04-02 13:52:10');
INSERT INTO `posteringer` VALUES (4, 1, 1000, '2019-04-02 13:53:12');
INSERT INTO `posteringer` VALUES (5, 1, 500, '2019-04-02 22:16:55');
INSERT INTO `posteringer` VALUES (6, 1, 4000, '2019-04-02 22:24:58');
INSERT INTO `posteringer` VALUES (7, 1, -400, '2019-04-02 22:38:09');
INSERT INTO `posteringer` VALUES (9, 1, -500, '2019-04-02 22:50:02');
INSERT INTO `posteringer` VALUES (10, 1, 1500, '2019-04-02 22:52:26');
INSERT INTO `posteringer` VALUES (11, 1, 10000, '2019-04-02 22:53:46');
INSERT INTO `posteringer` VALUES (12, 2, -100, '2019-04-02 23:42:10');
INSERT INTO `posteringer` VALUES (13, 2, -100, '2019-04-02 23:42:34');
INSERT INTO `posteringer` VALUES (14, 2, -100, '2019-04-02 23:43:52');
INSERT INTO `posteringer` VALUES (15, 2, -100, '2019-04-02 23:46:03');
INSERT INTO `posteringer` VALUES (16, 2, -100, '2019-04-02 23:46:20');
INSERT INTO `posteringer` VALUES (17, 2, -100, '2019-04-02 23:46:36');
INSERT INTO `posteringer` VALUES (18, 2, -100, '2019-04-02 23:47:47');
INSERT INTO `posteringer` VALUES (19, 2, -100, '2019-04-02 23:48:31');
INSERT INTO `posteringer` VALUES (20, 2, -100, '2019-04-02 23:51:07');
INSERT INTO `posteringer` VALUES (21, 2, -100, '2019-04-02 23:52:44');
INSERT INTO `posteringer` VALUES (22, 2, 50000, '2019-04-03 00:05:29');
INSERT INTO `posteringer` VALUES (23, 2, -100, '2019-04-03 00:12:05');
INSERT INTO `posteringer` VALUES (24, 2, -100, '2019-04-03 00:12:42');
INSERT INTO `posteringer` VALUES (25, 2, -100, '2019-04-03 00:12:55');
INSERT INTO `posteringer` VALUES (26, 2, -100, '2019-04-03 00:16:43');
INSERT INTO `posteringer` VALUES (27, 2, -100, '2019-04-03 00:17:14');
INSERT INTO `posteringer` VALUES (28, 2, -100, '2019-04-03 00:17:27');
INSERT INTO `posteringer` VALUES (29, 2, -500, '2019-04-03 00:22:39');

-- ----------------------------
-- Procedure structure for HævPenge
-- ----------------------------
DROP PROCEDURE IF EXISTS `HævPenge`;
delimiter ;;
CREATE PROCEDURE `HævPenge`(IN `Værdi` DOUBLE, IN `KontoId` INT, out `Rest` INT)
BEGIN
	DECLARE NySaldo INT;
	DECLARE AbsVærdi INT;
	DECLARE CUSTOM_EXCEPTION CONDITION FOR SQLSTATE '45000';
	
	SET @Rest = 0;
  SET AbsVærdi = ABS(@Værdi);
		
	IF ( SELECT EXISTS (SELECT 1 FROM konto WHERE konto.EjerId = @KontoId AND konto.Slettet IS NULL) ) THEN 
		SELECT Saldo INTO NySaldo FROM konto WHERE konto.EjerId = @KontoId;
		IF (NySaldo - AbsVærdi < 0) THEN
			SIGNAL CUSTOM_EXCEPTION
			SET MESSAGE_TEXT = 'Der er ikke nok pænge til at hæve beløbet';
		END IF;
		
		-- Data indsat i postering vil "trigger" insert trigger og opdatere saldo i konti
		INSERT INTO posteringer (KontoId, Værdi, Dato)
		-- Kan ikke finde nogen modsat ABS funktion så finder først positive, så negativ
		VALUES (@KontoId, -ABS(@Værdi), NOW());
		SET @Rest = NySaldo - AbsVærdi;
	ELSE
		SIGNAL CUSTOM_EXCEPTION
		SET MESSAGE_TEXT = 'Konto findes ikke.';
	END IF;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for IndsætPenge
-- ----------------------------
DROP PROCEDURE IF EXISTS `IndsætPenge`;
delimiter ;;
CREATE PROCEDURE `IndsætPenge`(IN `Værdi` INTEGER, IN `KontoId` DOUBLE, out Error INTEGER)
BEGIN
  -- Exit handler så den afslutter i tilfælde af fejl
	DECLARE EXIT HANDLER FOR SQLEXCEPTION SET Error = 1;   
	IF ( SELECT EXISTS (SELECT 1 FROM konto WHERE konto.EjerId = @KontoId AND konto.Slettet IS NULL) ) THEN
		SELECT 'Success';
		-- Data indsat i postering vil "trigger" insert trigger og opdatere saldo i konti
		INSERT INTO posteringer (KontoId, Værdi, Dato)
		-- Kan ikke finde nogen modsat ABS funktion så finder først positive, så negativ
		VALUES (@KontoId, ABS(@Værdi), NOW());
	ELSE
		SELECT 'Konti findes ikke.';
	END IF;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for OverførPenge
-- ----------------------------
DROP PROCEDURE IF EXISTS `OverførPenge`;
delimiter ;;
CREATE PROCEDURE `OverførPenge`(IN `Værdi` double,IN `AfsenderKonto` int, IN `ModtagerKonto` int)
BEGIN
	DECLARE AfsenderKontoSaldo INT(11) DEFAULT -1;
  DECLARE CUSTOM_EXCEPTION CONDITION FOR SQLSTATE '45000';
	
	IF (@AfsenderKonto = @ModtagerKonto) THEN
		SIGNAL CUSTOM_EXCEPTION
		SET MESSAGE_TEXT = 'Modtager og Afsender er ens.';
	END IF;
	
	-- Check Afsender Konto
	IF NOT EXISTS(SELECT (1) FROM konto WHERE konto.id = @AfsenderKonto AND konto.Slettet IS NULL) THEN
		SIGNAL CUSTOM_EXCEPTION
		SET MESSAGE_TEXT = 'Afsender Konto Findes Ikke.';
  END IF;
	
  -- Check Modtager Konto
	IF NOT EXISTS(SELECT (1) FROM konto WHERE konto.id = @ModtagerKonto AND konto.Slettet IS NULL) THEN
		SIGNAL CUSTOM_EXCEPTION
		SET MESSAGE_TEXT = 'Modtager Konto Findes Ikke.';
	END IF;
	
	-- Har nu verificeret at afsender of modtager eksistere og fået fat i afsender saldoen
	-- Kalder nu HævPenge proceduren som giver en rest tilbage
	CALL HævPenge(500, 1, @AfsenderKontoSaldo);
	SELECT @AfsenderKontoSaldo;
	
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for SletKonto
-- ----------------------------
DROP PROCEDURE IF EXISTS `SletKonto`;
delimiter ;;
CREATE PROCEDURE `SletKonto`(IN `KontoId` int, out Error INTEGER)
BEGIN
	DECLARE EXIT HANDLER FOR SQLEXCEPTION SET Error = 1;
	IF ( SELECT EXISTS (SELECT 1 FROM konto WHERE konto.EjerId = @KontoId AND konto.Slettet IS NULL) ) THEN
		SELECT 'Success';
		UPDATE konto SET konto.Slettet = NOW();
	ELSE
		SELECT 'Konto er allerede slettet.';
	END IF;

END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for SletKonto2
-- ----------------------------
DROP PROCEDURE IF EXISTS `SletKonto2`;
delimiter ;;
CREATE PROCEDURE `SletKonto2`(IN `KontoId` int, out Error INTEGER)
BEGIN
	DECLARE EXIT HANDLER FOR SQLEXCEPTION SET Error = 1;
	IF ( SELECT EXISTS (SELECT 1 FROM konto WHERE konto.EjerId = @KontoId AND konto.Slettet IS NULL) ) THEN
		SELECT 'Success';
		UPDATE konto SET konto.Slettet = NOW();
	ELSE
		SELECT 'Konto er allerede slettet.';
	END IF;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for Withdraw
-- ----------------------------
DROP PROCEDURE IF EXISTS `Withdraw`;
delimiter ;;
CREATE PROCEDURE `Withdraw`(IN `Værdi` DOUBLE, IN `KontoId` INT, out `Rest` INT)
BEGIN
	DECLARE NySaldo INT;
	DECLARE AbsVærdi INT;
	DECLARE CUSTOM_EXCEPTION CONDITION FOR SQLSTATE '45000';
	
	SET @Rest = 0;
  SET AbsVærdi = ABS(@Værdi);
		
	IF ( SELECT EXISTS (SELECT 1 FROM konto WHERE konto.EjerId = @KontoId AND konto.Slettet IS NULL) ) THEN 
		SELECT Saldo INTO NySaldo FROM konto WHERE konto.EjerId = @KontoId;
		IF (NySaldo - AbsVærdi < 0) THEN
			SIGNAL CUSTOM_EXCEPTION
			SET MESSAGE_TEXT = 'Der er ikke nok pænge til at hæve beløbet';
		END IF;
		
		-- Data indsat i postering vil "trigger" insert trigger og opdatere saldo i konti
		INSERT INTO posteringer (KontoId, Værdi, Dato)
		-- Kan ikke finde nogen modsat ABS funktion så finder først positive, så negativ
		VALUES (@KontoId, -ABS(@Værdi), NOW());
		SET @Rest = NySaldo - AbsVærdi;
	ELSE
		SIGNAL CUSTOM_EXCEPTION
		SET MESSAGE_TEXT = 'Konto findes ikke.';
	END IF;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table kunder
-- ----------------------------
DROP TRIGGER IF EXISTS `OnDeleteAccount`;
delimiter ;;
CREATE TRIGGER `OnDeleteAccount` AFTER DELETE ON `kunder` FOR EACH ROW BEGIN
	SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Kunder må ikke slettes';
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table posteringer
-- ----------------------------
DROP TRIGGER IF EXISTS `OnInsertPost`;
delimiter ;;
CREATE TRIGGER `OnInsertPost` AFTER INSERT ON `posteringer` FOR EACH ROW BEGIN
	UPDATE konto
     SET konto.Saldo = konto.Saldo + NEW.Værdi,
		 konto.Posteringer = konto.posteringer + 1
  WHERE konto.Id = NEW.KontoId;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table posteringer
-- ----------------------------
DROP TRIGGER IF EXISTS `PreventPostDeletion`;
delimiter ;;
CREATE TRIGGER `PreventPostDeletion` BEFORE DELETE ON `posteringer` FOR EACH ROW BEGIN
		SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Posteringer må ikke slettes';
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
