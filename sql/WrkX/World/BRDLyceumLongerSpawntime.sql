-- Bot Groups can make this area annoying/imposible, so a longer spawntime saves headaches
UPDATE creature 
SET `spawntimesecsmin` = 60, `spawntimesecsmax` = 60
WHERE `id` = 8901 and `spawntimesecsmax` = 30