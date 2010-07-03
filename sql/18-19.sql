ALTER TABLE users ADD COLUMN `level` INT AFTER status;
UPDATE db_version SET version=19;