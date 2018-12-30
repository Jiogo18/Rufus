-- 25/12/2018
USE `ComptaMedicale`;
DROP TABLE IF EXISTS `Factures`;
CREATE TABLE `Factures` (
    `idFacture` INT NOT NULL AUTO_INCREMENT,
    `DateFacture` date DEFAULT NULL,
    `LienFichier` VARCHAR(250) NULL,
    `Intitule` VARCHAR(250) NULL,
    `Echeancier` INT(1) NULL,
    `idDepense` INT NULL,
    `pdf` LONGBLOB NULL,
    `jpg` LONGBLOB NULL,
    PRIMARY KEY (`idFacture`)
    ) ENGINE=InnoDB;
USE  `Images`;
DROP TABLE IF EXISTS `FacturesASupprimer`;
CREATE TABLE `FacturesASupprimer` (
    `idFacture` INT NOT NULL,
    `Echeancier` INT(1) NULL,
    `LienFichier` VARCHAR(250) NULL,
    PRIMARY KEY (`idFacture`)
    ) ENGINE=InnoDB;
ALTER TABLE `EchangeImages`
    CHANGE COLUMN `idImpression` `idImpression` INT(11) NULL ,
    DROP PRIMARY KEY;
UPDATE `rufus`.`ParametresSysteme` SET VersionBase = 51;

