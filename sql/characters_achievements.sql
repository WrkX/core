
CREATE TABLE IF NOT EXISTS `character_achievement` (
  `guid` INT unsigned NOT NULL,
  `achievement` SMALLINT unsigned NOT NULL,
  `date` INT unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`achievement`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;


CREATE TABLE IF NOT EXISTS `character_achievement_progress` (
  `guid` INT unsigned NOT NULL,
  `criteria` SMALLINT unsigned NOT NULL,
  `counter` INT unsigned NOT NULL,
  `date` INT unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`criteria`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
